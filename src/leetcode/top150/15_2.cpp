#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<vector<int>> threeSum(vector<int>& nums)
{
    sort(nums.begin(), nums.end());
    vector<vector<int>> res;
    vector<int> temp(3);
    int size = nums.size();
    int a = -1, b = -1, c = -1;


    for (int i = 0; i < size; ++i)
    {
        if (nums[i] < 0)
            a = i;
        else if (nums[i] > 0)
        {
            b = i;
            break;
        }
        else
        {
            c = i;
        }
    }

    for (int i = 0; i <= a; ++i)
    {
        for (int j = size - 1;  b <= j; --j)
        {
            for (int k = i + 1; k < j; ++k)
            {
                if (nums[i] + nums[j] + nums[k] == 0)
                {
                    temp[0] = nums[i];
                    temp[1] = nums[j];
                    temp[2] = nums[k];
                    sort(temp.begin(), temp.end());
                    res.push_back(temp);
                }
            }
        }
    }

    // std:: cout << a << b << c << endl;

    if (c - a >= 3)
        res.push_back({0, 0, 0});


    // std::cout << "res size: " << res.size() << std::endl;
    sort(res.begin(), res.end());
    res.erase( unique(res.begin(), res.end()), res.end() );
    // std::cout << "res size: " << res.size() << std::endl;
    return res;
}

void func()
{
    // vector<int> nums {-1, 0, 1, 2, -1, -4};
    // vector<int> nums {0, 0, 0};
    vector<int> nums {1, 2, -2, -1};
    auto res = threeSum(nums);

    for (const auto &vec : res)
        for (const auto &i : vec)
            cout << i << " ";

    std::cout << std::endl;
}

int main()
{
    func();
    return 0;
}
