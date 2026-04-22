#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int table_init(Table *table, size_t capacity) {
  table->capacity = capacity;
  table->size = 0;
  table->pairs = malloc(sizeof(KV) * capacity);
  if (!table->pairs) {
    perror("init_table malloc");
    return -1;
  }
  return 0;
}

int table_find(Table *table, const char *key) {
  KV *pairs = table->pairs;
  int size = (int)table->size;

  for (int i = 0; i < size; ++i) {
    if (strcmp(pairs[i].key, key) == 0) {
      return i;
    }
  }
  return -1;
}

char *table_get(Table *table, const char *key) {
  int idx = table_find(table, key);
  if (idx >= 0) {
    return table->pairs[idx].value;
  }
  return nullptr;
}

int table_set(Table *table, const char *key, const char *value) {

  if (table->size + 1 >= table->capacity) {
    table->capacity *= 2;
    KV *temp = realloc(table->pairs, table->capacity * sizeof(KV));
    if (!temp) {
      perror("table_find realloc");
      return -1; // realloc failure
    }
    table->pairs = temp;
  }

  int idx_existing = table_find(table, key);
  if (idx_existing >= 0) {
    // the existing value lives in the heap, thus need to be freed
    free(table->pairs[idx_existing].value);
    table->pairs[idx_existing].value = strdup(value);
    return 0;
  }

  table->pairs[table->size].key = strdup(key);
  table->pairs[table->size].value = strdup(value);
  table->size += 1;

  return 0;
}

void table_destroy(Table *table) {
  int size = (int)table->size;

  for (int i = 0; i < size; ++i) {
    free(table->pairs[i].key);
    free(table->pairs[i].value);
  };
  free(table->pairs);
  table->size = 0;
  table->capacity = 0;
  table->pairs = nullptr;
}
