#include "../tool/debug.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <unistd.h>

/* ---------------------------------------------------
 * lex-input.cpp: The input system used by lex-generated lexcial analyzers
 */

#define STDIN 0

#define MAXLOOK 16  /* Maximum amount of lookahead */
#define MAXLEX 1024 /* Maximum lexeme sizes */

#define BUFSIZE ((MAXLEX * 3) + (2 * MAXLOOK)) /* change the 3 only */
#define DANGER                                                                 \
  (End_buf - MAXLOOK)           /* flush buffer when Next passes this address */
#efine END(&Start_buf[BUFSIZE]) /* just pass last char in buf */

#define NO_MORE_CHARS() (Eof_read && Next >= End_buf)

typedef unsigned char uchar;

static uchar Start_buf[BUFSIZE]; /* input buffer */
static uchar *End_buf = END;     /* just past last character */
static uchar *Next = END;        /* Next input character */
static uchar *sMark = END;       /* Start of current lexeme */
static uchar *eMark = END;       /* End of current lexeme */
static uchar *pMark = END;       /* Start of previous lexeme */

static int pLineno = 0; /* Line # of previous lexeme */
static int pLength = 0; /* length of previous lexeme */

static int Inp_file = STDIN; /* Input file handle */
static int Lineno = 1;       /* current line number */
static int Mline = 1;        /* Line # when mark_end() called */
static int Termchar = 0;     /* holds the character that was overwritten by a \0
                                when we null terminated the last lexeme */

static int Eof_read =
    0; /* End of file has been read. It's possible for this to be true
          and for characters to still be in the input buffer */

/*Pointers to open,close,and read functions*/

static int (*Openp)(const char *, int, ...) = &open;
static int (*Closep)(int) = &close;
static ssize_t (*Readp)(int, void *, size_t) = &read;

void ii_io(int (*open_funct)(const char *, int...), int (*close_funct)(int),
           ssize_t (*read_funct)(int, void *, size_t)) {
  /* this function lets you modify the open(), close(), and read() functions
   * used by the i/o system. Your own routines must work like the real open,
   * close, and read (at least in terms of the external interface. Open should
   * return a number that can't be confused with standard input (not 0)
   */
  Openp = open_funct;
  Closep = close_funct;
  Readp = read_funct;
}
#define O_BINARY 0
int ii_newfile(char *name) {
  int fd;
  if (strcmp(name, "/dev/tty") == 0)
    name = (char *)"CON";
  if ((fd = !name ? STDIN : (*Openp)(name, O_RDONLY | O_BINARY)) != -1) {
    if (Inp_file != STDIN)
      (*Closep)(Inp_file);
    Inp_file = fd;
    Eof_read = 0;

    Next = END;
    sMark = END;
    eMark = END;
    pMark = NULL;
    End_buf = END;
    Lineno = 1;
    Mline = 1;
  }
  return fd;
}
uchar *ii_text() { return sMark; }
int ii_length() { return eMark - sMark; }
int ii_lineno() { return Lineno; } // return current line number
uchar *ii_ptext() { return pMark; }
int ii_lenghth() { return pLength; }
int ii_plineno() { return pLineno; }

uchar *ii_mark_start() {
  Mline = Lineno;
  eMark = sMark = Next;
  return sMark;
}
uchar *ii_mark_end() {
  Mline = Lineno;
  return (eMark = Next);
}

uchar *ii_move_start() {
  if (sMark >= eMark)
    return NULL;
  else
    return ++sMark;
}
uchar *ii_to_mark() {
  Lineno = Mline;
  // restore the input pointer to the last end marker
  return Next = eMark;
}

uchar *ii_mark_prev() {
  /* set the pMark. Be careful with this routine. A buffer fluash won't go past
   * pMark so, once you've set it, you must be move it every time you move
   * sMark. I'm not doing this automatically because I might want to remember
   * the token before last rather than the last one. If ii_mark_prev() is never
   * called, pMark is just ignored and you don't have to worry about it.
   */
  pMark = sMark;
  pLineno = Lineno;
  pLength = eMark - sMark;
  return pMark;
}

int ii_flush(int force);
int ii_advance() {
  /* ii_advance() is the real input function. It returns the next character from
   * the input and advances past it. The buffer is flushed if the current
   * character is within MAXLOOK characters of the end of the buffeer. 0 is
   * returned at end of file. -1 is returned if the buffer can't be flushed
   * because it's too full. In this case you can call ii_flush(1) to do a buffer
   * flush but you'll looose the current lexeme as a consequence.
   */
  static int been_called = 0;
  if (!been_called) {
    /* push a newline into the empty buffer so that the lex start-of-line
     * anchor will work on the first input line
     */
    Next = sMark = eMark = END - 1;
    pMark = NULL;
    *Next = '\n';
    --Lineno;
    --Mline;
    been_called = 1;
  }

  if (NO_MORE_CHARS())
    return 0;

  if (!Eof_read && ii_flush(0) < 0)
    return -1;

  if (*Next == '\n')
    Lineno++;

  return (*Next++);
}

