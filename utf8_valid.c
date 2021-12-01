#include "utf8helper.h"

int main(int argc, const char* argv[]) {
  if (argc < 2) {
    printf("Missing file to read.\n");
    return 1;
  }

  char * str = 0;
  size_t len;
  FILE * f = fopen (argv[1], "rb");

  if (f) {
      fseek(f, 0, SEEK_END);
      len = ftell(f);
      fseek(f, 0, SEEK_SET);
      str = malloc(len);
      if (str) {
	fread(str, 1, len, f);
      }
      fclose(f);
  }
  
  if (!str) {
    printf("Unable to read file: %s\n", argv[1]);
    return 1;
  }

  if (utf8_IsValid(str, len)) {
    printf("VALID UTF8: %s\n", argv[1]);
  } else {
    printf("NOT UTF8: %s\n", argv[1]);
  }

  return 0;
}
