#include "utf8helper.h"

int main() {
    /*
      \u#### - 2B
      \U######## - 4B


      0 0001 0000 0011 0100 1000 # 11110000 10010000 10001101 10001000     # F0 90 8D 88
      000-010000-001101-001000   # 11110-...-10-......-10-......-10-...... # F0 90 8D 88
      000-010000-001101-001000   # 11110-000-10-010000-10-001101-10-001000 # F0 90 8D 88



      Header:
      0------- = 1B
      110----- = 2B
      1110---- = 3B
      11110--- = 4B

      Parts:
      10------

      Need:
      10000000 - Check for 1B
      11000000 - Check for part
      11100000 - Check for 2B
      11110000 - Check for 3B
      11111000 - Check for 4B


      data & <check>

     */

    char a[] = "04dc"; // \u
    char b[] = "D55C"; // \u

    char* c = utf8_SmallCodePoint2Unicode(a);
    printf("%s == \u04dc\n", c);
    free(c);

    c = utf8_SmallCodePoint2Unicode(b);
    printf("%s == \ud55c\n", c);
    free(c);

    char d[] = "\u04dc";
    c = utf8_ToSmallCodePoint(d);
    printf("%s == 04DC\n", c);
    free(c);
    return 0;
}
