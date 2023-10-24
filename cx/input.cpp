#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <unistd.h>

#ifdef MSODS
#define COPY(d, s, a) memmove(d, s, a)
#else
#define COPY(d, s, a) memcpy(d, s, a)
#endif

#define STDIN 0

#define MAXLOOK 16
#define MAXLEX 1024

#define BUFSIZE ((MAXLEX * 3) + (2 * MAXLOOK))
#define DANGER (End_buf - MAXLOOK)
#define END (&Start_buf[BUFSIZE])

#define NO_MORE_CHARS() (Eof_read && Next >= End_buf)

typedef unsigned char uchar;

static uchar Start_buf[BUFSIZE];
static uchar* End_buf = END;
static uchar* Next = END;
static uchar* sMark = END;
static uchar* eMark = END;
static uchar* pMark = END;

static int pLineno = 0;
static int pLength = 0;

static int Inp_file = STDIN;
static int Lineno = 1;
static int Mline = 1;
static int Termchar = 0;

static int Eof_read = 0;

/*Pointers to open,close,and read functions*/

static int (*Openp)(const char*, int, ...) = &open;
static int (*Closep)(int) = &close;
static ssize_t (*Readp)(int, void*, size_t) = &read;

void ii_io(int (*open_funct)(const char*, int...), int (*close_funct)(int),
    ssize_t (*read_funct)(int, void*, size_t))
{
    Openp = open_funct;
    Closep = close_funct;
    Readp = read_funct;
}
#define O_BINARY 0
int ii_newfile(char* name)
{
    int fd;
    if (strcmp(name, "/dev/tty") == 0)
        name = (char*)"CON";
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
uchar* ii_text() { return sMark; }
uchar ii_length() { return eMark - sMark; }
uchar ii_lineno() { return Lineno; } // return current line number
uchar* ii_ptext() { return pMark; }
uchar ii_lenghth() { return pLength; }
uchar ii_plineno() { return pLineno; }

uchar* ii_mark_start()
{
    Mline = Lineno;
    eMark = sMark = Next;
    return sMark;
}
uchar* ii_mark_end()
{
    Mline = Lineno;
    return eMark = Next;
}

uchar* ii_move_start()
{
    if (sMark >= eMark)
        return NULL;
    else
        return ++sMark;
}
uchar* ii_to_mark()
{
    Lineno = Mline;
    // restore the input pointer to the last end marker
    return Next = eMark;
}

uchar* ii_mark_prev()
{
    pMark = sMark;
    pLineno = Lineno;
    pLength = eMark - sMark;
    return pMark;
}
int ii_flush(int);
int ii_advance()
{
    static int been_called = 0;
    if (!been_called) {
        /*push a newline into the empty buffer so that the lex start-of-line
         * anchor will work on the first input line
         */
        Next = sMark = eMark = END - 1;
        ;
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
int ii_fillbuf(uchar*);
int ii_flush(int force)
{
    int copy_amt, shift_amt;
    uchar* left_edge;
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

int ii_fillbuf(uchar* starting_at)
{
    unsigned need, got;
    need = ((END - starting_at) / MAXLEX * MAXLEX); // float point number
    printf("Reading %d bytes\n", need);
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

int ii_look(int n)
{
    uchar* p;
    p = Next + (n - 1);
    if (Eof_read && p >= End_buf)
        return EOF;
    return p < Start_buf || p > End_buf ? 0 : *p;
}

int ii_pushback(int n)
{
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

void ii_term()
{
    Termchar = *Next;
    *Next = '\0';
}
void ii_unterm()
{
    if (Termchar) {
        *Next = Termchar;
        Termchar = 0;
    }
}

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

void ii_unput(int c)
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

int ii_lookahead(int n)
{
    return (n == 1 && Termchar) ? Termchar : ii_look(n);
}
int ii_flushbuf()
{
    if (Termchar)
        ii_unterm();
    return ii_flush(1);
}
