#include <iostream>
#include <iomanip>
#include <tuple>

template <typename Int>
int log2UP(Int x)
{
    if (x == 0)
    {
        return 0;
    }
    x -= 1;
    int res = 0;
    while (x > 0)
    {
        x >>= 1;
        ++res;
    }
    return res;
}


std::tuple<int, int, int> getAggParameter(int nranks, int stepDepth)
{
    int aggFactor = 1;
    int nrPow2 = (1 << log2UP(nranks));
    int aggDelta = nrPow2;
    int d = stepDepth;
    while (d > 1 && aggFactor < nranks/2) {
      d /= 2;
      aggFactor *= 2;
      aggDelta /= 2;
    }
    return std::make_tuple(nrPow2, aggFactor, aggDelta);
}

void func()
{

    std::cout << std::left << std::setw(10) << std::setfill(' ') << "stepDepth"
              << std::left << std::setw(10) << std::setfill(' ') << "nranks"
              << std::left << std::setw(10) << std::setfill(' ') << "nrPow2"
              << std::left << std::setw(10) << std::setfill(' ') << "aggFactor"
              << std::left << std::setw(10) << std::setfill(' ') << "aggDelta"
              << std::endl;
    std::cout << std::string(50, '-') << std::endl;


    int stepDepth = 8;
    for (int i = 1; i < 150; ++i)
    {
        int nranks = i;
        auto [nrPow2, aggFactor, aggDelta] = getAggParameter(nranks, stepDepth);
        std::cout << std::left << std::setw(10) << std::setfill(' ') << stepDepth
                  << std::left << std::setw(10) << std::setfill(' ') << nranks
                  << std::left << std::setw(10) << std::setfill(' ') << nrPow2
                  << std::left << std::setw(10) << std::setfill(' ') << aggFactor
                  << std::left << std::setw(10) << std::setfill(' ') << aggDelta
                  << std::endl;
    }
}

int main()
{
    func();
    return 0;
}
