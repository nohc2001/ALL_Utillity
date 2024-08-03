//reference form the book "Haker's Delight."

#ifndef UTILL_BITOPER
#define UTILL_BITOPER

typedef unsigned char ui8;
typedef unsigned short ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;
typedef unsigned int vui128 __attribute__((vector_size(16)));
typedef unsigned int vui256 __attribute__((vector_size(32)));
typedef unsigned int vui512 __attribute__((vector_size(64)));
typedef unsigned long long vul512 __attribute__((vector_size(64)));
struct page4096{
        unsigned char data[4096] = {};
};

//todo : 
/*
1. function is all inline function or macro.
2. all function support to all siz type. (ui8, ui16, ui32, ui64)
3. all function support to same work to multiple data (with SIMD(vui128, vui256, vui512) and ILP and Asynchrony Reapeat) with 1 page(4096)
4. all function comment describe predict cost.
*/

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
// 0-1 if not zero, return 1;
//--------------------------------------------------------------
#define pass0or1(x) !!x


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

//...

//--------------------------------------------------------------
// 5-1 counting 1bit in data
//--------------------------------------------------------------
inline ui8 count1bit(ui8 x){
        x = x - ((x >> 1) & 0x55);
        x = (x & 0x33) + ((x>>2) & 0x33);
        x = (x+(x>>4)) & 0x0F;
        return x & 0x0F;
}

inline ui16 count1bit(ui16 x){
        x = (x & 0x5555) + ((x >> 1) & 0x5555);
        x = (x & 0x3333) + ((x >> 2) & 0x3333);
        x = (x & 0x0F0F) + ((x >> 4) & 0x0F0F);
        x = (x & 0x00FF) + ((x >> 8) & 0x00FF);
        return x;
}

inline ui32 count1bit(ui32 x){
        x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
        x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
        x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
        x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
        x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
        return x;
}

inline ui64 count1bit(ui64 x){
        x = (x & 0x5555555555555555) + ((x >> 1) & 0x5555555555555555);
        x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
        x = (x & 0x0F0F0F0F0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F0F0F0F0F);
        x = (x & 0x00FF00FF00FF00FF) + ((x >> 8) & 0x00FF00FF00FF00FF);
        x = (x & 0x0000FFFF0000FFFF) + ((x >> 16) & 0x0000FFFF0000FFFF);
        x = (x & 0x00000000FFFFFFFF) + ((x >> 32) & 0x00000000FFFFFFFF);
        return x;
}

struct count1bit_context4{
        vui256 x1; // x << 1
        vui256 x2; // x & const
        vui256 x3; // x1 & const
        // x <- x2 + x3
};
ui32 count1bit_page(page4096* source, page4096* dest){
        constexpr unsigned int constV[5] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};
        count1bit_context4 cxt[64];
        vui256* vd = (vui256*)dest;
        vui256* vs = (vui256*)source;
        for(int i=0;i<14;i+=1){
                cxt[i+14].x1 = *vs << 1;
                cxt[i+14].x2 = *vs & constV[0];
                cxt[i+13].x3 = cxt[i+13].x1 & constV[0];
                cxt[i+12].x3 = cxt[i+12].x2 + cxt[i+12].x3;
                cxt[i+11].x1 = cxt[i+11].x3 << 2;
                cxt[i+11].x2 = cxt[i+11].x3 & constV[1];
                cxt[i+10].x3 = cxt[i+10].x1 & constV[1];
                cxt[i+9].x3 = cxt[i+9].x2 + cxt[i+9].x3;
                cxt[i+8].x1 = cxt[i+8].x3 << 4;
                cxt[i+8].x2 = cxt[i+8].x3 & constV[2];
                cxt[i+7].x3 = cxt[i+7].x1 & constV[2];
                cxt[i+6].x3 = cxt[i+6].x2 + cxt[i+6].x3;
                cxt[i+5].x1 = cxt[i+5].x3 << 8;
                cxt[i+5].x2 = cxt[i+5].x3 & constV[3];
                cxt[i+4].x3 = cxt[i+4].x1 & constV[3];
                cxt[i+3].x3 = cxt[i+3].x2 + cxt[i+3].x3;
                cxt[i+2].x1 = cxt[i+3].x1 << 16;
                cxt[i+2].x2 = cxt[i+3].x1 & constV[4];
                cxt[i+1].x3 = cxt[i+1].x1 & constV[4];
                //*v = cxt[i].x2 + cxt[i].x3;
                vs += 1;
        }

        for(int i=14;i<(14+32);i+=1){
                vd += 1;
                cxt[i+14].x1 = *vs << 1;
                cxt[i+14].x2 = *vs & constV[0];
                cxt[i+13].x3 = cxt[i+13].x1 & constV[0];
                cxt[i+12].x3 = cxt[i+12].x2 + cxt[i+12].x3;
                cxt[i+11].x1 = cxt[i+11].x3 << 2;
                cxt[i+11].x2 = cxt[i+11].x3 & constV[1];
                cxt[i+10].x3 = cxt[i+10].x1 & constV[1];
                cxt[i+9].x3 = cxt[i+9].x2 + cxt[i+9].x3;
                cxt[i+8].x1 = cxt[i+8].x3 << 4;
                cxt[i+8].x2 = cxt[i+8].x3 & constV[2];
                cxt[i+7].x3 = cxt[i+7].x1 & constV[2];
                cxt[i+6].x3 = cxt[i+6].x2 + cxt[i+6].x3;
                cxt[i+5].x1 = cxt[i+5].x3 << 8;
                cxt[i+5].x2 = cxt[i+5].x3 & constV[3];
                cxt[i+4].x3 = cxt[i+4].x1 & constV[3];
                cxt[i+3].x3 = cxt[i+3].x2 + cxt[i+3].x3;
                cxt[i+2].x1 = cxt[i+3].x1 << 16;
                cxt[i+2].x2 = cxt[i+3].x1 & constV[4];
                cxt[i+1].x3 = cxt[i+1].x1 & constV[4];
                *vd = cxt[i].x2 + cxt[i].x3;
                vs += 1;
        }
}

