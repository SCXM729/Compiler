#include "compiler.h"
#include <cmath>
#include <cstring>
#include <errno.h>
#include <filesystem>
#include <iostream>
/* most qsort() implementations use a "qucik sort", which does not
 * behave particularly well with small arrays or arrays that might
 * already be sorted. Shell sort is used.
 *    The basic strategy of Shells sort is to partition the array into several
 * smaller subarrays whose elements are spreaded out over the original array.
 * The subarrays are sorted using a bubble sort, and then the array is
 * repartitioned into a smaller number of subarrays, each having more elements.
 * The process is continued until there's only one subarray comprising the
 * entire array.
 */
/* base address of array, the number of elements, the size of one element, a
 * pointer to a comparsion dunction (negative if *p1<*p2, zero if *p1==*p2,
 * positive if *p1>*p2)*/
void ssort(char *base, int numele, int elesize, int (*cmp)(void *, void *)) {
  int i, j;
  /* The distance between any two elements in the subarray is called the gap
   * size. */
  int gap, k, tmp;
  char *p1, *p2;

  /* The gap is selected from a number in the series: 1, 4, 13, 40, 121,
   * ...(3N+1). */
  for (gap = 1; gap <= numele; gap = 3 * gap + 1)
    ;

  for (gap /= 3; gap > 0; gap /= 3) {
    // gradual exchange
    for (i = gap; i < numele; i++) {
      for (j = i - gap; j >= 0; j -= gap) {
        /* pay attention to "j-=gap", because array[j] had been swaped with
         * array[j-gap] to ensure array[j-gap] < array[j] , then is swaped with
         * array[j+gap], so we can't make sure the array[j] is smaller than
         * array[j-gap]
         */
        p1 = base + j * elesize;
        p2 = base + (j + gap) * elesize;
        if ((*cmp)(p1, p2) <= 0)
          break;
        // swap data in byte level
        for (k = elesize; --k >= 0;) {
          tmp = *p1;
          *p1++ = *p2;
          *p2++ = tmp;
        }
      }
    }
  }
}

/* A version of ssort() optimized to sort arrays of pointers. The elesize
 * argument is ignored.
 */
void assort(void **base, int numele, int elesize,
            int (*cmp)(void **, void **)) {
  int i, j, gap;
  void *tmp, **p1, **p2;

  for (gap = 1; gap <= numele; gap = 3 * gap + 1)
    ;

  for (gap /= 3; gap > 0; gap /= 3) {
    for (i = gap; i < numele; i++) {
      for (j = i - gap; j >= 0; j -= gap) {
        p1 = base + (j);
        p2 = base + (j + gap);

        if ((*cmp)(p1, p2) <= 0)
          break;

        tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
      }
    }
  }
}

unsigned long cstoul(char **instr) {
  /* Convert string to long. If string starts with 0x it is interpreted
   * as a hex number, else if it starts with 0 it is octal, else it is
   * decimal. Conversion stops on encountering the first character which
   * is not a digit in the indicated radix. *instr is updated to point
   * past the end of the number.
   */

  unsigned long num = 0;
  char *str = *instr;
  while (std::isspace(*str))
    ++*str;
  if (*str != '0') {
    while (std::isdigit(*str))
      num = num * 10 + (*str++ - '0');
  } else {
    if (*++str == 'x' || *str == 'X') {
      for (++str; std::isxdigit(*str); ++str)
        num = num * 16 +
              (std::isdigit(*str) ? *str - '0' : std::toupper(*str) - 'A' + 10);
    } else {
      while ('0' <= *str && *str <= '7') {
        num *= 8;
        num += *str++ - '0';
      }
    }
  }
  *instr = str;
  return num;
}

long cstol(char **instr) {
  /* like stoul(), but recognizes a leading minus sign and returns a
   * signed long.
   */

  while (std::isspace(**instr))
    ++*instr;

  if (**instr != '-')
    return (long)(cstoul(instr));
  else {
    ++*instr;
    return -(long)(cstoul(instr));
  }
}

/* map escape sequences to single character. */
int hex2bin(int c) {
  /* Convert the hex digit represented by 'c' to an int. 'c' must be one
   * of the following characters: 0123456789abcdefABCDEF 7 --> 7 A -->
   * 10
   */
  return (std::isdigit(c) ? (c) - '0' : ((std::toupper(c)) - 'A') + 10) & 0xf;
}
int oct2bin(int c) {
  /* Convert the hex digit represented by 'c' to an int. 'c' must be a
   * digit in the range '0'-'7'
   */
  return ((c) - '0') & 0x7;
}

