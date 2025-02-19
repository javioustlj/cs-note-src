
协议：
```cpp
#define NCCL_NUM_PROTOCOLS 3 // Simple/LL/LL128
#define NCCL_PROTO_UNDEF -1
#define NCCL_PROTO_LL 0
#define NCCL_PROTO_LL128 1
#define NCCL_PROTO_SIMPLE 2
```

```cpp
#define NCCL_NUM_ALGORITHMS 7 // Tree/Ring/CollNet*
#define NCCL_ALGO_UNDEF -1
#define NCCL_ALGO_TREE 0
#define NCCL_ALGO_RING 1
#define NCCL_ALGO_COLLNET_DIRECT 2
#define NCCL_ALGO_COLLNET_CHAIN 3
#define NCCL_ALGO_NVLS 4
#define NCCL_ALGO_NVLS_TREE 5
#define NCCL_ALGO_PAT 6
```

```cpp
typedef enum : uint8_t {
  ncclPatternRing,
  ncclPatternRingTwice,
  ncclPatternPipelineFrom,
  ncclPatternPipelineTo,
  ncclPatternTreeUp,
  ncclPatternTreeDown,
  ncclPatternTreeUpDown,
  ncclPatternCollnetChain,
  ncclPatternCollnetDirect,
  ncclPatternNvls,
  ncclPatternNvlsTree,
  ncclPatternPatUp,
  ncclPatternPatDown,
  ncclPatternSend,
  ncclPatternRecv
} ncclPattern_t;
```



```
ncclAllGather
ncclEnqueueCheck
```



| Variable Name  | Meaning                           |
| :------------- | :-------------------------------- |
| recvDim        | receive dimension                 |
| sendDim        | send dimension                    |
| recvOffset     | receive offset                    |
| sendOffset     | send offset                       |
| recvStepOffset | receive step offset               |
| postRecv       | post receive                      |
| postSend       | post send                         |
| nelem          | n elements                        |
| inpIx          | input index                       |
| outIx          | output index                      |
| stepSize       | step size                         |
| stepDepth      | step depth                        |
| chunkCount     | chunk count                       |
| aggDelta       | aggregation delta                 |
| aggFactor      | aggregation factor                |
| nrPow2         | number of ranks to the power of 2 |
| postFreq       | post frequency                    |
recvPow2













| run            | getNextOp      | patCopy        |
| :------------- | :------------- | :------------- |
| recvDim        | recvDim        | recvPow2       |
| sendDim        | sendDim        | sendPow2       |
| inpIx          | inpIx          | inpIx          |
| outIx          | outIx          | outIx          |
| recvOffset     | recvOffset     | recvOffset     |
| sendOffset     | sendOffset     | sendOffset     |
| recvStepOffset | recvStepOffset | recvStepOffset |
| nelem          | nelemOut       | nelem          |
| postRecv       | postRecv       | postRecv       |
| postSend       | postSend       | postSend       |
| last           | last           |







| run                     | constructor   | value     |
| :---------------------- | :------------ | :-------- |
| tid                     | tid           | tid       |
| nthreads                | nthreads      | nthreads  |
| NULL                    | recvPeers     | NULL      |
| NULL                    | sendPeers     | NULL      |
| inputBuf                | inputBuf      | inputBuf  |
| outputBuf               | outputBuf     | outputBuf |
| work->redOpArg          | redOpArg      | redOpArg  |
| 0*Proto::MaxGroupWidth, | group         | 0         |
| 0                       | connIndexRecv | 0         |
| 0                       | connIndexSend | 0         |
| nullptr                 | collWork      | nullptr   |
| nullptr                 | p2pWork       | nullptr   |
| 0                       | stepSize_     | 0         |
| primsModePatAg          | mode          | 2         |

| member                         | value                                                                                          |
| :----------------------------- | :--------------------------------------------------------------------------------------------- |
| const int tid,                 | tid                                                                                            |
| const int tidInBlock;          | threadIdx.x                                                                                    |
| const int nthreads;            | nthreads                                                                                       |
| int nworkers;                  | nthreads - (MaxSend > 0 && nthreads >= NCCL_SIMPLE_EXTRA_GROUP_IF_NTHREADS_GE ? WARP_SIZE : 0) |
| const int stepSize;            | stepSize_ == 0 ? ncclShmem.comm.buffSizes[NCCL_PROTO_SIMPLE]/NCCL_STEPS/sizeof(T) : stepSize_  |
| Fan fan;                       |
| int index;                     | tid % 32;                                                                                      |
| int flags;                     | 0x800                                                                                          |
| int group;                     | group                                                                                          |
| uint64_t step;                 |
| struct ncclConnInfo* conn;     | NULL                                                                                           |
| struct ncclConnFifo* connFifo; | NULL                                                                                           |
| T* connEltsFifo;               |
| T* directBuff = NULL;          |
| uint64_t *connStepPtr;         |
| uint64_t connStepCache;        |
| int      connStepSize;         |
| void*    netDeviceHandle;      |
| uint64_t accSize;              | 0                                                                                              |















$$
stepSize = chunkCount * sizeof(T)
$$

```cpp
#define NCCL_STEPS 8
```

$$
stepDepth = NCCL_STEPS = 8
$$


