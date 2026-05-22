/******************************************************************************************
 * Computer Science Note Source code
 * Javious Tian, javioustlj@outlook.com
 * 夏田墨  javious_tian@zuaa.zju.edu.cn
 * Copyright (c) 2024 Javioustlj. All rights reserved.
 ******************************************************************************************/

/******************************************************************************************
 *  沐曦二面题目 
 *  一个数组 2, 4, 7, 8, 3, 给定一个数14，找到最小的子数组，相加大于14，要求时间复杂度为O(n)
 ******************************************************************************************/


#include <iostream>
#include <utility>

std::pair<int, int> func(int v[], int n, int SUM)
{

    int sum = 0;
    int min_start = 0, min_end = 0;
    int min_len = n;
    for (int i = 0, k = 0;i < n && k < n;)
    {
        if ( sum < SUM )
        {
            sum += v[k];
            ++k;
        }
        else
        {
            min_end = k - 1;
            if (min_end - min_start + 1 < min_len)
            {
                min_len = min_end - min_start + 1;
            }
            sum -= v[i];
            ++i;
            min_start = i;
        }
    }
    return std::make_pair(min_start, min_len);
}

int main()
{
    int a[6] = {2, 4, 7, 8, 3};
    auto p = func(a, 6, 14);
    std::cout << p.second << ' ' << p.second << std::endl;

    return 0;
}
