
/**
 * @file main.cpp
 * @author 
 * @brief "1D测量类"的测试
 * @version 0.1
 * @date 2021-06-07
 * 
 */

#include "measurement.h"

int main()
{
    std::clock_t c_start = std::clock();

    Measurement test;
    test.Semicircle(520, 980, 220, 400, "left", 100, 0.5);
    test.Semicircle(1520, 980, 220, 400, "right", 100, 0.5);
    test.Save();

    std::clock_t c_end = std::clock();
    std::cout << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms" << std::endl;
    return 0;
}
