{
    int a;
    a = 1;
    
    float b = 1.0;
    
    if ( a > 0 ) {
        a = 0;
    }

    if ( a > 0 ) {
        a = 0;
    } else {
        a = 1;
    }

    {
        int a0 = -1;
        int a1 = !(a > 0);

        int a2 = 1 + 2;
        int a3 = 1 - 2;
        int a4 = 1 * 2;
        float a5 = 1 / 2;
        int a6 = 1 == 2;
        int a7 = 1 != 2;
        int a8 = 1 <= 2;
        int a9 = 1 >= 2;
        int a10 = 1 < 2;
        int a11 = 1 > 2;
    }
}