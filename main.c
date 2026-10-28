#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BRAINFUCK_TOKEN_INC '+'
#define BRAINFUCK_TOKEN_DEC '-'
#define BRAINFUCK_TOKEN_NEXT '>'
#define BRAINFUCK_TOKEN_PREV '<'
#define BRAINFUCK_TOKEN_OUTPUT '.'
#define BRAINFUCK_TOKEN_INPUT ','
#define BRAINFUCK_LOOP_START '['
#define BRAINFUCK_LOOP_END ']'
#define BRAINFUCK_TAPE_SIZE 10000

static char* buffer = NULL;

extern long find_matching_close_bracket(const char* code, long i);
extern long find_matching_open_bracket(const char* code, long i);

_Noreturn void cleanup(const char* msg, int status) {
  if (msg) fprintf(stderr, "RuntimeError: %s\n", msg);
  if (buffer) free(buffer);

  exit(status);
}

static int read_all(int fd) {
  int r;
  char c;
  size_t bufsize = 10, i = 0;

  while ((r = read(fd, &c, 1)) != -1 && r != 0) {
    if (!buffer || i >= bufsize) {
      bufsize *= 2;

      void *newptr;
      if ((newptr = realloc(buffer, bufsize * sizeof(char))) != NULL) {
        buffer = (char *) newptr;
      } else {
        free(buffer);
        return -1;
      }
    }

    buffer[i++] = c;
  }

  if (buffer) {
    buffer[i] = '\0';
    buffer = realloc(buffer, strlen(buffer) + 1);
    return 0;
  } else {
    return -1;
  }
}


static void brainfuck_execute(const char* code, const long size) {
  unsigned char tape[BRAINFUCK_TAPE_SIZE] = {0};
  int tape_index = 0;

  for (long i = 0; i < size; ++i) {
    switch (code[i]) {
      case BRAINFUCK_TOKEN_NEXT:
        if (++tape_index >= BRAINFUCK_TAPE_SIZE) cleanup("Pointer overflow", EXIT_FAILURE);
        break;
      case BRAINFUCK_TOKEN_PREV:
        if (--tape_index < 0) cleanup("Pointer underflow", EXIT_FAILURE);
        break;
      case BRAINFUCK_TOKEN_INC:
        if (tape[tape_index] < 0xff) ++tape[tape_index];
        else cleanup("cell value overflow", EXIT_FAILURE);
        break;
      case BRAINFUCK_TOKEN_DEC:
        if (tape[tape_index] > 0) --tape[tape_index];
        else cleanup("cell value underflow", EXIT_FAILURE);
        break;
      case BRAINFUCK_TOKEN_OUTPUT:
        putchar(tape[tape_index]);
        break;
      case BRAINFUCK_TOKEN_INPUT:
        int chr = getchar();
        fseek(stdin, 0, SEEK_END); // clear stdin
        tape[tape_index] = chr;
        break;
      case BRAINFUCK_LOOP_START:
        if (tape[tape_index] == 0) {
          long j = find_matching_close_bracket(code, i);

          if (j == -2) {
            cleanup("No matching close bracket", EXIT_FAILURE);
          }
        }
        break;
      case BRAINFUCK_LOOP_END:
        if (tape[tape_index] != 0) {
          long j = find_matching_open_bracket(code, i);

          if (j == -2) {
            cleanup("No matching open bracket", EXIT_FAILURE);
          }

          i = j - 1;
        }
        break;
      default: { /* ignore char */ }
    }
  }
}

int main(int argc, char** argv) {
  int fd = argc > 1 ? open(argv[1], O_RDONLY | O_NONBLOCK): STDIN_FILENO;

  if (fd < 0) {
    perror("could not open file!");
    return EXIT_FAILURE;
  }

  // read entire content from file descriptor
  if (read_all(fd) < 0) {
    perror("could not read from input!");
    return EXIT_FAILURE;
  }

  // close fd if it points to a file
  if (fd != STDIN_FILENO) close(fd);

  // start analyzing brainfuck code
  brainfuck_execute(buffer, strlen(buffer));

  return 0;
}
