#include "compiler.h"
#include <cctype>
#include <cmath>
#include <iostream>
void testfunc() { std::cout << "testfunc" << std::endl; }
double mean(int reset, double sample, double *dev) {
  static double m_xhat, m_ki, d_xhat, d_ki;
  double mean;

  if (reset)
    return (m_ki = m_xhat = d_xhat = 0.0);

  m_xhat += (sample - m_xhat) / ++m_ki;
  mean = m_xhat;
  *dev = sqrt(d_xhat += (pow(fabs(mean - sample), 2.0) - d_xhat) / ++d_ki);

  return mean;
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
bool isHexDigit(char x) {
  return isdigit(x) || ('a' <= x && x <= 'f') || ('A' <= x && x <= 'F');
}
bool isOctDigit(char x) { return '0' <= x && x <= '7'; }

int esc(const char *&s) {
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

  if (*s != '\\')
    rval = *(s++);
  else {
    ++s; /* skip the \ */
    switch (*s) {
    case '\0':
      rval = '\\';
      break;
    case 'b':
      rval = '\b';
      break;
    case 'f':
      rval = '\f';
      break;
    case 'n':
      rval = '\n';
      break;
    case 'r':
      rval = '\r';
      break;
    case 's':
      rval = ' ';
      break;
    case 't':
      rval = '\t';
      break;
    case 'e':
      rval = '\033';
      break;
    case '^':
      rval = *(++s);
      rval = toupper(rval) - '@'; // @(40) A(41)
      break;
    case 'x':
      rval = 0;
      ++s;
      if (isHexDigit(*s)) {
        // * precedence greater than ++
        rval = hex2bin(*(s++));
      }
      if (isHexDigit(*s)) {
        rval <<= 4;
        rval |= hex2bin(*(s++));
      }
      if (isHexDigit(*s)) {
        rval <<= 4;
        rval |= hex2bin(*(s++));
      }
      --s; /* \xa7a *s point at a */
      break;
    default:
      if (!isOctDigit(*s))
        rval = *s;
      else {
        ++s;
        rval = oct2bin(*(s++));
        if (isOctDigit(*s)) {
          rval <<= 3;
          rval |= oct2bin(*(s++));
        }
        if (isOctDigit(*s)) {
          rval <<= 3;
          rval |= oct2bin(*(s++));
        }
        --s;
      }
      break;
    }
    ++s; /* \xa7a *s point at the character after a */
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

void pchar(int c, FILE *stream) {
  /* like putc(), except that control characters are mapped to human-readable
   * string */
  fputs(bin_to_ascii(c, 1), stream);
}