//--------------------------------------------------------------
// 5-3 counting front0bit
//--------------------------------------------------------------

constexpr ui8 count_front0_4bit[16] = {4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};

inline ui32 count_front0bit(ui8 x){
        ui32 n = count_front0_4bit[x & 15];
        n += !(n-4) * count_front0_4bit[x >> 4];
        return n;
}

inline ui32 count_front0bit(ui16 x){
        ui32 n = count_front0_4bit[x & 15];
        x >>=4;
        ui32 n0 = count_front0_4bit[x & 15];
        n += !(n-4) * n0;
        x >>=4;
        ui32 n1 = count_front0_4bit[x & 15];
        n += !(n0-4) * n1;
        x >>=4;
        n0 = count_front0_4bit[x];
        n += !(n1-4) * n0;
        return n;
}

//opnum graph [2 3 4 5 6 7 7 6 4 3 2 1]
inline ui32 count_front0bit(ui32 x){
        //step 1 (2)
        ui32 x1 = x & 15;
        x >>= 4;
        //step 2 (3)
        ui32 n = count_front0_4bit[x1];
        ui32 x2 = x & 15;
        x >>= 4;
        //step3 (4)
        ui32 n1 = n-4;
        ui32 a = count_front0_4bit[x2];
        x1 = x & 15;
        x >>= 4;
        //step4 (5)
        n1 = !n1;
        ui32 a1 = a - 4;
        ui32 b = count_front0_4bit[x1];
        x2 = x & 15;
        x >>= 4;
        //step5 (6)
        n1 *= a;
        a1 = !a1;
        ui32 b1 = b - 4;
        a = count_front0_4bit[x2];
        x1 = x & 15;
        x >>= 4;
        //step6 (7)
        n1 += n;
        a1 *= b;
        b1 = !b1;
        ui32 a2 = a-4;
        b = count_front0_4bit[x1];
        x2 = x & 15;
        x >>= 4;
        //step7 (7)
        n1 += a1;
        b1 *= a;
        a2 = !a2;
        ui32 b2 = b - 4;
        a1 = count_front0_4bit[x2];
        x1 = x & 15;
        x >>= 4;
        //step8 (6)
        n1 += b1;
        a2 *= b;
        b2 = !b2;
        a = a1 - 4;
        ui32 b3 = count_front0_4bit[x1];
        ui32 a3 = count_front0_4bit[x];
        //step9 (4)
        n1 += a2;
        b2 *= a1;
        a = !a;
        b1 = b3 - 4;
        //step10 (3)
        n1 += b2;
        a *= b3;
        b1 = !b1;
        //step11 (2)
        n1 += a;
        b1 *= a3;
        //step12 (1)
        n1 += b1;
        return n1;
}

