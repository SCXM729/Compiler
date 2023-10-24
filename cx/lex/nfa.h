typedef struct NFA {
    int edge; /*Label for edge:character,CCL,EMPTY,or EPSILON*/
    SET* bitset /*Set to stroe character classes*/
        struct NFA* next; /*Next state (or NULL if none)*/
    struct NFA* next2; /*Another next state if edge==EPSILON
      NULL of this state isn't used*/
    char* accept; /*NULL if not an accapting state,else a
     pointer to action string*/
    int anchor; /*Says whether pattern is anchored and,if
     so,where (uses #define above)*/
} NFA;

#define EPSILON -1 /*Non-character values of NFA.edge*/
#define CCL -2 /*if a transition is made on a character class  \
,all character in the class are elements of the SET pointed to \
by bitset,and edge holds the value CCL*/
#define EMPTY -3

#define NONE 0 /*Values of the anchor field:Not anchored*/
#define START 1 /*Anchored at start of line*/
#define END 2 /*Anchored at end of liine*/
#define BOTH (START | END) /*Anchored in both places*/

#define NFA_MAX 768 /*Maxium number of NFA states in a single \
machine.NFA_MAX*sizeof(NFA) can't exceed 64K*/
#define STR_MAX (10*1024) /*Total space that can be used by 
the accept string*/

void new_marco(char*definition);  /*these three are in nfa.c*/
void printmacs(void);           
NFA*thompson(char*(*input_funct)(),int*max_state,NFA**start_state);
void print_nfa(NFA*nfa,int len,NFA*start);   /*int printnfa.c*/
