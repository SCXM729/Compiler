#include <cstdarg>
#include <cstdio>
void yycoment(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

void yyerror(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "ERROR on line %d,near <%s>\n", yylineno, yytext);
    va_end(args);
}