```cpp
template<typename Int>
__host__ __device__ inline void ncclCollCbdPart(
    struct ncclDevWorkColl* work, uint32_t channelId, int proto, int eltSize,
    Int* count, Int* partOffset, Int* partCount, Int* chunkCount
  ) {
  // eltPerGrain = 512 / element size;
  int eltPerGrain = ncclProtoGrainSize(proto)/eltSize;
  int nMidChannels = work->channelHi - work->channelLo - 1;
  // We can assum that nMidChannels<0 implies countMid==0, which let's us assume
  // that countMid*nMidChannels == 0.
  if (count != nullptr) {
    *count = work->cbd.countLo + work->cbd.countMid*nMidChannels + work->cbd.countHi;
  }
  if (channelId == work->channelLo) {
    *partOffset = 0;
    *partCount = work->cbd.countLo;
    *chunkCount = work->cbd.chunkGrainsLo*eltPerGrain;
  } else if (channelId == work->channelHi) {
    *partOffset = work->cbd.countLo + nMidChannels*work->cbd.countMid;
    *partCount = work->cbd.countHi;
    *chunkCount = work->cbd.chunkGrainsHi*eltPerGrain;
  } else {
    int mid = channelId - work->channelLo - 1;
    *partOffset = work->cbd.countLo + mid*work->cbd.countMid;
    *partCount = work->cbd.countMid;
    *chunkCount = work->cbd.chunkGrainsMid*eltPerGrain;
  }
}
```





## PatAGAlgorithm

Function call
| run                          | patAlgo    | patCopy        |
| :--------------------------- | :--------- | :------------- |
| chunkCount*sizeof(T)         | stepSize   | recvPow2       |
| NCCL_STEPS                   | stepDepth  | sendPow2       |
| channelOffset                | offset     | inpIx          |
| channelOffset + channelCount | end        | outIx          |
| count                        | count      | recvOffset     |
| chunkCount                   | chunkCount | sendOffset     |
| rank                         | rank       | recvStepOffset |
| nranks                       | nranks     | nelem          |





PatAGAlgorithm constructor
| formal parameter | actual parameter             | indiction |
| :--------------- | :--------------------------- | :-------- |
| int stepSize     | chunkCount*sizeof(T)         |
| int stepDepth    | NCCL_STEPS                   |8
| size_t offset    | channelOffset                |
| size_t end       | channelOffset + channelCount |
| size_t count     | count                        |
| int chunkCount   | chunkCount                   |
| int rank         | rank                         |
| int nranks       | nranks                       |





PatAGAlgorithm member
| member               | value | indiction |
| :------------------- | :---- | :-------- |
| size_t offset;       |channelOffset 
| size_t end;          |channelOffset + channelCount
| size_t count;        |count | 所有的count
| int chunkCount;      |chunkCount| channel里面element的数量
| int nelem;           |
| int rank;            |rank
| int nranks;          |nranks
| int nrPow2;          |1<<log2Up(nranks)
| int postFreq;        |
| int lastA;           |
| int aggFactor;       |
| int as;              |
| int a;               |
| int aggDelta;        |
| int scale;           |
| int phase;           |
| int asDim;           |
| int v;               |
| int bitCount[32];    |
| int bitZeroStep[32]; |


stepSize = chunkCount*sizeof(T) 
channelCount * sizeof(T) *aggFactor

chunkCount / (channelCount * aggFactor)


```cpp

   __device__ __host__ PatAGAlgorithm(int stepSize, int stepDepth, size_t offset, size_t end, size_t count, int chunkCount, int rank, int nranks):
     offset(offset), end(end), count(count), chunkCount(chunkCount), rank(rank), nranks(nranks) {
    aggDelta = nrPow2 = (1<<log2Up(nranks));

    aggFactor = 1;
    size_t channelSize = end-offset; // channelSize = channelCount
    while (stepSize / (channelSize*sizeof(T)*aggFactor) >= 2 && aggFactor < nranks/2) {
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

```



#### a

a 代表是第几个step

__device__ __host__ void resetA() {
a = 0;
lastA = aggFactor;
if (phase >= 2) lastA /= 2*scale;
}

phase == 0:
    a++;

phase == 1:

    if (++a == lastA) {
    if (as % 2 == 1) {
        phase = 0;
    } else {
        as = nextAs();
    }
    resetA();
    }

phase == 2:
    if (++a == lastA) {
    scale /= 2;
    phase = scale ? 2 : 1;
    resetA();
    }


#### as

  __device__ __host__ void reset() {
    nelem = getNelem();
    scale = aggFactor/2;
    phase = scale ? 2 : 1;
    v = 0;
    for (int i = 0; i<asDim; i++) {
      bitCount[i] = asDim-i;
      bitZeroStep[i] = 1;
    }
    as = nextAs();
    resetA();
  }

phase == 0:
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

phase == 1:

      if (++a == lastA) {
        if (as % 2 == 1) {
          phase = 0;
        } else {
          as = nextAs();
        }
        resetA();
      }

phase == 2:



#### others
element --> Grain --> count --> channels
int (4 byte) --> Grain (512 byte) --> 








一个rank一次收发比如1M的数据，这个1M数据就是一个chunk
一个chunk有多个step，即ALLREDUCE_CHUNKSTEPS
所以chunkSize就是stepSize * ALLREDUCE_CHUNKSTEPS
在directSend内部，会将chunk切分为多个slice，一个slice也是多个step，prmitives里数据通信和同步的实际粒度为slice。



nranks = 31
aggDelta = nrPow2 = 32
aggFactor = 1



https://github.com/gpu-mode/lectures/tree/main