/* && procedence greater than || */
#define ISHEXDIGIT(x)                                                          \
  (isdigit(x) || ('a' <= (x) && (x) <= 'f') || ('A' <= (x) && (x) <= 'F'))
#define ISOCTDIGIT(x) ('0' <= (x) && (x) <= '7')

int esc(char **s) {
  /* Map escape sequences into their equivalent symbols. Return the
   * equivalent ASCII character. *s is advanced past the escape
   * sequence. If no scape sequence is present, the current character is
   * returned and the string is advanced by one. The following are
   * recognized:
   *
   *   \b     backspace
   *   \f     formfeed
   *   \n     newline
   *   \r     carriage return
   *   \s     space
   *   \t     tab
   *   \e     ASCII ESC characters ('\033')
   *   \DDD   number formed of 1-3 octal digits
   *   \xDDD  number formed of 1-3 hex digits (two required)
   *   \^C    C = any letter. Control code
   *
   *   \b --> 0x08  \e --> 0x1b
   *   \^C --> 0x??
   *   \DDD --> 0x?? \xDDD --> 0x??
   *   \c --> c      c --> c
   */

  int rval;

  if (**s != '\\')
    rval = *((*s)++);
  else {
    ++(*s);                      /* skip the \ */
    switch (std::toupper(**s)) { /* attention to the toupper() */
    case '\0':
      rval = '\\';
      break;
    case 'B':
      rval = '\b';
      break;
    case 'F':
      rval = '\f';
      break;
    case 'N':
      rval = '\n';
      break;
    case 'R':
      rval = '\r';
      break;
    case 'S':
      rval = ' ';
      break;
    case 'T':
      rval = '\t';
      break;
    case 'E':
      rval = '\033';
      break;
    case '^':
      rval = *++(*s);
      rval = std::toupper(rval) - '@'; // @(40) A(41)
      break;
    case 'X':
      rval = 0;
      ++(*s);
      if (ISHEXDIGIT(**s)) {
        // * precedence greater than ++
        rval = hex2bin(*(*s)++);
      }
      if (ISHEXDIGIT(**s)) {
        rval <<= 4;
        rval |= hex2bin(*(*s)++);
      }
      if (ISHEXDIGIT(**s)) {
        rval <<= 4;
        rval |= hex2bin(*(*s)++);
      }
      --(*s); /* \xa7a *s point at a */
      break;
    default:
      if (!ISOCTDIGIT(**s))
        rval = **s;
      else {
        ++(*s);
        rval = oct2bin(*(*s)++);
        if (ISOCTDIGIT(**s)) {
          rval <<= 3;
          rval |= oct2bin(*(*s)++);
        }
        if (ISOCTDIGIT(**s)) {
          rval <<= 3;
          rval |= oct2bin(*(*s)++);
        }
        --(*s);
      }
      break;
    }
    ++(*s); /* \xa7a *s point at the character after a */
  }
  return rval;
}

char *bin_to_ascii(int c, int use_hex) {
  /* Return a pointer to a string that represented c. This will be the
   * character itself for normal characters and an escape sequence (\n,
   * \t, \x00, etc, for most others). A ' is represented as \'. The
   * string will be destroyed the next time bin_to_ascii() is called. If
   * "use_hex" is true then \xDD escape sequences are used. Otherwise,
   * octal sequences (\DDD) are used. (see also: pchar.c)
   */
  static char buf[8];
  c &= 0xff; // let c less then 255
  // the value of ' ' is 30
  if (' ' <= c && c < 0x7f && c != '\'' && c != '\\') {

    buf[0] = c;
    buf[1] = '\0';
  } else {
    buf[0] = '\\';
    buf[2] = '\0';
    switch (c) {
    case '\\':
      buf[1] = '\\';
      break;
    case '\'':
      buf[1] = '\'';
      break;
    case '\b':
      buf[1] = 'b';
      break;
    case '\f':
      buf[1] = 'f';
      break;
    case '\t':
      buf[1] = 't';
      break;
    case '\r':
      buf[1] = 'r';
      break;
    case '\n':
      buf[1] = 'n';
      break;
    default:
      sprintf(&buf[1], use_hex ? "x%03x" : "%03o", c);
      break;
    }
  }
  return buf;
}

