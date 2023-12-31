/*@A (C) 1992 Allen I. Holub                                                */

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tools/debug.h>
#include <tools/l.h>

/*------------------------------------------------------------------------
 * INPUT.C:   The input system used by LeX-generated lexical analyzers.
 *------------------------------------------------------------------------
 * System-dependent defines.
 */

#define STDIN 0 /* standard input */

/*----------------------------------------------------------------------*/

#define MAXLOOK 16 /* Maximum amount of lookahead */
#define MAXLEX 1024 /* Maximum lexeme sizes.	   */

#define BUFSIZE ((MAXLEX * 3) + (2 * MAXLOOK)) /* Change the 3 only  */

#define DANGER (End_buf - MAXLOOK) /* Flush buffer when Next      */
/* passes this address         */

#define END (&Start_buf[BUFSIZE]) /* Just past last char in buf  */

#define NO_MORE_CHARS() (Eof_read && Next >= End_buf)

typedef unsigned char uchar;

PRIVATE uchar Start_buf[BUFSIZE]; /* Input buffer			*/
PRIVATE uchar* End_buf = END; /* Just past last character		*/
PRIVATE uchar* Next = END; /* Next input character		*/
PRIVATE uchar* sMark = END; /* Start of current lexeme		*/
PRIVATE uchar* eMark = END; /* End of current lexeme		*/
PRIVATE uchar* pMark = NULL; /* Start of previous lexeme 	*/
PRIVATE int pLineno = 0; /* Line # of previous lexeme	*/
PRIVATE int pLength = 0; /* Length of previous lexeme	*/

PRIVATE int Inp_file = STDIN; /* Input file handle 		*/
PRIVATE int Lineno = 1; /* Current line number	 	*/
PRIVATE int Mline = 1; /* Line # when mark_end() called	*/
PRIVATE int Termchar = 0; /* Holds the character that was	*/
/* overwritten by a \0 when we	*/
/* null terminated the last		*/
/* lexeme.				*/
PRIVATE int Eof_read = 0; /* End of file has been read.	*/
/* It's possible for this to be	*/
/* true and for characters to	*/
/* still be in the input buffer.	*/

/* Pointers to open, close, and read functions */

PRIVATE int(*Openp) P((char*, int)) = (int(*) P((char*, int)))open;
PRIVATE int(*Closep) P((int)) = close;
PRIVATE int(*Readp) P((int, void*, unsigned)) = read;
void ii_io(open_funct, close_funct, read_funct) int(*open_funct) P((char* name, int mode));
int(*close_funct) P((int handle));
int(*read_funct) P((int handle, void* target_buffer, unsigned len));
{
    /* This function lets you modify the open(), close(), and read() functions
     * used by the i/o system. Your own routines must work like the real open,
     * close, and read (at least in terms of the external interface. Open should
     * return a number that can't be confused with standard input (not 0).
     */

    Openp = open_funct;
    ferror
        Closep
        = close_funct;
    Readp = read_funct;
}
/*-------------------------------------------------------------------*/
int ii_newfile(name)
char* name;
{
    /* Prepare a new input file for reading. If newfile() isn't called before
     * input() or input_line() then stdin is used. The current input file is
     * closed after successfully  opening the new one (but stdin isn't closed).
     *
     * Return -1 if the file can't be opened; otherwise, return the file
     * descriptor returned from open(). Note that the old input file won't be
     * closed unless the new file is opened successfully. The error code (errno)
     * generated by the bad open() will still be valid, so you can call perror()
     * to find out what went wrong if you like. At least one free file
     * descriptor must be available when newfile() is called. Note in the open
     * call that O_BINARY, which is needed in MS-DOS applications, is mapped
     * to 0 under UNIX (with a define in <tools/debug.h>).
     */

    int fd; /* File descriptor */

    MS(if (strcmp(name, "/dev/tty") == 0))
    MS(name = "CON";)

    if ((fd = !name ? STDIN : (*Openp)(name, O_RDONLY | O_BINARY)) != -1) {
        if (Inp_file != STDIN)
            (*Closep)(Inp_file);

        Inp_file = fd;
        Eof_read = 0;

        Next = END;
        sMark = END;
        pMark = NULL;
        eMark = END;
        End_buf = END;
        Lineno = 1;
        Mline = 1;
    }
    return fd;
}
PUBLIC uchar* ii_text() { return (sMark); }
PUBLIC int ii_length() { return (eMark - sMark); }
PUBLIC int ii_lineno() { return (Lineno); }
PUBLIC uchar* ii_ptext() { return (pMark); }
PUBLIC int ii_plength() { return (pLength); }
PUBLIC int ii_plineno() { return (pLineno); }