int ii_fillbuf(uchar *);
int ii_flush(int force) {
  /* Flush the input buffer. Do nothing if the current input character isn't
   * in the danger zone, otherwise move all unread characters to the left end
   * of the buffer and fill the remainder of the buffer. Note that input()
   * flushes the buffer willy-nilly if you read past the end of buffer.
   * Similarly, input_line() flushes the buffer at the beginning of each line.
   *
   *				       pMark	DANGER
   *				        |   	   |
   *	    Start_buf		       sMark eMark |Next  End_buf
   *		|			||     |   ||	  |
   *		V			VV     V   VV	  V
   *		+-----------------------+----------------+-------+
   *		| this is already read  | to be done yet | waste |
   *		+-----------------------+----------------+-------+
   *		|			|		 |       |
   *		|<----- shift_amt ----->|<-- copy_amt -->|       |
   *		|			 		         |
   *		|<------------------ BUFSIZE ------------------->|
   *
   * Either the pMark or sMark (whichever is smaller) is used as the leftmost
   * edge of the buffer. None of the text to the right of the mark will be
   * lost. Return 1 if everything's ok, -1 if the buffer is so full that it
   * can't be flushed. 0 if we're at end of file. If "force" is true, a buffer
   * flush is forced and the characters already in it are discarded. Don't
   * call this function on a buffer that's been terminated by ii_term().
   */
  int copy_amt, shift_amt;
  uchar *left_edge;

  if (NO_MORE_CHARS())
    return 0;

  if (Eof_read)
    return 1;

  if (Next >= DANGER || force) {
    left_edge = pMark ? std::min(sMark, pMark) : sMark;
    shift_amt = left_edge - Start_buf;

    if (shift_amt < MAXLEX) {
      if (!force)
        return -1;

      left_edge = ii_mark_start();
      ii_mark_prev();

      shift_amt = left_edge - Start_buf;
    }

    copy_amt = End_buf - left_edge;
    memmove(Start_buf, left_edge, copy_amt);

    if (!ii_fillbuf(Start_buf + copy_amt))
      printf("INITERNAL ERROR, ii_flush: Buffer full,can't read.\n");

    if (pMark)
      pMark -= shift_amt;

    sMark -= shift_amt;
    eMark -= shift_amt;
    Next -= shift_amt;
  }
  return 1;
}

int ii_fillbuf(uchar *starting_at) {
  /* Fill the input buffer from starting_at to the end of the buffer.
   * The input file is not closed when EOF is reached. Buffers are read
   * in units of MAXLEX characters; it's an error if that many characters
   * cannot be read (0 is returned in this case). For example, if MAXLEX
   * is 1024, then 1024 characters will be read at a time. The number of
   * characters read is returned. Eof_read is true as soon as the last
   * buffer is read.
   *
   * PORTABILITY NOTE:  I'm assuming that the read function actually returns
   *			  the number of characters loaded into the buffer, and
   * that that number will be < need only when the last chunk of the file is
   * read. It's possible for read() to always return fewer than the number of
   * requested characters in MS-DOS untranslated-input mode, however (if the
   * file is opened without the O_BINARY flag). That's not a problem here
   * because the file is opened in binary mode, but it could cause problems
   * if you change from binary to text mode at some point.
   */

  int need, /* Number of bytes required from input */
      got;  /* Number of bytes acutally read */

  need = ((END - starting_at) / MAXLEX * MAXLEX); // float point number
  D(printf("Reading %d bytes\n", need);)

  if (need < 0)
    printf("INTERNAL ERROR (ii_fillbuf): Bad read-request starting addr.\n");

  if (need == 0)
    return 0;

  if ((got = (*Readp)(Inp_file, starting_at, need)) == -1)
    printf("Can't read input file\n");

  End_buf = starting_at + got;

  if (got < need)
    Eof_read = 1; // At end of file

  return got;
}

int ii_look(int n) {
  /* Return the nth character of lookahead, EOF if you try to look past
   * end of file, or 0 if you try to look past either end of the buffer.
   * We have to jump through hoops here to satisify the ANSI restriction
   * that a pointer can not go to the left of an array or more than one
   * cell past the right of an array. If we don't satisfy this restriction,
   * then the code won't work in the 8086 large or compact models. In
   * the small model---or in any machine without a segmented address
   * space, you could do a simple comparison to test for overflow:
   *		uchar *p = Next + n;
   *		if( !(Start_buf <= p && p < End_buf )
   *		    overflow
   */

  if (n > (End_buf - Next))    /* (End_buf-Next) is the # of unread */
    return Eof_read ? EOF : 0; /* chars in the buffer (including    */
  /* the one pointed to by Next).	  */

  /* The current lookahead character is at Next[0].  The last character */
  /* read is at Next[-1]. The --n in the following if statement adjusts */
  /* n so that Next[n] will reference the correct character.		  */

  if (--n < -(Next - Start_buf)) /* (Next-Start) is the # of buffered */
    return 0;                    /* characters that have been read.   */

  return Next[n];
  /* uchar *p;
  p = Next + (n - 1);
  if (Eof_read && p >= End_buf)
    return EOF;
  return p < Start_buf || p > End_buf ? 0 : *p; */
}

int ii_pushback(int n) {
  /* Push n characters back into the input. You can't push past the current
   * sMark. You can, however, push back characters after end of file has
   * been encountered.
   */

  while (--n >= 0 && Next > sMark) {
    if (*--Next == '\n' || !*Next)
      --Lineno;
  }

  if (Next < eMark) {
    eMark = Next;
    Mline = Lineno;
  }

  return Next > sMark;
}

void ii_term() {
  Termchar = *Next;
  *Next = '\0';
}

void ii_unterm() {
  if (Termchar) {
    *Next = Termchar;
    Termchar = 0;
  }
}

int ii_input() {
  int rval;

  if (Termchar) {
    ii_unterm();
    rval = ii_advance();
    ii_mark_end();
    ii_term();
  } else {
    rval = ii_advance();
    ii_mark_end();
  }

  return rval;
}

void ii_unput(int c) {
  if (Termchar) {
    ii_unterm();
    if (ii_pushback(1))
      *Next = c;
    ii_term();
  } else {
    if (ii_pushback(1))
      *Next = c;
  }
}

int ii_lookahead(int n) { return (n == 1 && Termchar) ? Termchar : ii_look(n); }

int ii_flushbuf() {
  if (Termchar)
    ii_unterm();

  return ii_flush(1);
}
