#include <iostream>
#include <iomanip>

int cnt = 0;

template<typename Int>
inline int log2Up(Int x)
{
    int w, n;
    if (x != 0) x -= 1;
    if (x == 0) {
        return 0;
    } else if (sizeof(Int) <= sizeof(unsigned int)) {
        w = 8*sizeof(unsigned int);
        n = __builtin_clz((unsigned int)x);
    } else if (sizeof(Int) <= sizeof(unsigned long)) {
        w = 8*sizeof(unsigned long);
        n = __builtin_clzl((unsigned long)x);
    } else if (sizeof(Int) <= sizeof(unsigned long long)) {
        w = 8*sizeof(unsigned long long);
        n = __builtin_clzll((unsigned long long)x);
    } else {
        static_assert(sizeof(Int) <= sizeof(unsigned long long), "Unsupported integer size.");
    }

    return w-n;
}

class PAT
{
private:

    size_t offset;
    size_t end;
    int chunkCount;
    int nranks;
    int nrPow2;
    int postFreq;
    int lastA;

    int aggFactor;
    int as; // aggregated steps
    int a; // step inside aggregated step
    int aggDelta;

    int scale;
    int phase;

    // AS computation
    int asDim;
    int v;
    int bitCount[32];
    int bitZeroStep[32];

    int firstBitSet(int, int);
    void reset();
    void resetA();
public:
    PAT(int asDim) : asDim(asDim) {reset();};
    PAT(int stepSize, int stepDepth, size_t offset, size_t end, int chunkCount, int nranks);
    int nextAs();
    void print();
    void getNextOp(int &, int &);
};

PAT::PAT(int stepSize, int stepDepth, size_t offset, size_t end, int chunkCount, int nranks)
    : offset(offset)
    , end(end)
    , chunkCount(chunkCount)
    , nranks(nranks)
{
    aggDelta = nrPow2 = (1<<log2Up(nranks));
    aggFactor = 1;
    size_t channelSize = end-offset;
    std::cout << "stepSize: " << stepSize << std::endl;
    std::cout << "channelSize*sizeof(int)*aggFactor: " << channelSize*sizeof(int)*aggFactor << std::endl;
    while (stepSize / (channelSize*sizeof(int)*aggFactor) >= 2 && aggFactor < nranks/2) {
        std::cout <<"aaa" << std::endl;
      aggFactor *= 2;
      aggDelta /= 2;
    }
    postFreq = aggFactor;
    int d = stepDepth;
    while (d > 1 && aggFactor < nranks/2) {
      d /= 2;
      aggFactor *= 2;
      aggDelta /= 2;
    }
    //printf("AggFactor %d PostFreq %d AggDelta %d\n", aggFactor, postFreq, aggDelta);

    asDim = log2Up(aggDelta);
    reset();
}

int PAT::firstBitSet(int i, int max)
{
    int ffs = __builtin_ffs(i);
    return ffs ? ffs - 1 : max;
}

void PAT::resetA()
{
    a = 0;
    lastA = aggFactor;
    if (phase >= 2) lastA /= 2*scale;
}

void PAT::reset()
{
    scale = aggFactor / 2;
    phase = scale ? 2 : 1;
    v = 0;
    for (int i = 0; i < asDim; i++) {
        bitCount[i] = asDim - i;
        bitZeroStep[i] = 1;
    }
    as = nextAs();
    resetA();
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
    return v;
}

void PAT::print()
{
    std::cout << "\n";
    std::cout << "nrPow2: " << nrPow2 << std::endl;
    std::cout << "aggDelta: " << aggDelta << std::endl;
    std::cout << "aggFactor: " << aggFactor << std::endl;
    std::cout << "postFreq: " << postFreq << std::endl;
    std::cout << "asDim: " << asDim << std::endl;
    std::cout << "as: " << as << std::endl;
    std::cout << "a: " << a << std::endl;
    std::cout << "lastA: " << lastA << std::endl;


    std::cout << "offset: " << offset << std::endl;
    std::cout << "end: " << end << std::endl;
    std::cout << "chunkCount: " << chunkCount << std::endl;
    std::cout << "nranks: " << nranks << std::endl;
    std::cout << "scale: " << scale << std::endl;
    std::cout << "phase: " << phase << std::endl;
    std::cout << "v: " << v << std::endl;
    std::cout << "\n" << std::endl;

}

void PAT::getNextOp(int &sendDim, int &last)
{
    cnt++;
    // std::cout << "start of getNextOp: cnt = " << cnt << std::endl;
restart:
    printf("cnt: %d, Phase %d, as: %d, aggDelta: %d, a: %d, lastA: %d, scale: %d\n", cnt, phase, as, aggDelta, a, lastA, scale);
    last = 0;
    int skip = 0;
    if (phase == 0) {
      int s = a*aggDelta + as;
      if (s >= nranks) skip = 1;
      int nextSkip = (a+1)*aggDelta + as >= nranks ? 1 : 0;
      sendDim = -1;
      a++;
      if (nextSkip) {
        as = nextAs();
        if (as == aggDelta/2) {
          offset += chunkCount;
          if (offset >= end) {
            last = 1;
          } else {
            reset();
          }
          return;
        }
        phase = 1;
        resetA();
      }
    //   printf("nextSkip: %d, as: %d, aggDelta: %d, offset: %d, end: %d\n", nextSkip, as, aggDelta, offset, end);
    // printf("nextSkip: %d, as: %d, aggDelta: %d, offset: %d, end: %d", nextSkip, as, aggDelta, offset, end);
      if (skip == 0) return;
   } else if (phase == 1) {
      int s = a*aggDelta + as;
      if (s >= nranks) skip = 1;
      sendDim = firstBitSet(s, nrPow2);

      if (s < nranks && sendDim == 0 && skip) {
        // Don't forget to receive at least once even if we don't send afterwards
        sendDim = -1;
        skip = 0;
      }
      if (++a == lastA) {
        if (as % 2 == 1) {
          phase = 0;
        } else {
          as = nextAs();
        }
        resetA();
      }
    //   std::cout << "phase = 1, cnt = " << cnt << std::endl;
      if (skip == 0) return;
    } else if (phase == 2) {
      int s = (2*a+1)*scale*aggDelta;
      if (s >= nranks) skip = 1;
      if (++a == lastA) {
        scale /= 2;
        phase = scale ? 2 : 1;
        resetA();
      }
      if (skip == 0) return;
    }
    std::cout << "goto restart" << std::endl;
    goto restart;
}

void func()
{
    int nbytes = 1048576;
    // int chunkSize = 2097152;
    int chunkSize = 2048;

    int nranks = 15;
    int offset = 0;
    int stepDepth = 8;

    int channelCount = nbytes / nranks;
    int chunkCount = chunkSize / sizeof(int);

    std::cout << "channelCount: " << channelCount << std::endl;
    std::cout << "chunkCount: " << chunkCount << std::endl;

    PAT pat(chunkSize, stepDepth, offset, offset + channelCount, chunkCount, nranks);

    pat.print();
    // return;
    int last = 0;
    while (!last)
    {
        int sendDim;
        pat.getNextOp(sendDim, last);
        // pat.print();
    }
    std::cout << "last: " << last << std::endl;
}

int main()
{
    func();
    return 0;
}
