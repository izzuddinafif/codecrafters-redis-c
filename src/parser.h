#pragma once

// Splits string by a terminator string. Make sure the strings are properly
// terminated. Uses both token count and null sentinel value.
char **split_str_by_str_terminator(char *str, const char *terminator,
                                   int *count);
void free_split(char **tokens, int count);
char *parse_line(char *start, const char *terminator);
