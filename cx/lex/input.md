get_expr():
    actual input function. It gets an entire rule--both the regular expression
and any following code--from the input file(pointed to by Ifile) and puts it 
into Input_buf[]. Multiple-line rules are handled here in that lines that start
with white space are concatenated to the previous line. Two line number variables
are modifed in this routine. They are Lineno, which holds the input line number
of the first line of the rule, and Actual_lineno, which holds the current input
line number.
    
get_expr():
    normally returns a pointer to the input string (in Input_buf[]). It returns
NULL either at end of file or when a line starting with a %% is encountered. 
Since %% is treated as an end of file, the third part of the input file, which
contains C source code that is passed directly to the output, is ignored by the parser.

