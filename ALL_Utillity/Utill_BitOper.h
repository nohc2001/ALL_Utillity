//reference form the book "Haker's Delight."

#ifndef UTILL_BITOPER
#define UTILL_BITOPER

//###################################################################################
//###################################################################################
// 2. Basic Knowledge
//###################################################################################
//###################################################################################

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

//--------------------------------------------------------------
// 2-1 control last bit
//--------------------------------------------------------------

// l1to0(x) == 0 -> x == 2^(any_n)
#define l1to0(x) (x & (x-1))
#define l0to1(x) (x | (x+1))
// en1to0(x) == 0 -> x == 2^(any_n) - 1
#define en1to0(x) (x & (x+1))
#define en0to1(x) (x | (x-1))
#define l0to1_rest0(x) (~x & (x+1))
#define l1to0_rest1(x) (~x | (x-1))
#define en0to1_rest0(x) (~x & (x-1)) // or (~(x | -x)) or ((x & -x)-1)
#define en1to0_rest1(x) (~x | (x+1))
#define l1p_rest0(x) (x & (-x))
#define l1p_en0to1_rest0(x) (x ^ (x-1))
#define l0to1_en1p_rest0(x) (x ^ (x+1))
// ln1to0(x) == 0 -> x == 2^(any_n) - 2^(any_m) {n > m >= 0}
#define ln1to0(x) (((x | (x-1))+1) & x) // or (((x & -x)+x)&x)

//extend De Morgan's Law
/*
~(x & y) == ~x | ~y
~(x | y) == ~x & ~y
~(x + 1) == ~x - 1
~(x - 1) == ~x + 1
~(-x) == x - 1
~(x ^ y) == ~x ^ y == x≡y
~(x≡y) == ~x≡y == x^y
~(x+y) == ~x-y
~(x-y) == ~x+y
*/

//get minimum return that bigger than x and 1bit count same with x.
// form : ((x ^ (x + (x & -x)) >> 2) / (x & -x)) | (x + (x & -x))
unsigned int Gosper175(unsigned int x){
    unsigned int s, r, o;
    s = l1p_rest0(x);
    r = x + s;
    o = x ^ r;
    o = (o >> 2) / s;
    return r | o;
}

//--------------------------------------------------------------
// 2-2 addition combine with logical operation
//--------------------------------------------------------------

// add operation and logical operation identity equation
/*
-x == ~x + 1 == ~(x - 1)
~x == -x-1
-~x == x+1
~-x == x-1
x + y == x - ~y - 1 == (x ^ y) + 2 * (x & y) == (x | y) + (x & y) == 2 * (x | y) - (x ^ y)
x - y == x + ~y + 1 == (x ^ y) + 2 * (~x & y) == (x & ~y) - (~x & y) == 2 * (x & ~y) - (x ^ y)
x ^ y == (x | y) - (x & y)
x & ~y == (x | y) - y == x - (x & y)
~(x - y) == y - x - 1 == ~x + y
x≡y == (x & y) - (x | y) - 1 == (x & y) - ~(x | y)
x | y == (x & ~y) + y
x & y == (~x | y) - ~x
*/

//--------------------------------------------------------------
// 2-3 logical and arithmetic inequation
//--------------------------------------------------------------

//16 logical operation with every xy cases
//every bit location is seperate.
/*
        (0, 0)  (1, 0)  (0, 1)  (1, 1)
0       0       0       0       0
x & y   0       0       0       1
x & ~y  0       1       0       0
x       0       1       0       1
~x & y  0       0       1       0
y       0       0       1       1
x ^ y   0       1       1       0
x | y   0       1       1       1
~(x | y)1       0       0       0
x≡y    1       0       0       1
~y      1       1       0       0
x | ~y  1       1       0       1
~x      1       0       1       0
~x | y  1       0       1       1
~(x & y)1       1       1       0
1       1       1       1       1


sort by siz >> good to find inequation
        (0, 0)  (1, 0)  (0, 1)  (1, 1)
0       0       0       0       0

~(x | y)1       0       0       0
x & ~y  0       1       0       0
~x & y  0       0       1       0
x & y   0       0       0       1

~y      1       1       0       0
~x      1       0       1       0
x≡y    1       0       0       1
x ^ y   0       1       1       0
x       0       1       0       1
y       0       0       1       1

~(x & y)1       1       1       0
x | ~y  1       1       0       1
~x | y  1       0       1       1
x | y   0       1       1       1

1       1       1       1       1

<2 > 1> inequation
~y ≥ ~(x | y)
~y ≥ x & ~y
~x ≥ ~(x | y)
~x ≥ ~x & y
x≡y ≥ ~(x | y)
x≡y ≥ x & y
x ^ y ≥ x & ~y
x ^ y ≥ ~x & y
x ≥ x & ~y
x ≥ x & y
y ≥ ~x & y
y ≥ x & y

<3 > 2> inequation
~(x & y) ≥ ~y
~(x & y) ≥ ~x
~(x & y) ≥ x ^ y
x | ~y ≥ ~y
x | ~y ≥ x≡y
x | ~y ≥ x
~x | y ≥ ~x
~x | y ≥ x≡y
~x | y ≥ y
x | y ≥ x ^ y
x | y ≥ x
x | y ≥ y
*/

