
BUCKET: next-prev-user area. the size of user area is passed as a parameter.
the BUCKET's structure and two structure interact as shown in ../images/

The obscure point is the two stars in the definition of the backwards pointer--the 
forward pointer(next) is a pointer to the BUCKET, but the backwards pointer(prev) 
is a pointer to a BUCKET pointer. Because the head of the chain is a simple pointer,
not an entire BUCKET structure. The backwards pointer for the leftmost node in the chain
points at the head-of-chain pointer. All other backwards pointers hold the address
of the next field from the previous node.

HASH_TAB*maketab(unsigned maxsym,unsigned(*hash)(void*),unsigned(*cmp)(void*,void*)):
    Make a hash table of the size specified in maxsym. The hash table is
a data structure that the manager uses to origanize the database entries.
It contains a "hash table" array along with various other other housekeeping
variables. The maxsym argument controls the size of the array component of 
the data structure, but any number of entries can be put into the table,
regardless of the table size.
    The hash function should return a pseudo-random number, the value of 
which is controlled by the key, the same key should always generate the 
same number, but different keys should generate different numbers. The 
simplest, but by no means the best, hash strategy just adds together the 
character in the name.
    comparison function is passed two pointers to databse records. It should
compare the key fields and return a value representing the ordering of the 
keys in a manner analogous to strcmp(). A call to (*cmp)(p1,p2) should return
a negative number if the key field in the structure pointed to by p1 is less
than the one in *p2. It should return 0 if the two keys are identical, and it
should return a positive number if the key in *p2 is greater.

void*newsym(int size):
    like malloc() to get space for a database record. The returned memory
is initialized to zeros. Typically, you use newsym() to allocate a structure,
one field of which is the key.

void freesym(void*sym):
    frees the memory for a symbol created by a previous newsym() call. You 
may not use free() for this purpose. Do not free symbols that are stil in
the table--remove them with a delsym() call first.

void* addsym(HASH_TAB*tabp,void*sym):
    Add a symbol to the hash table pointed to by tabp--a pointer form a previous
maketab() call. The sym argument points at a database record, a pointer to which
was returned from a previous newsym() call. You must initialize the key field of 
that record prior to the addsym() call.
    p(second rank pointer) first get the address of the computed array element. 
tmp pointer save the computed array element. Because sym is linked to the head
of chain found at the previously computed array element. There doesn't exist any
BUCKET* element in front of it. So sym->prev just point to itself. Then sym->next
point to the tmp. If tmp is not NULL, connect tmp and sym.

void*findsym(HASH_TAB*tabp,void*sym):
    Return either a pointer to a previously-inserted database record or NULL if the
record isn't in the database. If more than one entry for a given key is in the database,
the most recently added one is found. The sym argument is used to identify the record
for which you're searching. It is not used directly by findsym(), but is passed to the
hash and comparison functions.
    The comparison function is called as follows: (*cmp)(sym,item_in_table); 
Here, item_in_table is a pointer to an arbitrary database element, and sym is just the 
second argument to findsym(). Strictly speaking, sym should be a pointer to an initialized
database record returned from newsym(). It's inconvenient, though, to allocate and initialize
a structure just to pass it to findsym(). You can get around the problem in one common situation.
If the key field is a character array, and that array is the first field in the structure, you 
can pass a character-string name to findsym() as the key.
    It just hashes to the correct place in the table and then chases down the chain,
looking for the required node. It returns a pointer to the user area of the BUCKET
(thus the +1), or NULL if ti can't find the required node.

void delsym(HASH_TAB*tabp,void*p):
    Remove a symbol from the hash table, a pointer to which was returned by a previous findsym()
call. sym is the pointer returned from findsym() and tabp is a pointer returned from maketab().
The record is removed from the table, but the associated memory is not freed, so you can recycle it
--you can reinitialize the record and reinsert it into the table at a later time. Use freesym()
to free the memory used by sym--you may not use free() for this purpose. It's a serious error to
delete a symbol that isn't actually in the table--addsym() must have been called for a node before
delsym() can be called.
   
void*nextsym(HASH_TAB*tabp,void*last):
    This function finds all references to objects in a table that have the same name. The first such
object is found by findsym(). The second object is found by passing the pointer returned from find_sym()
to nextsym(), which returns either a pointer to the next pointer object or NULL if there are no such objects.

int ptab(HASH_TAB*tabp,void(*print)(),void*param,int sort):
    Print all records in the database represented by the hash table pointed to by tabp. The 
function pointed to by print is called for every element of the table as follows:
 (*print)(sym,param)
    If sort is false, the table elements are pointed in random order and 1 is always returned. 
If sort is true, the table is printed only if the routine can get memory to sort the table.
Zero is returned (and nothing is printed) if memory isn't available, otherwise 1 is returned
and the table is printed in order controlled by the comparsion function passed to maketab().
If this comparsion function works as described earlier, the table is printed in descending order.
In the current example, you can change the comparsion function as follows, reversing the arguments
to strcmp():
    cmp(ENTRY*1,ENTRY*b){return strcmp(b->key,a->key);}

unsigned hash_add(char*name):
unsigned hash_pjw(char*name):
    These two functions are hash functions that you pass to maketab(). They are passed character
strings and return a pseudo-random integer generated from that string. hash_add() just adds the 
character in the name--it's fast but doesn't work well if the table size is larger than 128 or
if keys are likely to be permutations of each other. The hash_pjw function uses a shift-and-exclusive
-OR algorithm that yields better results at the cost of of execution speed.