// hmm i dont know
/*

struct count_front0bit_context4{
        vui256 x;
        vui256 x1;
        vui256 x2;
        vui256 a;
        vui256 a1;
        vui256 a2;
        vui256 a3;
        vui256 b;
        vui256 b1;
        vui256 b2;
        vui256 b3;
        vui256 n;
        vui256 n1;
};
ui32 count_front0bit_page(page4096* source, page4096* dest){
        vui256* vs = (vui256*)source;
        vui256* vd = (vui256*)dest;
        count_front0bit_context4 cxt[34];
        for (int i = 0; i < 32; ++i)
        {
                // step 1 (2)
                cxt[i].x1 = *vs & 15;
                cxt[i].x = *vs >> 4;
                // step 2 (3)
                cxt[i].n = *(count_front0_4bit + cxt[i].x1);
                cxt[i].x2 = cxt[i].x & 15;
                cxt[i].x >>= 4;
                // step3 (4)
                cxt[i].n1 = cxt[i].n - 4;
                cxt[i].a = count_front0_4bit[cxt[i].x2];
                cxt[i].x1 = cxt[i].x & 15;
                cxt[i].x >>= 4;
                // step4 (5)
                cxt[i].n1 = !cxt[i].n1;
                cxt[i].a1 = cxt[i].a - 4;
                cxt[i].b = count_front0_4bit[cxt[i].x1];
                cxt[i].x2 = cxt[i].x & 15;
                cxt[i].x >>= 4;
                // step5 (6)
                cxt[i].n1 *= cxt[i].a;
                cxt[i].a1 = !cxt[i].a1;
                cxt[i].b1 = cxt[i].b - 4;
                cxt[i].a = count_front0_4bit[cxt[i].x2];
                cxt[i].x1 = cxt[i].x & 15;
                cxt[i].x >>= 4;
                // step6 (7)
                cxt[i].n1 += cxt[i].n;
                cxt[i].a1 *= cxt[i].b;
                cxt[i].b1 = !cxt[i].b1;
                cxt[i].a2 = cxt[i].a - 4;
                cxt[i].b = count_front0_4bit[cxt[i].x1];
                cxt[i].x2 = cxt[i].x & 15;
                cxt[i].x >>= 4;
                // step7 (7)
                cxt[i].n1 += cxt[i].a1;
                cxt[i].b1 *= cxt[i].a;
                cxt[i].a2 = !cxt[i].a2;
                cxt[i].b2 = cxt[i].b - 4;
                cxt[i].a1 = count_front0_4bit[cxt[i].x2];
                cxt[i].x1 = cxt[i].x & 15;
                cxt[i].x >>= 4;
                // step8 (6)
                cxt[i].n1 += cxt[i].b1;
                cxt[i].a2 *= cxt[i].b;
                cxt[i].b2 = !cxt[i].b2;
                cxt[i].a = cxt[i].a1 - 4;
                cxt[i].b3 = count_front0_4bit[cxt[i].x1];
                cxt[i].a3 = count_front0_4bit[cxt[i].x];
                // step9 (4)
                cxt[i].n1 += cxt[i].a2;
                cxt[i].b2 *= cxt[i].a1;
                cxt[i].a = !cxt[i].a;
                cxt[i].b1 = cxt[i].b3 - 4;
                // step10 (3)
                cxt[i].n1 += cxt[i].b2;
                cxt[i].a *= cxt[i].b3;
                cxt[i].b1 = !cxt[i].b1;
                // step11 (2)
                cxt[i].n1 += cxt[i].a;
                cxt[i].b1 *= cxt[i].a3;
                // step12 (1)
                cxt[i].n1 += cxt[i].b1;
        }
}
*/

//--------------------------------------------------------------
// 6-3 seek the specific 1 bit string
//--------------------------------------------------------------

inline ui32 find_specific_str1(ui32 x, int n){
        ui32 k = 1;
        ui32 np = n+1;
        k += k << np;
        np <<= 1;
        k += k << np;
        np <<= 1;
        k += k << np;
        np <<= 1;
        k += k << np;

        for(int i=0;i<n;++i){
                x = x & k;
                k <<= 1;
        }

        if(x == ~0){
                
        }
        else{
        }
}
#endif