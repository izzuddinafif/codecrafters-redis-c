#include "strlib.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool str_starts_with(const char *str, const char *prefix) {
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool str_ends_with(const char *str, const char *suffix) {
  if (!str || !suffix)
    return false;
  size_t len_str = strlen(str);
  size_t len_suffix = strlen(suffix);

  if (len_str < len_suffix) {
    return false;
  }

  size_t start = len_str - len_suffix;
  // pointer arithmetic, str is char[], it decays to pointer to the first char
  // of str
  return (strcmp((str + start), suffix) == 0);
}

void str_remove_all_whitespace_inplace(char *str) {
  if (!str)
    return;

  char *write = str;
  char *read = str;
  while (*read) {
    if (!isspace((unsigned char)*read)) {
      // assign current char in read to write then increment both
      *write++ = *read++;
      continue;
    }
    read++;
  }
  *write = '\0';
}

void str_trim_leading_whitespace_inplace(char *str) {
  if (!str)
    return;

  char *start = str;
  while (*start && isspace((unsigned char)*start)) {
    start++;
  }
  // include null terminator (+ 1)
  memmove(str, start, strlen(start) + 1);
}

void str_trim_trailing_whitespace_inplace(char *str) {
  if (!str)
    return;

  char *end = str + strlen(str) - 1;
  while (end >= str && isspace((unsigned char)*end)) {
    end--;
  }
  *(end + 1) = '\0';
}

void str_trim_whitespace_inplace(char *str) {
  if (!str) {
    return;
  }
  str_trim_leading_whitespace_inplace(str);
  str_trim_trailing_whitespace_inplace(str);
}

int str_to_int(const char *str, int *out) {
  if (!str || !out)
    return 1;
  char *end_ptr;
  // errno can be set to any non-zero value by a library function call
  // regardless of whether there was an error, so it needs to be cleared
  // in order to check the error set by strtol
  errno = 0; // reset errno

  // base 10
  long value = strtol(str, &end_ptr, 10);

  // On systems where long is larger than int (which is common on 64-bit
  // systems), a number might fit in a long but not in an int
  if (value < INT_MIN || value > INT_MAX)
    return 1; // Value doesn't fit in an int

  if (str == end_ptr)
    return 1; // no digits found, end_ptr stil points to str (first char)

  if (errno == ERANGE)
    return 1; // out of range error / overflow

  if (*end_ptr != '\0')
    return 1; // non-number after number e.g. 123abc (optional tho)

  *out = (int)value;
  return 0; // success
}

void str_to_upper(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = (char)toupper(
        (unsigned char)str[i]); // Cast to unsigned char for safety
  }
}

void str_to_lower(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = (char)tolower(
        (unsigned char)str[i]); // Cast to unsigned char for safety
  }
}

// char s[12] = "  hey yo  ";

// int main() {
// void (*f[])(char *) = {str_trim_leading_whitespace_inplace,
//                       str_trim_trailing_whitespace_inplace,
//                       str_remove_all_whitespace_inplace};

//   printf("'%s' with len of %llu\n", s, strlen(s));

//   for (size_t i = 0; i < sizeof(f) / sizeof(f[0]); i++) {
//     f[i](s);
//     printf("'%s' with len of %llu\n", s, strlen(s));
//   }
// }
