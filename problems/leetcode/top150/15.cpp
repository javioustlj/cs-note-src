#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<vector<int>> threeSum(vector<int>& nums)
{
    vector<vector<int>> res;
    vector<int> temp(3);
    int size = nums.size();
    for (int i = 0; i < size; ++i)
    {
        for (int j = i + 1; j < size; ++j)
        {
            for (int k = j + 1; k < size; ++k)
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
    // std::cout << "res size: " << res.size() << std::endl;
    sort(res.begin(), res.end());
    res.erase( unique(res.begin(), res.end()), res.end() );
    // std::cout << "res size: " << res.size() << std::endl;
    return res;
}

void func()
{
    vector<int> nums {-1, 0, 1, 2, -1, -4};
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