uchar* ii_mark_start()
{
    Mline = Lineno;
    eMark = sMark = Next;
    return (sMark);
}

PUBLIC uchar* ii_mark_end()
{
    Mline = Lineno;
    return (eMark = Next);
}

PUBLIC uchar* ii_move_start()
{
    if (sMark >= eMark)
        return NULL;
    else
        return ++sMark;
}

PUBLIC uchar* ii_to_mark()
{
    Lineno = Mline;
    return (Next = eMark);
}

uchar* ii_mark_prev()
{
    /* Set the pMark. Be careful with this routine. A buffer flush won't go past
     * pMark so, once you've set it, you must move it every time you move sMark.
     * I'm not doing this automatically because I might want to remember the
     * token before last rather than the last one. If ii_mark_prev() is never
     * called, pMark is just ignored and you don't have to worry about it.
     */

    pLineno = Lineno;
    pLength = eMark - sMark;
    return (pMark = sMark);
}

int ii_advance()
{
    /* ii_advance() is the real input function. It returns the next character
     * from input and advances past it. The buffer is flushed if the current
     * character is within MAXLOOK characters of the end of the buffer. 0 is
     * returned at end of file. -1 is returned if the buffer can't be flushed
     * because it's too full. In this case you can call ii_flush(1) to do a
     * buffer flush but you'll loose the current lexeme as a consequence.
     */

    static int been_called = 0;

    if (!been_called) {
        /*  Push a newline into the empty buffer so that the LeX start-of-line
         *  anchor will work on the first input line.
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

int ii_flush(force)
int force;
{
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
    uchar* left_edge;

    if (NO_MORE_CHARS())
        return 0;

    if (Eof_read) /* nothing more to read */
        return 1;

    if (Next >= DANGER || force) {
        left_edge = pMark ? min(sMark, pMark) : sMark;
        shift_amt = left_edge - Start_buf;

        if (shift_amt < MAXLEX) /* if(not enough room) */
        {
            if (!force)
                return -1;

            left_edge = ii_mark_start(); /* Reset start to current character */
            ii_mark_prev();

            shift_amt = left_edge - Start_buf;
        }

        copy_amt = End_buf - left_edge;
        memmove(Start_buf, left_edge, copy_amt);

        if (!ii_fillbuf(Start_buf + copy_amt))
            ferr("INTERNAL ERROR, ii_flush: Buffer full, can't read.\n");

        if (pMark)
            pMark -= shift_amt;

        sMark -= shift_amt;
        eMark -= shift_amt;
        Next -= shift_amt;
    }

    return 1;
}

/*----------------------------------------------------------------------*/

PRIVATE int ii_fillbuf(starting_at)
unsigned char* starting_at;
{
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

    int need, /* Number of bytes required from input.	*/
        got; /* Number of bytes actually read.	*/

    need = ((END - starting_at) / MAXLEX) * MAXLEX;

    D(printf("Reading %d bytes\n", need);)

    if (need < 0)
        ferr("INTERNAL ERROR (ii_fillbuf): Bad read-request starting addr.\n");

    if (need == 0)
        return 0;

    if ((got = (*Readp)(Inp_file, starting_at, need)) == -1)
        ferr("Can't read input file\n");

    End_buf = starting_at + got;

    if (got < need MS(&&eof(Inp_file)))
        Eof_read = 1; /* At end of file */

    return got;
}

int ii_look(n)
int n;
{
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

    if (n > (End_buf - Next)) /* (End_buf-Next) is the # of unread */
        return Eof_read ? EOF : 0; /* chars in the buffer (including    */
    /* the one pointed to by Next).	  */

    /* The current lookahead character is at Next[0].  The last character */
    /* read is at Next[-1]. The --n in the following if statement adjusts */
    /* n so that Next[n] will reference the correct character.		  */

    if (--n < -(Next - Start_buf)) /* (Next-Start) is the # of buffered */
        return 0; /* characters that have been read.   */

    return Next[n];
}
int ii_pushback(n)
int n;
{
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

    return (Next > sMark);
}
void ii_term()
{
    Termchar = *Next;
    *Next = '\0';
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

void ii_unterm()
{
    if (Termchar) {
        *Next = Termchar;
        Termchar = 0;
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

int ii_input()
{

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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

void ii_unput(c) int c;
{
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

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

int ii_lookahead(int n)
{
    return (n == 1 && Termchar) ? Termchar : ii_look(n);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

int ii_flushbuf()
{
    if (Termchar)
        ii_unterm();

    return ii_flush(1);
}
