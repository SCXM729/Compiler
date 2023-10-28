/* stack.h Stack-maintenance marcos.Create downward-growing
 * stacks(which should work in all six memory models)
 */

#define stack_cls /*empty*/

#define stack_dcl(stack, type, size)                                           \
  typedef type t_stack;                                                        \
  stack_cls t_stack stack[size];                                               \
  stack_cls t_stack(*p_stack) = stack + (size)

#define stack_clear(stack)                                                     \
  ((p_stack) = (stack + sizeof(stack) / sizeof(*stack)))

#define stack_full(stack) ((p_stack <= stack))
#define stack_empty(stack) ((p_stack)>=(stack+sizeof(stack)/sizof(*stack))

#define stack_ele(stack) ((sizeof(stack) / sizeof(*stack)) - (p_stack - stack))
/*evaluate to the number of elements currently on the indicated stack*/

#define stack_item(stack, offset) (*(p_stack + offset))
#define stack_p p_stack

#define push_(stack, x) (*--p_stack = (x))
#define pop_(stack) (*p_stack++)

#define push(stack, x)                                                         \
  (stack_full(stack) ? ((t_stack)(long)(stack_err(1))) : push_(stack, x))
#define pop(stack)                                                             \
  (stack_empty(stack) ? ((t_stack)(long)(stack_err(0))) : pop_(stack))

#define popn_(stack, amt) ((p_stack += amt)[-amt])
/*the index can be negative*/
#define popn(stack, amt)                                                       \
  ((stack_ele(stack) < amt) ? ((t_stack)(long)(stack_err(0)))                  \
                            : popn_(stack, amt))
#define ferr(str)                                                              \
  printf(str);                                                                 \
  exit(1);
#define stack_err(o)                                                           \
  ((o) ? ferr("stack overflow\n") : ferr("stack underflow\n"))
// ((o) ? ferr("Stack overflow\n") : ferr("Stack underflow\n"))
