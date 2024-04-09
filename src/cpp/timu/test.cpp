#include <string>
#include <iostream>
#include <algorithm>

std::string func(const std::string &a, const std::string &b)
{
    std::string ans;
    int c;
    int flag = 0;
    auto ia = a.crbegin(), ib = b.crbegin();
    for ( ;  ia != a.crend() &&  ib != b.crend(); ++ia, ++ib)
    {
        c = *ia - flag  - *ib;
        if (c < 0)
        {
            c += 10;
            flag = 1;
            ans += std::to_string(c);
        }
        else
        {
            flag = 0;
            ans += std::to_string(c);
        }
    }
    for (; ia != a.crend() && flag; ++ia)
    {
        c = *ia - flag;
        if (c < 0)
        {
            c += 10;
            flag = 1;
            ans += std::to_string(c);
        }
    }
    for (; ia != a.crend(); ++ia)
    {
        ans.push_back(*ia);
    }
    std::reverse(ans.begin(), ans.end());
    return ans;
}

int main()
{
    std::string a = "9999999999999999999";
    std::string b = "8888888888888888888";
    std::string ans = func(a, b);
    std::cout << ans << std::endl;
}
