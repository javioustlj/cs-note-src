/******************************************************************************************
 * Computer Science Note Source code
 * Javious Tian, javioustlj@outlook.com
 * 夏田墨  javious_tian@zuaa.zju.edu.cn
 * Copyright (c) 2024 Javioustlj. All rights reserved.
 ******************************************************************************************/

/******************************************************************************************
 * 通用技术集团机床工程研究院
 * 对该函数进行性能优化
 ******************************************************************************************/

#include <string>

std::string remove_char_from_string(std::string s, char ch)
{
    std::string result;
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] != ch)
        {
            result += s[i];
        }
    }
    return result
}

std::string remove_char_from_string_optimization(const std::string& s, char ch)
{
    std::string result;
    result.reserve(s.size()); // 预先分配足够的空间

    for (const char& c : s) {
        if (c != ch) {
            result.push_back(c); // 使用 push_back 将字符添加到 result 中
        }
    }

    return result;
}
