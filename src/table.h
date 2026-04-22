#pragma once
#include <stddef.h>

typedef struct {
  char *key;
  char *value;
} KV;

typedef struct {
  KV *pairs;
  size_t capacity;
  size_t size;
} Table;

int table_init(Table *table, size_t capacity);
int table_find(Table *table, const char *key);
int table_set(Table *table, const char *key, const char *value);
char *table_get(Table *table, const char *key);
void table_destroy(Table *table);
