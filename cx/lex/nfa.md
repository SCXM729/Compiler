nfa.md
struct nfa:
    char *accept: the string itself is of variable length. The first int's
        worth of characters hold the line number and the remainder of the array 
        holds the string.

nfa.cpp
    ENTER() and LEAVE() expand to empty string whe DEBUG is not defined, When debugging
, they print the current subroutine name(which is passed in as an argument or "__function__")
,the current lexeme and what's left of the current input line. An ENTER invocation is placed
at the top of every subroutine of interest. and a LEAVE is put at the bottom. The text is indented
by an amount proportional to the subroutine-nesting level-Lev is incremented by every ENTER invocation
, and decrement by every LEAVE  


the stack memory-management strategy: It's convenient when constructing the NFA to create a physical 
machine with one node per state and actual pointers to the next state. Later on, it will be convenient
to have the NFA represented as an array because you can use the array index as the state number. The 
only disadvantage is that any nodes that still on the stack when NFA construction is complete will be
holes in the array

NFA*cnew():
    uses a node from the stack if one is available it gets a new node from the Nfa_states[] array.

void discard(NFA*):
    pushes a pointer to the discarded node from stack 

char*save(char*):
   the function is passed a pointer to a string. and returns a pointer to a copy-in static memory-
of that string.

void new_macro(char*def):
    passed a pointer to a line that contains a macro definition and it files the marco in a 
small table. 

char*expand_macro(char*namep):
    passed a pointer to a character pointer, which in turn points at a macro invocation. The
routine advances *namep past the invocation, and returns a string holding the macro's contents.
    


