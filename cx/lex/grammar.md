Grammer  

machine  ->  rule machine              # a list of rules  
          |  rule* __END_OF_INPUT__

rule     ->  expr EOS action           # a single regular expression followed by an accepting action  
          |  ^expr __EOS__ action      # expression anchored to start of l
          |  expr$ __EOS__ action      # expression anchored to end of line


action   ->  white_space string        # an optional accepting action  
          |  white_space           
          |  epsilon

expr     ->  expr|cat_expr             # a list of expressions delimited by vertical bars  
          |  cat_expr              

cat_expr ->  cat_expr factor           # a list of concatenated expression  
          |  factor

factor   ->  term*                     # a subexpression followed by a *  
          |  term+                     # a subexpression followed by a +  
          |  term?                     # a subexpression followed by a ?  
          |  term

term     ->  [string]                  # a character class  
          |  [^string]                 # a negative character class  
          |  []                        # (nonstandard) matches white space  
          |  [^]                       # (nonstandard) everything but a white space  
          |  .                         # matches any character except newline  
          |  character                 # a single character  
          |  (expr)                    # a parenthesized subexpression  

white_space  ->  one or more tabs or spaces  
character    ->  any single ASCII character except white_space  
string       ->  one or more ASCII characters   



# Grammer
> *machine  ->  rule machine*     &emsp; # a list of rules  
*&emsp; &emsp; &emsp; &emsp; |  rule* __END_OF_INPUT__

> rule &emsp; &emsp; ->  expr EOS action       &emsp; # a single regular expression followed by an accepting action  
&emsp; &emsp; &emsp; &emsp; | ^expr __EOS__ action      &emsp; # expression anchored to start of line    
&emsp; &emsp; &emsp; &emsp; | expr$ __EOS__ action    &emsp;  # expression anchored to end of line

> action   ->  white_space string    # an optional accepting action  
          |  white_space           
          |  epsilon

> expr     ->  expr|cat_expr         # a list of expressions delimited by vertical bars  
          |  cat_expr              

> cat_expr ->  cat_expr factor       # a list of concatenated expression  
          |  factor

> factor   ->  term*                 # a subexpression followed by a *  
          |  term+                 # a subexpression followed by a +  
          |  term?                 # a subexpression followed by a ?  
          |  term

> term     ->  [ string ]            # a character class  
          |  [ ^string ]           # a negative character class  
          |  []                    # (nonstandard) matches white space  
          |  [^]                   # (nonstandard) everything but a white space  
          |  .                     # matches any character except newline  
          |  character             # a single character  
          |  (expr)                # a parenthesized subexpression  

>white_space  ->  one or more tabs or spaces  
character    ->  any single ASCII character except white_space  
string       ->  one or more ASCII characters   

