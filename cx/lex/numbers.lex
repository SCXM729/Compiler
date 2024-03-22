%{
  #define FCON 1;
  #define ICON 2;
%}

D  {0-9}  /* a single decimal digit */
%%
{D}+    return ICON;
({D}+|{D}*\.{D}+|{D}+\.{D}*)(e{D}+)?    return FCON;
%%
