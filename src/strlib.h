#pragma once

bool str_starts_with(const char *string, const char *prefix);
bool str_ends_with(const char *string, const char *suffix);
void str_remove_all_whitespace_inplace(char *string);
void str_trim_whitespace_inplace(char *string);
void str_trim_leading_whitespace_inplace(char *string);
void str_trim_trailing_whitespace_inplace(char *string);
int str_to_int(const char *string, int *out);
void str_to_upper(char *str);
void str_to_lower(char *str);
