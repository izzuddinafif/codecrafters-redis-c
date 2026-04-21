#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_digit(int n) {
  if (n == 0)
    return 1;
  int count = 0;
  long temp = labs(n);
  while (temp != 0) {
    temp /= 10;
    count++;
  }
  return count;
}

char *encode_bulk_str(char *str) {
  size_t len = strlen(str);
  int digit_count = count_digit((int)len);
  char *result = malloc(1 + len + 2 + (size_t)digit_count + 2 + 1);
  sprintf(result, "$%d\r\n%s\r\n", (int)len, str);
  return result;
}

// int main() {
//   char *encoded = encode_bulk_str("hey");
//   printf("%s\n", encoded);
//   free(encoded);
// }