//mix logical and arithmetic expression inequation
/*
(x | y) ≥ max(x, y)
min(x, y) ≥ (x & y)
if(carry == 0) { x + y ≥ (x | y) }
if(carry == 1) { (x | y) > x + y}
x ^ y ≥ abs(x - y)
*/

//--------------------------------------------------------------
// 2-4 absolute function
//--------------------------------------------------------------
/*
y = x >> 31 
// 4byte = 32bit 32-1 = 31 (2^n-1)
abs(x) == (x ^ y) - y == (x + y) ^ y == x - (2*x & y)
if(cpu's multiply ±1 operation is faster than normal) abs(x) == ((x >> 30) | 1) * x 
nabs(x) == y - (x ^ y) == (y - x) ^ y == (2*x & y) - x
*/

#define abs_int(x) (x - ((x << 1) & (x >> 31)))
#define nabs_int(x) (((x << 1) & (x >> 31)) - x)

//--------------------------------------------------------------
// 2-5 average of two integer
//--------------------------------------------------------------
/*
unsigned integer floor average (never occur overflow)
(x & y) + ((x ^ y) >> 1)
unsigned integer ceiling average (never occur overflow)
(x | y) - ((x ^ y) >> 1)
signed integer average is same as unsigned stuff but shift is not unsigned shift. it's signed shift operation.
In C language, just change type "unsigned int" to "int". 
*/

#define floor_average(x, y) (x & y) + ((x ^ y) >> 1)
#define ceiling_average(x, y) (x | y) - ((x ^ y) >> 1)

//--------------------------------------------------------------
// 2-6 sign extension
//--------------------------------------------------------------

/*
sign extension : casting short -> int operation.
how to operate ?
        1. just casting
        ....
*/

#define sign_extension_int(x) ((x + 0x00000080) & 0x000000FF) - 0x00000080

//--------------------------------------------------------------
// 2-7 signed right shift
//--------------------------------------------------------------

/*
if x type is unsigned int
(int)x >> n == ((x+0x80000000) >> n) - (0x80000000 >> n)
->
t = 0x80000000 >> n; ((x >> n) ^ t) - t
*/

//--------------------------------------------------------------
// 2-8 sign function
//--------------------------------------------------------------

/*
sign(x) = if(x < 0) {-1} else if(x > 0) {1} else {0}
*/

#define sign_int(x) = (x >> 31) | ((unsigned int)(-x) >> 31)
//if signed right shift instruct is not exist, sign(x) = -((unsigned int)x >> 31) | ((unsigned int)-x >> 31)

//--------------------------------------------------------------
// 2-9 compare function
//--------------------------------------------------------------

/*
cmp(x, y) = if(x < y) {-1} else if(x > y) {1} else {0}
*/
#define cmp_int(x, y) ((x > y) - (x < y))

//--------------------------------------------------------------
// 2-10 sign sending function
//--------------------------------------------------------------

/*
ISIGN(x, y) == if(y >= 0) abs(x) else nabs(x)
*/

inline int isign_int(int x, int y){
        int t = y >> 31;
        return (abs_int(x) ^ t) - t;
}

//--------------------------------------------------------------
// 2-11 decoding 0 to max value + 1 (2^n) (in n bit)
//--------------------------------------------------------------

/*
ex> 4bit data decode to 5bit data
00000 -> 10000
00000 - 1 = 11111
11111 & 01111 = 01111
01111 + 1 = 10000
*/

#define decoding_0toMaxPlus1_uint_to_ulonglong(x) (((unsigned long long)x-1) & (unsigned long long)(unsigned int)(-1)) + 1
// just casting to uint to encode back.

//--------------------------------------------------------------
// 2-12 comparison predicate
//--------------------------------------------------------------
#endif