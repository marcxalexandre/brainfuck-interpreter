/** iteratates through the `code` string and searches for the next closing bracket on the same level.
*
 * Returns -2 if there is no matching bracket or -1 if the character at index `i` is not an open bracket
 *
 * On success, returns the index of the matching bracket.
 *
 */
long find_matching_close_bracket(const char* code, long i) {
  if (code[i] == '[') {
    int depth = 1;

    for (long j = i + 1; code[j] != '\0'; ++j) {
      if (code[j] == '[')
        ++depth;
      else if (code[j] == ']')
        --depth;
      else
        continue;

      if (depth == 0) return j;
    }

    return -2; // No matching close bracket
  }

  return -1;
}

/** iteratates through the `code` string backwards and searches for the next open bracket on the same level.
 *
 * Returns -2 if there is no matching bracket or -1 if the character at index `i` is not a closing bracket
 *
 * On success, returns the index of the matching bracket.
 *
 */
long find_matching_open_bracket(const char* code, long i) {
  if (code[i] == ']') {
    int depth = 1;

    for (long j = i - 1; j >= 0; --j) {
      if (code[j] == ']')
        ++depth;
      else if (code[j] == '[')
        --depth;
      else
        continue;

      if (depth == 0) return j;
    }

    return -2; // No matching open bracket
  }

  return -1;
}