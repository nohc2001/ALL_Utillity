#pragma once
#define Cor3(a, b, c) ((a||b)||c)
#define Cor4(a, b, c, d) ((a||b)||(c||d))

#define Cand3(a, b, c) ((a&&b)&&c)
#define Cand4(a, b, c, d) ((a&&b)&&(c&&d))

#define Cisor3(v, a, b, c) ((v==a||v==b)||v==c)
#define Cisor4(v, a, b, c, d) ((v==a||v==b)&&(v==c||c==d))

#define Cisin(a, b, c) ((a < b) && (b < c))
#define Cisin_same(a, b, c) ((a<=b)&&(b<=c))

#define Cisout(a, b, c) ((a > b) || (b > c))
#define Cisout_same(a, b, c) ((a >= b) || (b >= c))