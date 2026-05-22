#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n)
{
    vector<int> res(m + n, 0);
    int i = 0, j = 0, cnt = 0;

    for (; i != m && j != n; )
        if (nums1[i] < nums2[j])
            res[cnt++] = nums1[i++];
        else
            res[cnt++] = nums2[j++];

    for (int k = i; k < m; ++k)
        res[cnt++] = nums1[i++];

    for (int k = j; k < n; ++k)
        res[cnt++] = nums2[j++];

    nums1 = res;
}

void test()
{
    vector<int> nums1 {1,2,3,0,0,0};
    vector<int> nums2 {2,5,6};
    merge(nums1, 3, nums2, 3);
    for_each(nums1.begin(), nums1.end(), [](const int n) { std::cout << n << ' '; });
}

int main()
{
    test();

    return 0;
}
