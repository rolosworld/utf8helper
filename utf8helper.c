/*

  MIT License

  Copyright (c) 2021 Rolando Gonzalez-Chevere

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/
#include "utf8helper.h"
#include <stdio.h>
#include <stdlib.h>

char utf8_char2value(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    return -1;
}

char utf8_value2char(char c) {
    if (c >= 0 && c <= 9) {
        return c + '0';
    } else if (c >= 10 && c <= 15) {
        return c + 'A' - 10;
    }

    return -1;
}

unsigned char utf8_nibbles2byte(const char* _n) {
    return (_n[0] << 4) | _n[1];
}

unsigned short utf8_SmallCodePoint2Bin(const char* _s) {
    char v[4] = {0};
    v[0] = utf8_char2value(_s[0]);
    v[1] = utf8_char2value(_s[1]);
    v[2] = utf8_char2value(_s[2]);
    v[3] = utf8_char2value(_s[3]);

    unsigned short r;
    ((unsigned char*)&r)[1] = utf8_nibbles2byte(v);
    ((unsigned char*)&r)[0] = utf8_nibbles2byte(v+2);

    return r;
}

char utf8_SmallBytesNeeded(unsigned short _p) {
    if (_p < 0x0080) {
        return 1;
    }
    else if (_p < 0x0800) {
        return 2;
    }

    return 3;
}

char* utf8_SmallCodePoint2Unicode(const char* _s) {
    unsigned short point = utf8_SmallCodePoint2Bin(_s);
    unsigned char size = utf8_SmallBytesNeeded(point);

    char* r = malloc(size+1);
    r[size] = '\0';

    if (size == 1) {
        r[0] = ((char*)&point)[1];
    } else if (size == 2) {
        unsigned char a = (((unsigned char*)&point)[1] << 2) & 0x1C;
        unsigned char b = (((unsigned char*)&point)[0] >> 6) & 0x03;
        unsigned char c = ((unsigned char*)&point)[0] & 0x3F;
        r[0] = 0xC0 | a | b;
        r[1] = 0x80 | c;
    } else if (size == 3) {
        unsigned char a = (((unsigned char*)&point)[1] & 0xF0) >> 4;
        unsigned char b = (((unsigned char*)&point)[1] & 0x0F) << 2;
        unsigned char c = (((unsigned char*)&point)[0] & 0xC0) >> 6;
        unsigned char d = ((unsigned char*)&point)[0] & 0x3F;
        r[0] = 0xE0 | a;
        r[1] = 0x80 | b | c;
        r[2] = 0x80 | d;
    }

    return r;
}

char* utf8_ToSmallCodePoint(const char* _s) {
    // 1000000000
    unsigned char one = 0x80;

    // 1100000000
    unsigned char part = 0xC0;

    // 1110000000
    unsigned char two = 0xE0;

    // 1111000000
    unsigned char three = 0xF0;

    unsigned char t1 = 0;
    unsigned char t2 = 0;
    if ((*_s & three) == two || (*_s & two) == part) {
        if ((*(_s+1) & part) != one) {
            return NULL;
        }

        if ((*_s & three) == two) {
            if ((*(_s+2) & part) != one) {
                return NULL;
            }
            // 3B type
            t1 = ((_s[0] & 0x0f) << 4) | ((_s[1] & 0x3C) >> 2);
            t2 = (_s[1] << 6) | (_s[2] & 0x3F);
        }
        else if ((*_s & two) == part) {
            // 2B type
            t1 = (_s[0] & 0x1C) >> 2;
            t2 = (_s[1] & 0x3F) | (_s[0] & 0x03 << 6);
        }

        char*r = malloc(7);
        r[0] = '\\';
        r[1] = 'u';
        r[2] = utf8_value2char(t1 >> 4 & 0x0f);
        r[3] = utf8_value2char(t1 & 0x0f);
        r[4] = utf8_value2char(t2 >> 4 & 0x0f);
        r[5] = utf8_value2char(t2 & 0x0f);
        r[6] = '\0';
        return r;
    }
    else if ((*_s & one) == 0) {
        char*r = malloc(2);
        r[0] = *_s;
        r[1] = '\0';
        return r;
    }

    return NULL;
}

ssize_t utf8_charCount(const char* b) {
    // 1000000000
    unsigned char one = 0x80;
    
    // 1100000000
    unsigned char part = 0xC0;

    // 1110000000
    unsigned char two = 0xE0;
    
    // 1111000000
    unsigned char three = 0xF0;
    
    // 1111100000
    unsigned char four = 0xF8;

    size_t l = 0;
    while (*b != '\0') {
        if ((*b & four) == three) {
            l += 10; // \Uxxxxxxxx
            if ((*(b+1) & part) != one ||
                (*(b+2) & part) != one ||
                (*(b+3) & part) != one) {
                return -1;
            }
            b += 4;
        }
        else if ((*b & three) == two) {
            l += 6; // \uxxxx
            if ((*(b+1) & part) != one ||
                (*(b+2) & part) != one) {
                return -1;
            }
            b += 3;
        }
        else if ((*b & two) == part) {
            l += 6; // \uxxxx
            if ((*(b+1) & part) != one) {
                return -1;
            }
            b += 2;
        }
        else if ((*b & one) == 0) {
            l++;
            b++;
        }
        else {
            return -1;
        }
    }

    return l;
}

char utf8_IsValid(const char* _s) {
  if (_s == NULL) {
    return 0;
  }

  ssize_t l = utf8_charCount(_s);
  if (l < 0) {
    return 0;
  }

  return 1;
}
