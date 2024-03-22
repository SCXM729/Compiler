#function
SET*newset(): 
    Create a new set and return a pointer to it. Print an error message
and raise SIGABRT if there's insufficient menory. Normally this signal terminates the 
program.(equal to exit(0))
    
void delset(SET*set):
    Delete a set created with previous newset call and free the associated memory.
The argument must have been returned from a previous newset() call.

SET*dupset(SET*set):
    Create a new set that has the same members as the input set. This routine is
more efficient than using newset() to create the set and then copying the members
one at a time, but otherwise has the same effect.

int _addset(SET*set,int bit):
    This is an internal function used by the ADD macro, and shouldn't be called by you

int num_ele(SET*set):
    Return the number of elements in the input set. NULL sets(describle below) are 
considered to be empty.

int _set_test(SET*set1,SET*set2):
    This an another workhouse function used internally by the macros. Don't call it directly.

int setcmp(SET*set1,SET*set2):
    Compare two sets in a manner similar to strcmp() returning 0 if the sets are equivalent,
<0 if set1<set2 and >0 if set1>set2. This routine lets you sort an array of SET's so that 
equivalent ones are adjacent. The determination of less than and greater than is pretty much
arbitrary. (This routine just compares the bit map as if you were doing a lexicographic ordering
of an array of ints). It first compares the map elements that exist in both of the sets. If there's
a mismatch, the two bytes are subtracted to get a relative ordering. Then, and then the tail end of 
the larger set is scanned to make sure that it's all zeros. if so, the two sets are equivalent.
If not, the larger set is the greater.

unsigned sethash(SET*set1):
    This function is even more obscure than setcmp(). It is provided for those situations where
a SET is used as the key in a hash table. It returns the sum of the individual words in the bit map.

int subset(SET*set,SET*sub):
    Return 1 if sub is a sbuset of set, 0 otherwise. Empty and null sets are subsets of everything,
and 1 is returning if both sets are empty or null.

void _set_op(int op,SET*dest,SET*src):
    Another workhouse function used internally by the macros.
the first while in every case takes care of all destination elements that correspond to source elements.
The second while loop in each case won't execute if the two sets are the same size, because the tail size
will be zero.Otherwise, the destination set is larger and you must do different things to the tail, 
depending on the operation.

void invert(SET*set):
    Physically invert the bits in the set, setting 1's to 0 and vice versa. In effect, this operation
removes all existing members from a set and adds all possible members that weren't there before. 
Note that the set must be expanded to the maximum possible size before calling invert()---ADD the largest
element and then delete it. See also, COMPLEMEMT().

void truncate(SET*set):
    Clears the set and shrinks it back to the original, default size. Compare this routine to the CLEAR()
macro, describled below, which clears all the bits in the map but doesn't modify the size. This routine
is really a more efficient replacement for delset(s); s=newset(); If the original set isn't very big, 
you're better off using CLEAR().

int next_member(SET*set):
    When called several successive times with the same argument, returns the next element of the set each
time it's called or -1 if there are no more elements. Every time the set argument changes, the search for 
elements starts back at the beginning of the set. A NULL argument also resets the search to the beginning
(and does nothing else). Strange things happen if you add members to the set between successive calls.
You should not put any new elemets in the set between a next_member(NULL) call and a subsequent next_member(set) call.
This problem could be solved by putting the current_member counter into the SET structure rather than keeping 
it as a local static variable.If calls to next_menber are interspersed(散布,点缀) with calls to pset(discussed below), 
next_member won't work properly. Calls to next_member() on different sets cannot be interspersed.

void pset(SET*set,int(*out)(),void*param):
    Print the set. The output routine pointed to by out is called for each element of the set with the following
argument:
(*out)(param,"null",-1); Null set
(*out)(param,"empty",-2); Empty set
(*out)(param,"%d ",N); Set element N
This way you can use fprintf() as a default output routine.

UNION(SET*dest,SET*src):
    Modify the dest set to hold the union of the src and dest sets.

INTERSECT(SET*dest,SET*src):
    Modify the dest set to hold the intersection(交叉,相交) of the src and dest sets.

DIFFERENCE(SET*dest,SET*src):
    Modify the dest set to hold the symmetric difference(差) of the src and dest sets.
(An element is put into the target set if it is a member of dest but not of src, or vice versa)

ASSIGN(SET*dest,SET*src):
    Overwrite the dest with src.

CLEAR(SET*s):
    Clear all bits in s, creating an empty set.

FILL(SETs*s):
    Set all bits in s to 1, creating a set that holds every element in the input aplhabet.

COMPIEMENT(SET*s):
    Complement(补充，补集) a set efficently by modifying the set's complement bit. Sets complemented in 
this way can't be manipulated by UNION(), etc. See also, invert() and INVERT().

INVERT(SET*s):
    Complement a set by physically changing the bit map(see text)

IS_DISJOINT(SET*s1,SET*s2):
    Evaluate to true only it the two sets are disjoint(have no elements in common).

IS_INTERSECTING(SET*s1,SET*s2):
    Evaluate to true only if the two sets are intersect(have at least one element in common)

IS_EMPTY(SET*s):
    Evaluate to true only if the set is empty(having no elements) or null(is NULL)

IS_EQUIVALENT(SET*s1,SET*s2):
    Evaluate to true only if the two sets are equivalent(have the same elements).

ADD(SET*s,int x):
    Add the element c to set s, It is not an error to add an element to s set more than once

REMOVE(SET*s,int x):
    Remove the element c from set s, It is not an error to remove an element that is not in the set.

TEST(SET*s,int x):
    Evaluate to true if x is an element of set s.

MEMBER(SET*s,int x):
    Evaluate to true if x is an element of set s. This macro doesn't work on COMPLEMENTed set, but
it's both faster and smaller then TEST, which does. The distinction is describled below.