int ferr(const char *fmt, ...) {
  /* Note that ferr() is typed as int, even though it usually doesn't
   * return, because it's occasionally useful to use it inside a
   * conditional expression where a type will be required.
   */
  /* print fatal error message and terminated the program. */

  va_list args;
  va_start(args, fmt);
  if (fmt)
    prnt(fputc, stderr, fmt, args);
  else
    perror(va_arg(args, char *));
  va_end(args);
  exit(on_ferr());
}

int on_ferr() {
  /* This is the default routine called by ferr when it exits. It should
   * return the exit status. You can supply your own version of this
   * routine if you like.
   */
  // extern int errno; There are errors
  return errno;
}

void fputstr(char *str, int maxlen, FILE *stream) {
  /* writes a stirng(str) having at most maxlen characters to the
   * indicated stream in human-readable form
   */
  char *s;
  while (*str && maxlen >= 0) {
    s = bin_to_ascii(*str++, 1);
    while (*s && --maxlen >= 0)
      putc(*s++, stream);
  }
}

void pchar(int c, FILE *stream) {
  /* like putc(), except that control characters are mapped to
   * human-readable string.*/
  fputs(bin_to_ascii(c, 1), stream);
}

void printv(FILE *fp, const char *argv[]) {
  /* Print an argv-like array of pointers to string, one string per
   * line. The array must be NULL terminated.
   */
  while (*argv)
    fprintf(fp, "%s\n", *argv++);
}

void comment(FILE *fp, const char **argv) {
  /* works like printv except that the array is printed as C comment.*/
  fprintf(fp, "\n/*----------------------------------------------------\n");
  while (*argv)
    fprintf(fp, " * %s\n", *argv++);
  fprintf(fp, " */\n\n");
}

void prnt(int (*ofunct)(int, FILE *), FILE *funct_arg, const char *format,
          va_list args) {
  char buf[256], *p;
  vsprintf(buf, format, args);
  for (p = buf; *p; p++)
    (*ofunct)(*p, funct_arg);
}

#define ERR_NONE 0
#define ERR_DST_OPEN -1
#define ERR_SRC_OPEN -2
#define ERR_READ -3
#define ERR_WRITE -4

int copyfile(const char *dst, const char *src, const char *mode) {
  /* Copy the src to the destination file, opening the destination in the
   * indicated mode. Note that the buffer size used on the first call is used on
   * subsequent calls as well. Return values are defined, above. errno will hold
   * the appropriate error code if the return value is <0.
   */
  FILE *fd_dst, *fd_src;
  char *buf;
  int got;
  int werror;
  int ret_val = ERR_NONE;
  static unsigned size = 31 * 1024;
  /* prevent insufficient space acquisition */
  while (size > 0 && !(buf = (char *)malloc(size)))
    size -= 1024;

  if (!size) /* couldn't get a buffer, do it one byte at a time */
  {
    size = 1;
    buf = (char *)"xx"; /* allocate a buffer implicitly */
  }

  fd_src = fopen(src, "r");
  if (fd_src == NULL) {
    std::cout << "the src file doesn't exit" << std::endl;
    return ERR_SRC_OPEN;
  }

  FILE *test = fopen(dst, "r");
  if (test == NULL) {
    std::cout << "the dst file doesn't exit" << std::endl;
    return ERR_DST_OPEN;
  } else {
    fclose(test);
  }

  fd_dst = fopen(dst, mode);
  while (1) {
    fread(buf, size, 1, fd_src);
    if (!std::feof(fd_src)) {
      fwrite(buf, size, 1, fd_dst);
      memset(buf, 0, size);
    } else {
      fwrite(buf, strlen(buf), 1, fd_dst);
      break;
    }
  }
  fclose(fd_src);
  fclose(fd_dst);
  return ret_val;
}

int movefile(const char *dst, const char *src, const char *mode) {
  /* works like copyfile() but delete src if the copy is successful */
  int rval;
  if ((rval = copyfile(dst, src, mode)) == 0)
    remove(src);
  return rval;
}

int *memiset(int *dst, int with_what, int count) {
  /* works like memset but fills integer arrays with an integer value. The count
   * is the number of ints (not the number pf bytes). */
  int *targ;
  for (targ = dst; --count >= 0; *targ++ = with_what)
    ;
  return dst;
}

