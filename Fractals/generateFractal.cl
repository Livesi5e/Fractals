__kernel void generateFractal(__global const float* x, __global const float* y, __global const int* res, __global const float cx, __global const float cy){
    int ret = computeIterations(x, y, cx, cy, 255);
    res[i] = 0x00010000 * ret + 0x00000100 * ret;
}

int computeIterations(float x, float y, float cx, float cy, int d){
    float xn = x;
    float yn = y;
    int round = 0;
    while(mod2(xn, yn) < 4.0 && round < d){
        xn = computeNextX(x, y, cx);
        yn = computeNextY(x, y, cy);
        round = round + 1;
    }
    return round;
}

float mod2(float x, float y){
    return x * x + y * y;
}

float computeNextX(float x, float y, float cx){
    return (x * x - y * y) + cx;
}

float computeNextY(float x, float y, float cy){
    return 2.0 * x * y + cy;
}