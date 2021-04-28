#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTACK 256

int eval_rpn(char *s) {
  int len, i, num;
  int st[MAXSTACK];
  int n_st = 0;

  len = strlen(s);
  num = -1;
  for (i = 0; i < len; i++) {
    if (s[i] >= '0' && s[i] <= '9') {
      if (num == -1)
        num = 0;
      num = 10 * num + (int)(s[i] - '0');
    } else if (s[i] == '+') {
      st[n_st - 2] += st[n_st - 1];
      n_st--;
    } else if (s[i] == '-') {
      st[n_st - 2] -= st[n_st - 1];
      n_st--;
    } else if (s[i] == ' ') {
      if (num >= 0)
        st[n_st++] = num;
      num = -1;
    }
  }

  return st[0];
}

int main() {
  assert(eval_rpn("1 2 +") == 3);
  assert(eval_rpn("1 2 + 10 +") == 13);
  assert(eval_rpn("1 2 - 10 +") == 9);
  assert(eval_rpn("1 2 10 + +") == 13);
  return 0;
}
