//reference form the book "Haker's Delight."

#ifndef UTILL_BITOPER
#define UTILL_BITOPER

/*
symbol guide
R : right
L : left
s : start
f : first
e : end
l : last
nton : transform
rest : rest bits
p : pass
*/

// basic parameter type is unsigned integer types.
// control last bit
#define l1to0(x) (x & (x-1))
#define l0to1(x) (x | (x+1))
#define en1to0(x) (x & (x+1))
#define en0to1(x) (x | (x-1))
#define l0to1_rest0(x) (~x & (x+1))
#define l1to0_rest1(x) (~x | (x-1))
#define en0to1_rest0(x) (~x & (x-1)) // or (~(x | -x)) or ((x & -x)-1)
#define en1to0_rest1(x) (~x | (x+1))
#define l1p_rest0(x) (x & (-x))
#define l1p_en0to1_rest0(x) (x ^ (x-1))
#define l0to1_en1p_rest0(x) (x ^ (x+1))
#define ln1to0(x) (((x | (x-1))+1) & x) // or (((x & -x)+x)&x)

#endif