#include "Utill_Math_Interpolation.h"

init_mathexpr

int main()
{
    //f(1) = 0
    //f(40) = 1
    //f(256) = 2
    //f(65535) = 3
    math_interp::LagrangeInterpolation li;
    li.axy(1, 0);
    li.axy(40, 1);
    li.axy(100, 1);
    li.axy(256, 2);
    cout << li.GetExpressionStr() << endl;
    li.Bake();
    cout << li.e.GetExpressionString() << endl;
    for (int i = 0; i < 256; ++i) {
        cout << i << " : " << li.GetReturn(i) << endl;
    }

    return 0;
}