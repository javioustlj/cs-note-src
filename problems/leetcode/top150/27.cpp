#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
int removeElement(vector<int>& nums, int val)
{
    int i = 0, j = nums.size() - 1, cnt = 0;
    for (int i = 0; i <= j; ++i)
    {
        if ( nums[i] == val)
        {
            ++cnt;
            for (;j > i; --j)
            {
                if (nums[j] == val)
                {
                    ++cnt;
                }
                else
                {
                    nums[i] = nums[j];
                    --j;
                    break;
                }
            }
        }
    }
    std::cout << cnt <<std::endl;
    return nums.size() - cnt;
}

void test()
{
    vector<int> nums1 {0, 1, 2, 2, 3, 0, 4, 2};
    removeElement(nums1, 2);
    for_each(nums1.begin(), nums1.end(), [](const int n) { std::cout << n << ' '; });
    std::cout << std::endl;
}

int main()
{
    test();

    return 0;
}
