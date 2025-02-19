#include <iostream>
#include <iomanip>


class PAT
{
private:

    int as;

    // AS computation
    int asDim;
    int v;
    int bitCount[32];
    int bitZeroStep[32];

    int firstBitSet(int, int);
    void reset();
public:
    PAT(int asDim) : asDim(asDim) {reset();};
    int nextAs();
    void print();
    void getNextOp(int &);
};

int PAT::firstBitSet(int i, int max)
{
    int ffs = __builtin_ffs(i);
    return ffs ? ffs - 1 : max;
}

void PAT::reset()
{
    v = 0;
    for (int i = 0; i < asDim; i++) {
        bitCount[i] = asDim - i;
        bitZeroStep[i] = 1;
    }
    // as = nextAs();
}

int PAT::nextAs()
{
    for (int d=0; d<asDim; d++) {
        int p = 1<<d;
        bitCount[d]--;
        if (bitCount[d] == 0) {
            v ^= p;
            bitCount[d] = p;
            if ((v&p) == 0) {
                // std::cout << "v&p == 0" << std::endl;
                // exit(0);
                bitCount[d] += firstBitSet(bitZeroStep[d], asDim) - 1;
                if (bitCount[d] == 0) {
                v ^= p;
                bitCount[d] = p;
                }
                bitZeroStep[d]++;
            }
        }
    }

    // as = v;

    return v;
}

void PAT::print()
{
    // if (as == 2)
    //     return;
    std::cout << "\nas: " << as << '\n';
    std::cout << "index       : ";
    for (int i = 0; i < asDim; ++i)
        std::cout << std::left << std::setw(5) << std::setfill(' ') << i;
    std::cout << '\n';
    std::cout << "bitCount    : ";
    for (int i = 0; i < asDim; ++i)
        std::cout << std::left << std::setw(5) << std::setfill(' ') << bitCount[i];
    std::cout << '\n';
    std::cout << "bitZeroStep : ";
    for (int i = 0; i < asDim; ++i)
        std::cout << std::left << std::setw(5) << std::setfill(' ') << bitZeroStep[i];
    std::cout << '\n' << std::endl;
}

void func()
{
    PAT pat(5);

    // pat.print();
    int as;
    int firstAs = pat.nextAs();

    std::cout << firstAs;
    // std::cout << "\n\n\n" << std::endl;
    for (int i = 0; i < 10000; ++i)
    {
        as = pat.nextAs();
        // pat.print();
        if (as == firstAs)
        {
            std::cout << '\n' << as;
        }
        else
        {
            std::cout << ' ' << as;
        }
    }
    std::cout << std::endl;
}

int main()
{
    func();
    return 0;
}