int concate(int size, char *dst, ...) {
  /* This subroutine concates an arbitrary number of strings into a single
   * destination array (dst) of size "size." At most size-1 characters are
   * copied. The amount of available space is required or -1 if the string was
   * truncated. Use it like this:
   *    char target[SIZE];
   *    concat(SIZE,target,"first","second","...","last",NULL);
   */
  const char *src;
  va_list args;
  va_start(args, dst);
  while ((src = va_arg(args, const char *)) && size > 1)
    while (*src && size-- > 1)
      *dst++ = *src++;

  *dst++ = '\0';
  va_end(args);
  /* subtle differences in return values */
  return size <= 1 ? -1 : size;
}

#define PBUF_SIZE 129 /* Maxium length of a path name */
void searchenv(const char *filename, const char *envname, char *pathname) {
  /* Search for file by looking in the current directory or the directories
   * listed in the envname environment. Put the full path name (if you find it)
   * into pathname. Otherwise set *pathname to 0. Unlike the DOS PATH command
   * (and the microsoft _searchenv), you can use either a space or semicolon to
   * separate directory names. The pathname array must be at least 128
   * characters.
   */
  char pbuf[PBUF_SIZE];
  char *p;

  strcpy(pathname, filename);
  namespace fs = std::filesystem;
  fs::path file_path = std::string(pathname);
  if (fs::exists(file_path))
    return; /* find in current directory */

  /* The file doesn't exist in the current directory. If a specific path was
   * requested (ie. file contains \ or /) or if the environment isn't set,
   * return a NULL, else search for file on the path.
   */
  if (strpbrk(filename, "\\/") || !(p = getenv(envname))) {
    *pathname = '\0';
    return;
  }

  strncpy(pbuf, p, PBUF_SIZE);
  if ((p = strtok(pbuf, ";"))) {
    do {
      sprintf(pathname, "%s/%s", p, filename);
      if (fs::exists(pathname))
        return; /* found it */
    } while ((p = strtok(NULL, ";")));
  }
  *pathname = '\0';
}

static FILE *Input_file = NULL;
static int Input_line;     /* line number of most-recently read line */
static char File_name[80]; /* template-file name */

FILE *driver_1(FILE *output, int lines, const char *file_name) {
  /* two routines work together to transfer a template file to a LEX or occs
   * outputfile. driver_1 must be called first. if lines is true, then a #line
   * directive that references the template file's current line number is output
   * just before the block is output, return NULL if the template file can't be
   * opened, otherwise the FILE pointer for the template is returned. close file
   * using it after it has been copied.
   */
  char path[80];

  if (!(Input_file = fopen(file_name, "r"))) {
    searchenv(file_name, "LIB", path);
    if (!*path || !(Input_file = fopen(path, "r")))
      return NULL;
  }

  strncpy(File_name, file_name, sizeof(File_name));
  Input_line = 0;
  driver_2(output, lines);
  return Input_file;
}

int driver_2(FILE *output, int lines) {
  static char buf[256];
  char *p;
  int processing_comment = 0;

  if (!Input_file)
    ferr("INTERNAL ERROR [driver_2], Template file not open.\n");
  if (lines)
    fprintf(output, "\n#line %d \"%s\"\n", Input_line + 1, File_name);

  while (fgets(buf, sizeof(buf), Input_file)) {
    ++Input_line;
    if (*buf == '\f')
      break;
    for (p = buf; isspace(*p); ++p)
      ;

    if (*p == '@') { /* ignore @ sign */
      processing_comment = 1;
      continue;
    } else if (processing_comment) {
      processing_comment = 0;
      if (lines)
        fprintf(output, "\n#line %d \"%s\"\n", Input_line, File_name);
    }
    fputs(buf, output);
  }
  return feof(Input_file);
}

void defnext(FILE *fp, const char *name) {
  /* Print the default yy_next(s,c) subroutine for an
   * uncompressed table
   */
  static const char *comment_text[] = {
      "yy_next(state,c) is given the current state and input character and",
      "evaluates to the next state", NULL};
  comment(fp, comment_text);
  fprintf(fp, "#define yy_next(state,c) %s[state][c]\n", name);
}

double mean(int reset, double sample, double *dev) {
  static double m_xhat, m_ki, d_xhat, d_ki;
  double mean;

  if (reset)
    return (m_ki = m_xhat = d_ki = d_xhat = 0.0);

  m_xhat += (sample - m_xhat) / ++m_ki;
  mean = m_xhat;
  *dev = sqrt(d_xhat += (pow(fabs(mean - sample), 2.0) - d_xhat) / ++d_ki);

  return mean;
}
