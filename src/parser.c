#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN 10

char *parse_line(char *start, const char *terminator) {

  char *end = strstr(start, terminator);
  if (end == nullptr) {
    return nullptr;
  }
  size_t len = (size_t)(end - start);
  char *token = malloc(len + 1);
  if (!token) {
    perror("malloc");
    return nullptr;
  }
  memcpy(token, start, len);
  token[len] = '\0';

  return token;
}

char **split_str_by_str_terminator(char *str, const char *terminator,
                                   int *count) {
  struct {
    char **entries;
    int capacity;
  } result;
  result.capacity = MAX_TOKEN;
  result.entries = malloc(sizeof(char *) * (size_t)result.capacity);
  if (!result.entries) {
    perror("malloc");
    return nullptr;
  }
  char *start = str;
  (*count) = 0;

  while (1) {
    char *token = parse_line(start, terminator);
    if (token == nullptr) {
      break;
    }

    if (*count + 2 == result.capacity) {
      char **new_ptr =
          realloc(result.entries, sizeof(char *) * (size_t)result.capacity * 2);
      if (!new_ptr) {
        perror("realloc");
        return nullptr;
      } else {
        result.entries = new_ptr;
        result.capacity *= 2;
      }
    }
    result.entries[(*count)++] = token;
    start += strlen(token) + strlen(terminator);
  }
  result.entries[(*count)] = nullptr;
  return result.entries;
}

void free_split(char **tokens, int count) {
  if (!tokens)
    return;
  for (int i = 0; i < count || tokens[i] != nullptr; ++i) {
    free(tokens[i]);
  }
  free(tokens);
}

// int main() {
//   char *text =
//   "apple\r\n\r\nbanana\r\ngrape\r\napple\r\n\r\nbanana\r\ngrape\r"
//                "\napple\r\n\r\nbanana\r\ngrape\r\napple\r\n\r\nbanana\r\ngrape"
//                "\r\napple\r\n\r\nbanana\r\ngrape\r\napple\r\n\r\nbanana\r\ngrap"
//                "e\r\napple\r\n\r\nbanana\r\ngrape";
//   int count;
//   char **tokens = split_str_by_str_terminator(text, "\r\n", &count);

//   for (int i = 0; i < count; i++) {
//     printf("tokens[%d] = '%s'\n", i, tokens[i]);
//   }

//   free_split(tokens, count);
//   return 0;
// }
