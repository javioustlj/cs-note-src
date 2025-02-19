# Definition

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

# Search Result

```cpp
// /home/e01592/work/nccl/src/collectives.cc
const char* ncclAlgoToString(int algo) {
  switch (algo) {
  case NCCL_ALGO_TREE: return "TREE";
  case NCCL_ALGO_RING: return "RING";
  case NCCL_ALGO_COLLNET_DIRECT: return "COLLNET_DIRECT";
  case NCCL_ALGO_COLLNET_CHAIN: return "COLLNET_CHAIN";
  case NCCL_ALGO_NVLS: return "NVLS";
  case NCCL_ALGO_NVLS_TREE: return "NVLS_TREE";
  case NCCL_ALGO_PAT: return "PAT";
  default: return "Unknown";
  }
}
```

```cpp
//  /home/e01592/work/nccl/src/enqueue.cc


// numPipeOps: number of pipelined ops. Can be greater than 1 in aggregation mode. Used to adjust latency.
static ncclResult_t updateCollCostTable(
    struct ncclComm* comm, struct ncclTaskColl* info, size_t nBytes,
    int collNetSupport, int nvlsSupport, int numPipeOps,
    float** collCostTable) {
  float (*table)[NCCL_NUM_PROTOCOLS] = (float (*)[NCCL_NUM_PROTOCOLS])collCostTable;

  if (comm->nRanks == 1) {
    table[NCCL_ALGO_RING][NCCL_PROTO_SIMPLE] = 0.0;
    return ncclSuccess;
  }

  for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
    if ((a == NCCL_ALGO_COLLNET_DIRECT || a == NCCL_ALGO_COLLNET_CHAIN) && collNetSupport != 1) continue;
    // CollNetDirect is only supported for up to 8 local GPUs
    if (a == NCCL_ALGO_COLLNET_DIRECT && comm->maxLocalRanks > NCCL_MAX_DIRECT_ARITY+1) continue;
    if ((a == NCCL_ALGO_NVLS || a == NCCL_ALGO_NVLS_TREE) && nvlsSupport != 1 && info->func != ncclFuncAllGather) continue;
    if (a == NCCL_ALGO_NVLS && collNetSupport != 1 && comm->nNodes > 1) continue;
    /* now we only support single-node NVLS allgather and reducescatter */
    if (a == NCCL_ALGO_NVLS && (info->func == ncclFuncAllGather || info->func == ncclFuncReduceScatter) && comm->nNodes > 1) continue;
    /* Tree reduceScatter doesn't support scaling yet */
    if (a == NCCL_ALGO_PAT && info->func == ncclFuncReduceScatter
        && (info->opDev.op == ncclDevPreMulSum || info->opDev.op == ncclDevSumPostDiv)) continue;
    for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
      NCCLCHECK(ncclTopoGetAlgoTime(comm, info->func, a, p, nBytes, numPipeOps, &table[a][p]));
      // Relegate fp8 reduction trees of sufficient depth that they incur precision loss
      // to be least preferred.
      if (info->datatype == ncclFloat8e4m3 || info->datatype == ncclFloat8e5m2) {
        if (a == NCCL_ALGO_RING && comm->nRanks > 8) {
          table[a][p] *= 1024.0; // Any factor large enough to act as a partition between lossy and non-lossy algos.
        }
      }
    }
  }

  return ncclSuccess;
}


static ncclResult_t topoGetAlgoInfo(
    struct ncclComm* comm, struct ncclTaskColl* info, size_t nBytes,
    float** collCostTable, ncclSimInfo_t* simInfo
  ) {
  float (*table)[NCCL_NUM_PROTOCOLS] = (float (*)[NCCL_NUM_PROTOCOLS])collCostTable;

  float minTime = 3600000000.0;
  int algorithm = info->algorithm = NCCL_ALGO_UNDEF;
  int protocol = info->protocol = NCCL_PROTO_UNDEF;
  for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
    for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
      if (table[a][p] == NCCL_ALGO_PROTO_IGNORE) continue;
      if (table[a][p] >= 0.0 && table[a][p] < minTime) {
        algorithm = a;
        protocol = p;
        minTime = table[a][p];
      }
    }
  }

  info->algorithm = algorithm;
  info->protocol = protocol;
  float time = minTime;

  // Yes, we are first assigning and then testing if protocol is sane, but that's OK in this case.
  // coverity[check_after_sink]
  if (info->algorithm == NCCL_ALGO_UNDEF || info->protocol == NCCL_PROTO_UNDEF) {
    char ncclAlgoEnvStr[1024] = "";
    char ncclProtoEnvStr[1024] = "";
    char* algoEnv = getenv("NCCL_ALGO");
    if (algoEnv) {
      snprintf(ncclAlgoEnvStr, 1023, " NCCL_ALGO was set to %s.", algoEnv);
    }
    char* protoEnv = getenv("NCCL_PROTO");
    if (protoEnv) {
      snprintf(ncclProtoEnvStr, 1023, " NCCL_PROTO was set to %s.", protoEnv);
    }
    WARN("Error : no algorithm/protocol available for function %s with datatype %s.%s%s", ncclFuncToString(info->func), ncclDatatypeToString(info->datatype), ncclAlgoEnvStr, ncclProtoEnvStr);
    return (algoEnv || protoEnv) ? ncclInvalidUsage : ncclInternalError;
  }
  if (simInfo) simInfo->estimatedTime = time;
  TRACE(NCCL_COLL, "%ld Bytes -> Algo %d proto %d time %f", nBytes, info->algorithm, info->protocol, time);

  int nc = comm->nChannels;
  int nt = comm->maxThreads[info->algorithm][info->protocol];
  int threadThreshold = comm->threadThresholds[info->algorithm][info->protocol];
  if (info->algorithm == NCCL_ALGO_COLLNET_DIRECT) {
    // CollNet channel tuning
    int ncSwitch = 16;
    bool flag = true;
    while (ncSwitch >= 1 && flag) {
      while ((flag = nBytes < nc*nt*comm->channels[0].collnetDirect.nHeads*threadThreshold) && nc > ncSwitch) {
        if (nc == ncSwitch+ncSwitch/2) threadThreshold /= 2;
        nc--;
      }
      ncSwitch /= 2;
    }
  } else if (info->algorithm == NCCL_ALGO_NVLS || info->algorithm == NCCL_ALGO_NVLS_TREE) {
    // NVLS should not need more than 16 channels to get peak BW.
    nc = comm->nvlsChannels;
  } else {
    // Ring/Tree channel tuning
    while (nBytes < nc * nt * threadThreshold) {
      if (nc >= 2) nc--;
      else break;
    }
  }

  if (info->algorithm != NCCL_ALGO_NVLS && info->algorithm != NCCL_ALGO_NVLS_TREE &&
    info->algorithm != NCCL_ALGO_COLLNET_DIRECT) {
    while (nBytes < nc * nt * threadThreshold) {
      if (nt % 128 == 0) nt /= 2;
      else break;
    }
  }
  if (info->protocol == NCCL_PROTO_SIMPLE) {
    if (info->algorithm == NCCL_ALGO_RING) nt += WARP_SIZE; // Extra warp for sync
    // More threads or sync warps needed due to split thread model
    if (info->algorithm == NCCL_ALGO_TREE) nt += 4*WARP_SIZE;
  }
  nt = nt/WARP_SIZE < 3 ? 3*WARP_SIZE : nt;
  if (info->algorithm == NCCL_ALGO_TREE) nt = NCCL_MAX_NTHREADS; // Tree now uses all threads always.
  if (info->algorithm == NCCL_ALGO_PAT) nt = NCCL_MAX_NTHREADS;
  info->nMaxChannels = nc;
  info->nWarps = nt/WARP_SIZE;
  return ncclSuccess;
}


static ncclResult_t calcCollChunking(
    struct ncclComm* comm, struct ncclTaskColl* info, int nChannels, size_t nBytes,
    /*outputs*/uint32_t* outChunkSize, uint32_t* outDirectFlags, struct ncclProxyOp* proxyOp
  ) {
  ncclPattern_t pattern;
  size_t grainSize = ncclProtoGrainSize(info->protocol);

  switch (info->func) {
  case ncclFuncBroadcast:
    pattern = info->algorithm == NCCL_ALGO_TREE ? ncclPatternTreeDown : ncclPatternPipelineFrom;
    break;
  case ncclFuncReduce:
    pattern = info->algorithm == NCCL_ALGO_TREE ? ncclPatternTreeUp : ncclPatternPipelineTo;
    break;
  case ncclFuncReduceScatter:
    pattern =
      info->algorithm == NCCL_ALGO_PAT ? ncclPatternPatUp :
      info->algorithm == NCCL_ALGO_NVLS ? ncclPatternNvls :
      info->algorithm == NCCL_ALGO_COLLNET_DIRECT ? ncclPatternCollnetDirect :
      ncclPatternRing;
    break;
  case ncclFuncAllGather:
    pattern =
      info->algorithm == NCCL_ALGO_PAT ? ncclPatternPatDown :
      info->algorithm == NCCL_ALGO_NVLS ? ncclPatternNvls :
      info->algorithm == NCCL_ALGO_COLLNET_DIRECT ? ncclPatternCollnetDirect :
      ncclPatternRing;
    break;
  case ncclFuncAllReduce:
    pattern =
      info->algorithm == NCCL_ALGO_NVLS ? ncclPatternNvls :
      info->algorithm == NCCL_ALGO_NVLS_TREE ? ncclPatternNvlsTree :
      info->algorithm == NCCL_ALGO_COLLNET_DIRECT ? ncclPatternCollnetDirect :
      info->algorithm == NCCL_ALGO_COLLNET_CHAIN ? ncclPatternCollnetChain :
      info->algorithm == NCCL_ALGO_TREE ? ncclPatternTreeUpDown :
      ncclPatternRingTwice;
    break;
  default:
    WARN("Unknown pattern for collective %d algorithm %d", info->func, info->algorithm);
    return ncclInternalError;
  }

  int nstepsPerLoop, nchunksPerLoop;
  size_t loopOffset = 0;
  int stepSize   = comm->buffSizes[info->protocol]/NCCL_STEPS;
  int chunkSteps = (info->protocol == NCCL_PROTO_SIMPLE && info->algorithm == NCCL_ALGO_RING) ? info->chunkSteps : 1;
  int sliceSteps = (info->protocol == NCCL_PROTO_SIMPLE && info->algorithm == NCCL_ALGO_RING) ? info->sliceSteps : 1;
  int chunkSize = stepSize*chunkSteps;
  if (info->protocol == NCCL_PROTO_LL) chunkSize /= 2;
  if (info->protocol == NCCL_PROTO_LL128) chunkSize = (chunkSize / NCCL_LL128_LINEELEMS) * NCCL_LL128_DATAELEMS;

  if (info->algorithm == NCCL_ALGO_COLLNET_DIRECT) {
    // Optimize chunkSize / nSteps
    while (nBytes / (nChannels * comm->channels[0].collnetDirect.nHeads * chunkSize) < comm->channels[0].collnetDirect.depth * 64 && chunkSize > 131072) chunkSize /= 2;
    while (nBytes / (nChannels * comm->channels[0].collnetDirect.nHeads * chunkSize) < comm->channels[0].collnetDirect.depth * 8 && chunkSize > 65536) chunkSize /= 2;
    while (nBytes / (nChannels * comm->channels[0].collnetDirect.nHeads * chunkSize) < comm->channels[0].collnetDirect.depth * 8 && chunkSize > 32768) chunkSize /= 2;
  } else if (info->algorithm == NCCL_ALGO_COLLNET_CHAIN) {
    stepSize = comm->buffSizes[NCCL_PROTO_SIMPLE] / NCCL_STEPS;
    chunkSize = std::min(256 * 1024, stepSize * chunkSteps);
    while (nBytes / (nChannels * chunkSize) < comm->channels[0].collnetChain.depth * 64 && chunkSize > 131072) chunkSize /= 2;
    while (nBytes / (nChannels * chunkSize) < comm->channels[0].collnetChain.depth * 8 && chunkSize > 65536) chunkSize /= 2;
    while (nBytes / (nChannels * chunkSize) < comm->channels[0].collnetChain.depth && chunkSize > 32768) chunkSize /= 2;
  } else if (info->algorithm == NCCL_ALGO_NVLS) {
    int maxChunkSize = comm->nvlsChunkSize;
    if (comm->nNodes > 1 && comm->bandwidths[ncclFuncAllReduce][NCCL_ALGO_NVLS][NCCL_PROTO_SIMPLE] < 150) maxChunkSize = 32768;
    if (chunkSize > maxChunkSize) chunkSize = maxChunkSize;
    // Use uint64_t so that concurrentOps*chunkSize*X does not overflow.
    // However, nChannels * comm->channels[0].nvls.nHeads should easily fit in 32 bits.
    // coverity[overflow_before_widen]
    uint64_t concurrentOps = nChannels * comm->channels[0].nvls.nHeads;
    if ((nBytes < (64 * (concurrentOps * chunkSize))) && (chunkSize > 65536)) chunkSize = 65536;
    if ((nBytes < (8 * (concurrentOps * chunkSize))) && (chunkSize > 32768)) chunkSize = 32768;
    if ((nBytes < (2 * (concurrentOps * chunkSize))) && (chunkSize > 16384)) chunkSize = 16384;
  } else if (info->algorithm == NCCL_ALGO_NVLS_TREE) {
    // Use uint64_t so that concurrentOps*chunkSize*X does not overflow.
    // However, nChannels * comm->channels[0].nvls.nHeads should easily fit in 32 bits.
    // coverity[overflow_before_widen]
    uint64_t concurrentOps = nChannels * comm->channels[0].nvls.nHeads;
    chunkSize = comm->nvlsChunkSize;
    int maxChunkSize = (int)ncclParamNvlsTreeMaxChunkSize();
    if (maxChunkSize == -2) maxChunkSize = comm->nNodes >= 4 ? 65536 : chunkSize;
    chunkSize = std::min(chunkSize, maxChunkSize);
    if ((nBytes < (32 * (concurrentOps * chunkSize))) && (chunkSize > 262144)) chunkSize = 262144;
    if ((nBytes < (16 * (concurrentOps * chunkSize))) && (chunkSize > 131072)) chunkSize = 131072;
    if ((nBytes < (4 * (concurrentOps * chunkSize))) && (chunkSize > 65536)) chunkSize = 65536;
    if ((nBytes < (1 * (concurrentOps * chunkSize))) && (chunkSize > 32768)) chunkSize = 32768;
  } else if (info->algorithm == NCCL_ALGO_TREE && info->protocol == NCCL_PROTO_LL128) {
    int nNodes = comm->nNodes;
    float ppn = comm->nRanks / (float)nNodes;
    float nstepsLL128 = 1+log2i(nNodes) + 0.1*ppn;
    // Yes, we are OK with the division on the left side of the < operand being integer.
    // coverity[integer_division]
    while (nBytes / (nChannels*chunkSize) < nstepsLL128*64/ppn && chunkSize > 131072) chunkSize /= 2;
    // coverity[integer_division]
    while (nBytes / (nChannels*chunkSize) < nstepsLL128*16/ppn && chunkSize > 32768) chunkSize /= 2;
  } else if (info->func == ncclFuncAllGather && info->algorithm == NCCL_ALGO_PAT) {
    while (chunkSize*nChannels*32 > nBytes && chunkSize > 65536) chunkSize /= 2;
  } else if (info->func == ncclFuncReduceScatter && info->algorithm == NCCL_ALGO_PAT) {
    while (chunkSize*nChannels*16 > nBytes && chunkSize > 65536) chunkSize /= 2;
  }

  // Compute directFlags of work struct.
  if (info->algorithm == NCCL_ALGO_COLLNET_DIRECT) {
    // Set direct direction for broadcast-gather (read or write)
    *outDirectFlags = (nBytes/nChannels <= 1024 * 4) ? NCCL_P2P_READ : NCCL_P2P_WRITE;
  } else {
    *outDirectFlags = 0;
  }

  // Compute nSteps for proxies
  chunkSize = chunkSize / grainSize * grainSize; // align chunkSize to multiple grainSize
  switch (pattern) {
  case ncclPatternTreeUp:
  case ncclPatternTreeDown:
  case ncclPatternTreeUpDown:
  case ncclPatternPatUp:
  case ncclPatternPatDown:
  case ncclPatternPipelineFrom:
  case ncclPatternPipelineTo:
  case ncclPatternCollnetChain:
    nstepsPerLoop = nchunksPerLoop = 1;
    break;
  case ncclPatternNvls:
    nstepsPerLoop = 1; nchunksPerLoop = comm->channels[0].nvls.nHeads;
    loopOffset = nChannels * chunkSize * comm->channels[0].nvls.headRank;
    break;
  case ncclPatternCollnetDirect:
    nstepsPerLoop = 1; nchunksPerLoop = comm->channels[0].collnetDirect.nHeads;
    loopOffset = nChannels * chunkSize * comm->channels[0].collnetDirect.headRank;
    break;
  case ncclPatternRing:
    nstepsPerLoop = comm->nRanks-1; nchunksPerLoop = comm->nRanks;
    break;
  case ncclPatternRingTwice:
    nstepsPerLoop = 2*(comm->nRanks-1); nchunksPerLoop = comm->nRanks;
    break;
  case ncclPatternNvlsTree:
    nstepsPerLoop = 1; nchunksPerLoop = comm->channels[0].nvls.nHeads;
    break;
  default:
    WARN("Unknown pattern %d", pattern);
    return ncclInternalError;
  }

  // Compute nSteps for proxies
  size_t loopSize = size_t(nChannels)*nchunksPerLoop*chunkSize;
  int nLoops = (int)DIVUP(nBytes, loopSize);
  memset(proxyOp, 0, sizeof(*proxyOp));
  proxyOp->nsteps = nstepsPerLoop * nLoops * chunkSteps;
  proxyOp->sliceSteps = sliceSteps;
  proxyOp->chunkSteps = chunkSteps;
  proxyOp->chunkSize = chunkSize;
  proxyOp->sliceSize = chunkSize / chunkSteps * sliceSteps;
  proxyOp->loopSize = loopSize;
  proxyOp->loopOffset = loopOffset;
  proxyOp->protocol = info->protocol;
  proxyOp->dtype = info->datatype;
  proxyOp->algorithm = info->algorithm;
  if (info->opDev.op == ncclDevPreMulSum || info->opDev.op == ncclDevSumPostDiv) {
    proxyOp->redOp = ncclSum; // Network sees avg as sum
  } else {
    proxyOp->redOp = info->opHost;
  }
  proxyOp->pattern = pattern;
  proxyOp->coll = info->func;
  proxyOp->root = info->root;
  proxyOp->isOneRPN = comm->isOneRPN;
  // This is used by P2P to reduce the receive buffer size. We don't use it in collectives
  // because some protocols need to transmit more than the total size, plus they sometimes
  // round up
  proxyOp->nbytes = stepSize*sliceSteps;

  if (info->regBufType & NCCL_NET_REG_BUFFER) {
    proxyOp->reg = 1;
    if (info->algorithm == NCCL_ALGO_COLLNET_DIRECT || info->algorithm == NCCL_ALGO_NVLS || info->algorithm == NCCL_ALGO_COLLNET_CHAIN) {
      if (proxyOp->isOneRPN) {
        proxyOp->nsteps = 1;
        proxyOp->loopOffset = 0;
        proxyOp->sendbuff = (uint8_t*)info->sendbuff;
        proxyOp->sendMhandle = info->sendMhandle;
      } else {
        if (info->func == ncclFuncAllGather || info->func == ncclFuncReduceScatter) {
          proxyOp->nbytes = nBytes / nchunksPerLoop;
          proxyOp->loopSize = proxyOp->loopSize / nchunksPerLoop;
          proxyOp->loopOffset = 0;
          if (info->func == ncclFuncAllGather) {
            proxyOp->sendbuff = (uint8_t*)info->sendbuff;
            proxyOp->sendMhandle = info->sendMhandle;
          }
        } else {
          proxyOp->sendbuff = (uint8_t*)info->recvbuff;
          proxyOp->sendMhandle = info->recvMhandle;
        }
      }
    } else if (info->algorithm == NCCL_ALGO_RING) {
      if (proxyOp->isOneRPN && info->func == ncclFuncAllGather) {
        proxyOp->chunkSize = NCCL_MAX_NET_SIZE;
        proxyOp->sliceSize = NCCL_MAX_NET_SIZE;
        proxyOp->chunkSteps = 1;
        proxyOp->sliceSteps = 1;
        proxyOp->loopSize = size_t(nChannels) * nchunksPerLoop * proxyOp->chunkSize;
        proxyOp->nsteps = DIVUP(nBytes, proxyOp->loopSize) * nstepsPerLoop;
        proxyOp->loopOffset = 0;
      }
    } else {
      WARN("Net registration invalid algorithm %s", ncclAlgoToString(info->algorithm));
      return ncclInternalError;
    }

    proxyOp->recvMhandle = info->recvMhandle;
    proxyOp->recvbuff = (uint8_t*)info->recvbuff;
    proxyOp->nbytes = nBytes;
  } else {
    proxyOp->reg = 0;
  }

  if (pattern == ncclPatternCollnetDirect) {
    proxyOp->specifics.collnetDirect.nNodes = comm->nNodes;
    proxyOp->specifics.collnetDirect.node = comm->node;
    if (info->func == ncclFuncAllGather || info->func == ncclFuncReduceScatter) {
      proxyOp->specifics.collnetDirect.sizePerRank = info->count*ncclTypeSize(info->datatype);
    }
  }

  if (pattern == ncclPatternPatUp || pattern == ncclPatternPatDown) {
    proxyOp->nbytes = DIVUP(nBytes, nChannels);
  }

  *outChunkSize = proxyOp->chunkSize;
  return ncclSuccess;
}
```


```cpp
// /home/e01592/work/nccl/src/group.cc


ncclResult_t ncclCollPreconnectFunc(struct ncclAsyncJob* job_) {
  struct ncclPreconnectJob* job = (struct ncclPreconnectJob*)job_;
  struct ncclComm* comm = job->comm;
  ncclResult_t ret = ncclSuccess;

  CUDACHECK(cudaSetDevice(comm->cudaDev));
  if (CPU_COUNT(&comm->cpuAffinity)) sched_setaffinity(0, sizeof(cpu_set_t), &comm->cpuAffinity);
  for (int i = 0; i < NCCL_NUM_ALGORITHMS; ++i) {
    if (job->algoNeedConnect[i]) {
      switch (i) {
        case NCCL_ALGO_RING: {
          NCCLCHECKGOTO(ncclTransportRingConnect(comm), ret, fail);
          break;
        }
        case NCCL_ALGO_TREE: {
          NCCLCHECKGOTO(ncclTransportTreeConnect(comm), ret, fail);
          break;
        }
        case NCCL_ALGO_NVLS: {
          /* If we are using NVLS_TREE algo, we must mark NVLS algo to set up
           * NVLS intra-node buffer */
          NCCLCHECKGOTO(ncclNvlsBufferSetup(comm), ret, fail);
          break;
        }
        case NCCL_ALGO_NVLS_TREE: {
          NCCLCHECKGOTO(ncclNvlsTreeConnect(comm), ret, fail);
          break;
        }
        case NCCL_ALGO_COLLNET_CHAIN: {
          NCCLCHECKGOTO(ncclCollNetChainBufferSetup(comm), ret, fail);
          break;
        }
        case NCCL_ALGO_COLLNET_DIRECT: {
          NCCLCHECKGOTO(ncclCollNetDirectBufferSetup(comm), ret, fail);
          break;
        }
        case NCCL_ALGO_PAT: {
          NCCLCHECKGOTO(ncclTransportPatConnect(comm), ret, fail);
          break;
        }
        // Yes, it's a dead code.  That's fine...
        // coverity[dead_error_begin]
        default: {
          ret = ncclInternalError;
          goto fail;
        }
      }
    }
  }

exit:
  free(job->algoNeedConnect);
  return ret;
fail:
  goto exit;
}
```



```cpp
// /home/e01592/work/nccl/src/device/all_gather.h

template<typename T, typename RedOp>
struct RunWorkColl<ncclFuncAllGather, T, RedOp, NCCL_ALGO_PAT, NCCL_PROTO_SIMPLE> {
  __device__ __forceinline__ void run(int tid, int nthreads, struct ncclDevWorkColl* work) {
    using Proto = ProtoSimple<1, 1>;
    const int nranks = ncclShmem.comm.nRanks;
    const int rank = ncclShmem.comm.rank;
    size_t count, channelOffset, channelCount, chunkCount;
    ncclCollCbdPart(work, ncclShmem.channelId, Proto::Id, sizeof(T), &count, &channelOffset, &channelCount, &chunkCount);

    T *inputBuf = (T*)work->sendbuff;
    T *outputBuf = (T*)work->recvbuff;
    Primitives<T, RedOp, FanSymmetric<1>, 0, Proto, 0> prims
      (tid, nthreads, NULL, NULL, inputBuf, outputBuf, work->redOpArg, 0*Proto::MaxGroupWidth, 0, 0, nullptr, nullptr, 0, primsModePatAg);

    PatAGAlgorithm<T> patAlgo(chunkCount*sizeof(T), NCCL_STEPS, channelOffset, channelOffset + channelCount, count, chunkCount, rank, nranks);
    int last = 0;
    while (!last) {
      int recvDim, sendDim, recvOffset, sendOffset, recvStepOffset, postRecv, postSend, nelem;
      size_t inpIx, outIx;
      patAlgo.getNextOp(recvDim, sendDim, inpIx, outIx, recvOffset, sendOffset, recvStepOffset, nelem, postRecv, postSend, last);
      prims.patCopy(recvDim, sendDim, inpIx, outIx, recvOffset, sendOffset, recvStepOffset, nelem, postRecv, postSend);
    }
  }
};
```

```cpp
// /home/e01592/work/nccl/src/device/reduce_scatter.h

template<typename T, typename RedOp>
struct RunWorkColl<ncclFuncReduceScatter, T, RedOp, NCCL_ALGO_PAT, NCCL_PROTO_SIMPLE> {
  __device__ __forceinline__ void run(int tid, int nthreads, struct ncclDevWorkColl* work) {
    using Proto = ProtoSimple<1, 1>;
    const int nranks = ncclShmem.comm.nRanks;
    const int rank = ncclShmem.comm.rank;
    size_t count, channelOffset, channelCount, chunkCount;
    ncclCollCbdPart(work, ncclShmem.channelId, Proto::Id, sizeof(T), &count, &channelOffset, &channelCount, &chunkCount);

    T *inputBuf = (T*)work->sendbuff;
    T *outputBuf = (T*)work->recvbuff;
    Primitives<T, RedOp, FanSymmetric<1>, 0, Proto, 0> prims
      (tid, nthreads, NULL, NULL, inputBuf, outputBuf, work->redOpArg, 0*Proto::MaxGroupWidth, 0, 0, nullptr, nullptr, 0, primsModePatRs);

    PatRSAlgorithm<T> patAlgo(chunkCount*sizeof(T), NCCL_STEPS, channelOffset, channelOffset + channelCount, count, chunkCount, rank, nranks);
    int last = 0;
    while (!last) {
      int recvDim, sendDim, recvOffset, sendOffset, sendStepOffset, postRecv, postSend, nelem;
      size_t inpIx, outIx;
      patAlgo.getNextOp(recvDim, sendDim, inpIx, outIx, recvOffset, sendOffset, sendStepOffset, nelem, postRecv, postSend, last);
      prims.patReduce(recvDim, sendDim, inpIx, outIx, recvOffset, sendOffset, sendStepOffset, nelem, postRecv, postSend);
    }
  }
};
```

```cpp
// /home/e01592/work/nccl/src/graph/tuning.cc

ncclResult_t ncclTopoTuneModel(struct ncclComm* comm, int minCompCap, int maxCompCap, struct ncclTopoGraph** graphs) {
  int simpleDefaultThreads = (graphs[NCCL_ALGO_RING]->bwIntra*graphs[NCCL_ALGO_RING]->nChannels <= PCI_BW) ? 256 : NCCL_SIMPLE_MAX_NTHREADS;
  comm->maxThreads[NCCL_ALGO_RING][NCCL_PROTO_SIMPLE] =
    getNthreads("NCCL_NTHREADS", ncclParamNthreads(), 2*WARP_SIZE, NCCL_SIMPLE_MAX_NTHREADS, simpleDefaultThreads);
  comm->maxThreads[NCCL_ALGO_TREE][NCCL_PROTO_SIMPLE] =
    getNthreads("NCCL_NTHREADS", ncclParamNthreads(), 2*WARP_SIZE, NCCL_SIMPLE_MAX_NTHREADS, NCCL_SIMPLE_MAX_NTHREADS);
  comm->maxThreads[NCCL_ALGO_COLLNET_DIRECT][NCCL_PROTO_SIMPLE] =
    comm->maxThreads[NCCL_ALGO_COLLNET_CHAIN][NCCL_PROTO_SIMPLE] =
    comm->maxThreads[NCCL_ALGO_NVLS][NCCL_PROTO_SIMPLE] =
    comm->maxThreads[NCCL_ALGO_NVLS_TREE][NCCL_PROTO_SIMPLE] = NCCL_MAX_NTHREADS;
  comm->maxThreads[NCCL_ALGO_RING][NCCL_PROTO_LL] = comm->maxThreads[NCCL_ALGO_TREE][NCCL_PROTO_LL] =
    getNthreads("NCCL_NTHREADS", ncclParamNthreads(), 2*WARP_SIZE, NCCL_LL_MAX_NTHREADS, NCCL_LL_MAX_NTHREADS);
  comm->maxThreads[NCCL_ALGO_RING][NCCL_PROTO_LL128] = comm->maxThreads[NCCL_ALGO_TREE][NCCL_PROTO_LL128] =
    getNthreads("NCCL_LL128_NTHREADS", ncclParamLl128Nthreads(), NCCL_LL128_MAX_NTHREADS/4, NCCL_LL128_MAX_NTHREADS, NCCL_LL128_MAX_NTHREADS);

  int nNodes = comm->nNodes;
  int nRanks = comm->nRanks;
  if (nRanks <= 1) return ncclSuccess;

  int compCapIndex = minCompCap >= 90 ? HOPPER_COMPCAP_IDX : minCompCap >= 80 ? AMPERE_COMPCAP_IDX : VOLTA_COMPCAP_IDX;
  int index2 = nNodes <= 2 ? nNodes-1 : 2;
  // LL: for single node, we look at GPU type; for multi-node, we look at CPU type
  int index1 = nNodes == 1 ? compCapIndex :
               (comm->cpuVendor == NCCL_TOPO_CPU_VENDOR_AMD || comm->cpuVendor == NCCL_TOPO_CPU_VENDOR_MIXED) ? 1 : 0;
  double llMaxBw = llMaxBws[index1][index2];
  double perChMaxTreeBw = perChMaxTreeBws[compCapIndex][index2];
  double perChMaxRingLL128Bw = perChMaxRingLL128Bws[compCapIndex][index2];
  double perChMaxTreeLL128Bw = perChMaxTreeLL128Bws[compCapIndex][index2];
  // De-penalize Tree/Simple latency on Power systems to favor Tree than Ring
  if (comm->cpuArch == NCCL_TOPO_CPU_ARCH_POWER) hwLat[NCCL_HW_PCI][NCCL_ALGO_TREE][NCCL_PROTO_SIMPLE] = hwLat[NCCL_HW_PCI][NCCL_ALGO_RING][NCCL_PROTO_SIMPLE];
  float ppn = (float)nRanks / nNodes;

  int intraHw[NCCL_NUM_ALGORITHMS], hw[NCCL_NUM_ALGORITHMS];
  for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) intraHw[a] = graphs[a]->typeIntra == LINK_NVL ? NCCL_HW_NVLINK : NCCL_HW_PCI;
  for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) hw[a] = nNodes == 1 ? intraHw[a] : NCCL_HW_NET;

  for (int coll=0; coll<NCCL_NUM_FUNCTIONS; coll++) {
    int nsteps = coll == ncclFuncAllReduce ? 2*(nRanks-1) :
      coll == ncclFuncReduceScatter || coll == ncclFuncAllGather ? nRanks-1 :
      nRanks;

    for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
      if ((coll == ncclFuncBroadcast || coll == ncclFuncReduce) && a != NCCL_ALGO_RING) continue;
      if ((coll == ncclFuncReduceScatter || coll == ncclFuncAllGather)
          && a != NCCL_ALGO_PAT && a != NCCL_ALGO_RING
          && a != NCCL_ALGO_NVLS && a != NCCL_ALGO_COLLNET_DIRECT) continue;
      if (coll == ncclFuncAllReduce && a == NCCL_ALGO_PAT) continue;

      for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
        if ((a == NCCL_ALGO_NVLS || a == NCCL_ALGO_NVLS_TREE) && p != NCCL_PROTO_SIMPLE) continue;
        if ((coll == ncclFuncReduceScatter || coll == ncclFuncAllGather)
            && a == NCCL_ALGO_PAT && (p != NCCL_PROTO_SIMPLE || ncclPatEnable(comm) == 0)) continue;
        int collnet = (a == NCCL_ALGO_COLLNET_DIRECT || a == NCCL_ALGO_COLLNET_CHAIN) ? 1 : 0;
        float bw = nNodes <= 2 || collnet ? graphs[a]->bwIntra : graphs[a]->bwInter;
        if (a == NCCL_ALGO_NVLS) bw = std::min(graphs[a]->bwIntra, graphs[a]->bwInter);
        if (a == NCCL_ALGO_NVLS_TREE) bw = std::min(graphs[a]->bwIntra, nNodes <= 2 ? graphs[a]->bwInter : graphs[a]->bwInter/2);
        float busBw = graphs[a]->nChannels * bw;

        // Various model refinements
        if (a == NCCL_ALGO_RING && p == NCCL_PROTO_LL) { busBw = std::min(llMaxBw, busBw * .5); }
        if (a == NCCL_ALGO_RING && p == NCCL_PROTO_LL128) busBw = std::min(busBw * (0.92 /*120.0/128.0*/), graphs[a]->nChannels*perChMaxRingLL128Bw);
        if (a == NCCL_ALGO_TREE && coll == ncclFuncAllReduce) busBw = std::min(busBw*.92, graphs[a]->nChannels*perChMaxTreeBw);
        if (a == NCCL_ALGO_TREE && p == NCCL_PROTO_LL) busBw = std::min(busBw*1.0/3.8, llMaxBw);
        if (a == NCCL_ALGO_TREE && p == NCCL_PROTO_LL128) busBw = std::min(busBw * (nNodes == 1 ? 7.0/9.0 : 120.0/128.0), graphs[a]->nChannels*perChMaxTreeLL128Bw);
        if (a == NCCL_ALGO_TREE && graphs[a]->pattern == NCCL_TOPO_PATTERN_TREE) busBw *= .85;
        if (a == NCCL_ALGO_PAT) busBw *= .75;
        if (a == NCCL_ALGO_COLLNET_DIRECT && p != NCCL_PROTO_SIMPLE) busBw = 0;  // Not used
        if (a == NCCL_ALGO_COLLNET_CHAIN && p != NCCL_PROTO_SIMPLE) busBw = 0;  // Not used
        if (a == NCCL_ALGO_COLLNET_DIRECT && p == NCCL_PROTO_SIMPLE) {
          if (coll == ncclFuncAllGather || coll == ncclFuncReduceScatter) {
            busBw = ppn * bw;
            // AllGather/ReduceScatter requires 1:1 GPU:NIC
            int nicPerNode = comm->collNetHeadsNum;
            if (coll == ncclFuncAllGather && comm->nNodes > 1) {
              if (!comm->ncclCollNet || !comm->ncclCollNet->iallgather || ppn > nicPerNode) busBw = 0;
            }
            if (coll == ncclFuncReduceScatter && comm->nNodes > 1) {
              if (!comm->ncclCollNet || !comm->ncclCollNet->ireducescatter || ppn > nicPerNode) busBw = 0;
            }
            // Measured corrective ratio needed at 1 ppn and 8ppn. Here we hackishly
            // interpolate the two.
            float w = (ppn-1)/(8-1);
            busBw *= w*0.85 + (1-w)*0.95;
          } else {
            // Collnet+Direct requires all GPUs to have a local NIC to work at full speed
            float factor = ppn / (1.0*graphs[a]->nChannels); // GPU/NIC ratio
            factor -= (factor-1)/2;
            busBw /= factor;
            if (minCompCap >= 90) busBw *= .85;
          }
        }

        // Convert bus BW to algorithm BW
        if (!(a != NCCL_ALGO_RING && (coll == ncclFuncAllGather || coll == ncclFuncReduceScatter))) {
          float ratio = 1.0f;
          if (a == NCCL_ALGO_RING) ratio *= (1.0 * nRanks) / nsteps;
          else if (a == NCCL_ALGO_NVLS || a == NCCL_ALGO_NVLS_TREE) ratio *= 5.0/6.0;
          else ratio *= .5;
          busBw *= ratio;
        }
        comm->bandwidths[coll][a][p] = busBw;
        comm->latencies[coll][a][p] = baseLat[a][p];
        float intraLat = hwLat[intraHw[a]][a][p];
        // With ppn=1 latencies are fully exposed, use the Tree network latency
        float interLat = ppn == 1 ? hwLat[NCCL_HW_NET][NCCL_ALGO_TREE][p] : hwLat[NCCL_HW_NET][a][p];
        interLat += graphs[a]->latencyInter;
        // Also add the flush extra latency
        if (p == NCCL_PROTO_SIMPLE) interLat += graphs[a]->latencyInter;

        if (a == NCCL_ALGO_RING) {
          float lat = hwLat[hw[a]][a][p];
          if ((coll == ncclFuncReduce || coll == ncclFuncBroadcast)) {
            if (graphs[a]->sameChannels) {
              comm->latencies[coll][a][p] += lat;
            } else {
              if (p == NCCL_PROTO_SIMPLE) lat = hwLat[hw[a]][NCCL_ALGO_TREE][p]; // Add some chunk latency, waiting for proper chunk modeling
              comm->latencies[coll][a][p] += nsteps*lat;
            }
          } else {
            // Inter-node rings still have to launch nsteps * net overhead.
            float netOverhead = 0.0;
            if (nNodes > 1) {
              netOverhead = getNetOverhead(comm);
              if (p == NCCL_PROTO_SIMPLE) netOverhead *= 3;
            }
            intraLat = std::max(intraLat, netOverhead);
            int nInterSteps = nNodes == 1 ? 0 : coll == ncclFuncAllReduce ? 2*(nNodes-1) : nNodes-1;
            comm->latencies[coll][a][p] += (nsteps-nInterSteps)*intraLat + nInterSteps*interLat;
          }
        } else if (a == NCCL_ALGO_TREE) {
          if (coll == ncclFuncAllReduce) {
            comm->latencies[coll][a][p] +=
              2 * ((nRanks/nNodes-1) * intraLat + log2i(nNodes) * interLat);
          }
        } else if (a == NCCL_ALGO_COLLNET_DIRECT) {
          comm->latencies[coll][a][p] +=
            2 * (std::min(1, (nRanks/nNodes-1)) * intraLat + (nRanks/nNodes-1) * 0.4) + interLat;  // Add 0.4 us arity serialization latency
        } else if (a == NCCL_ALGO_COLLNET_CHAIN) {
          comm->latencies[coll][a][p] += 2 * (nRanks/nNodes-1) * intraLat + interLat;
        } else if (a == NCCL_ALGO_NVLS) {
          comm->latencies[coll][a][p] = intraLat;
          if (nNodes > 1) comm->latencies[coll][a][p] += interLat;
        } else if (a == NCCL_ALGO_NVLS_TREE) {
          comm->latencies[coll][a][p] += intraLat + 2 * log2i(nNodes) * interLat;
        } else if (a == NCCL_ALGO_PAT) {
          if (coll == ncclFuncAllGather || coll == ncclFuncReduceScatter) {
            comm->latencies[coll][a][p] = 8 // Base time
              + log2i(nNodes) * (interLat/3.5) // Log latency
              + nRanks * 2.8; // Still a linear part; hopefully we'll manage to remove it at some point.
          }
        }
      }
    }
  }

  // Protocols/Algorithms enable/disable, and user overrides.
  // All are enabled except ll128 which is enabled by default only in certain cases.
  int protoEnable[NCCL_NUM_FUNCTIONS*NCCL_NUM_PROTOCOLS];
  int algoEnable[NCCL_NUM_FUNCTIONS*NCCL_NUM_ALGORITHMS];
  for (int f=0; f<NCCL_NUM_FUNCTIONS; f++) {
    for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
      protoEnable[f*NCCL_NUM_PROTOCOLS+p] = p == NCCL_PROTO_LL128 ? 2 : 1;
    }
    for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
      algoEnable[f*NCCL_NUM_ALGORITHMS+a] = 1;
    }
  }

  const char *protoStr = ncclGetEnv("NCCL_PROTO");
  if (protoStr) {
    INFO(NCCL_ENV, "NCCL_PROTO set by environment to %s", protoStr);
    NCCLCHECK(parseList(protoStr, ncclFuncStr, NCCL_NUM_FUNCTIONS, ncclProtoStr, NCCL_NUM_PROTOCOLS, protoEnable));
  }
  const char *algoStr = ncclGetEnv("NCCL_ALGO");
  if (algoStr) {
    INFO(NCCL_ENV, "NCCL_ALGO set by environment to %s", algoStr);
    NCCLCHECK(parseList(algoStr, ncclFuncStr, NCCL_NUM_FUNCTIONS, ncclAlgoStr, NCCL_NUM_ALGORITHMS, algoEnable));
  }

  if (comm->rank == 0 && (algoStr||protoStr)) {
    constexpr int strLength = 1024;
    char funcAlgoProtoTuningStr[strLength];
    int offset = 0;
    offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "\n     Function | ");
    for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
      offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "%8s  ", ncclProtoStr[p]);
    }
    offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), " | ");
    for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
      offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "%13s  ", ncclAlgoStr[a]);
    }
    offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "\n");

    for (int f=0; f<NCCL_NUM_FUNCTIONS; f++) {
      offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "%13s | ", ncclFuncStr[f]);
      for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
        offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "%8d  ", protoEnable[f*NCCL_NUM_PROTOCOLS+p]);
      }
      offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), " | ");
      for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
        offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "%13d  ", algoEnable[f*NCCL_NUM_ALGORITHMS+a]);
      }
      offset += snprintf(funcAlgoProtoTuningStr+offset, std::max(0, strLength-offset), "\n");
    }

    INFO(NCCL_ENV, "Enabled NCCL Func/Proto/Algo Matrix:%s", funcAlgoProtoTuningStr);
  }

  int nvsCount = 0;
  NCCLCHECK(ncclTopoGetNvsCount(comm->topo, &nvsCount));

  for (int f=0; f<NCCL_NUM_FUNCTIONS; f++) {
    for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
      int disable = 0;
      // Disable NVLS Tree on a single node
      if (comm->nNodes == 1 && a == NCCL_ALGO_NVLS_TREE) disable = 1;
      // Disable Collnet+Direct, Collnet+Chain or Collnet+NVLS if collnet is not supported.
      if (comm->collNetSupport == 0 &&
          (a == NCCL_ALGO_COLLNET_DIRECT ||
           a == NCCL_ALGO_COLLNET_CHAIN ||
           (a == NCCL_ALGO_NVLS && comm->nNodes > 1))) disable = 1;
      // Disable CollNet+Direct if not on an NVSwitch system
      if (nvsCount == 0 && a == NCCL_ALGO_COLLNET_DIRECT) disable = 1;
      if (disable) algoEnable[f*NCCL_NUM_ALGORITHMS+a] = 0;
    }
  }

  for (int c=0; c<NCCL_NUM_FUNCTIONS; c++) for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
    int pEnable = protoEnable[c*NCCL_NUM_PROTOCOLS+p];
    if (pEnable == 2 && p == NCCL_PROTO_LL128) {
      // Enable LL128 by default only on Volta/Ampere/Hopper+NVLink. Other cases are not tested and may cause silent data corruption.
      pEnable = 1;
      pEnable &= (graphs[a]->typeInter <= PATH_PXB || (minCompCap >= 90 && graphs[a]->typeInter <= PATH_PXN));
      pEnable &= (graphs[a]->typeIntra <= PATH_NVB);
      pEnable &= (minCompCap == maxCompCap);
      switch (minCompCap) {
      case 70: pEnable &= 1; break;
      case 80: pEnable &= 1; break;
      case 90: pEnable &= !(CUDART_VERSION == 11080 && c == ncclFuncAllReduce && a == NCCL_ALGO_RING && comm->nRanks == 2); break;
      default: pEnable &= 0; break;
      }
    }
    if (pEnable == 0) comm->bandwidths[c][a][p] = 0;
    if (algoEnable[c*NCCL_NUM_ALGORITHMS+a] == 0) comm->bandwidths[c][a][p] = 0;
  }

  if (comm->rank == 0) {
    constexpr int lineLen = 1024;
    char line[lineLen];
    int offset = 0;
    for (int block=0; block<DIVUP(NCCL_NUM_ALGORITHMS, 3); block++) {
      offset = snprintf(line, lineLen, "  Algorithm   |");
      for (int ba=0; ba<3; ba++) {
        int a = block*3+ba;
        if (a >= NCCL_NUM_ALGORITHMS) continue;
        offset += snprintf(line+offset, std::max(0, lineLen-offset), " %14s   %14s   %14s |", "", ncclAlgoStr[a], "");
      }
      INFO(NCCL_TUNING, "%s", line);
      offset = snprintf(line, lineLen, "  Protocol    |");
      for (int ba=0; ba<3; ba++) {
        for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
          offset += snprintf(line+offset, std::max(0, lineLen-offset), " %14s |", ncclProtoStr[p]);
        }
      }
      INFO(NCCL_TUNING, "%s", line);
      offset = snprintf(line, lineLen, " Max NThreads |");
      for (int ba=0; ba<3; ba++) {
        int a = block*3+ba;
        if (a >= NCCL_NUM_ALGORITHMS) continue;
        for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
          offset += snprintf(line+offset, std::max(0, lineLen-offset), " %14d |", comm->maxThreads[a][p]);
        }
      }
      INFO(NCCL_TUNING, "%s", line);
      for (int c=0; c<NCCL_NUM_FUNCTIONS; c++) {
        offset = snprintf(line, lineLen, "%13s |", ncclFuncStr[c]);
        for (int ba=0; ba<3; ba++) {
          int a = block*3+ba;
          if (a >= NCCL_NUM_ALGORITHMS) continue;
          for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
            offset += snprintf(line+offset, std::max(0, lineLen-offset), "%8.1f/%6.1f |", comm->latencies[c][a][p], comm->bandwidths[c][a][p]);
          }
        }
        INFO(NCCL_TUNING, "%s", line);
      }
    }
  }
 
  // Set per-thread amount of work before we increase nThreads and nChannels
  for (int a=0; a<NCCL_NUM_ALGORITHMS; a++) {
    comm->threadThresholds[a][NCCL_PROTO_LL] = NCCL_LL_THREAD_THRESHOLD;
    comm->threadThresholds[a][NCCL_PROTO_LL128] = NCCL_LL128_THREAD_THRESHOLD;
    comm->threadThresholds[a][NCCL_PROTO_SIMPLE] = NCCL_SIMPLE_THREAD_THRESHOLD;
  }
  comm->threadThresholds[NCCL_ALGO_RING][NCCL_PROTO_LL] *= nRanks;
  comm->threadThresholds[NCCL_ALGO_COLLNET_DIRECT][NCCL_PROTO_SIMPLE] = 512;
  comm->threadThresholds[NCCL_ALGO_COLLNET_CHAIN][NCCL_PROTO_SIMPLE] = 512;

  // Override defaults with user env
  const char* str = ncclGetEnv("NCCL_THREAD_THRESHOLDS");
  if (str) {
    INFO(NCCL_ENV, "NCCL_THREAD_THRESHOLDS set by environment to %s", str);
    ssize_t t[2][NCCL_NUM_PROTOCOLS] = {{ -2, -2, -2 }, { -2, -2, -2 }};
    sscanf(str, "%ld %ld %ld %ld %ld %ld", t[0], t[0]+1, t[0]+2, t[1], t[1]+1, t[1]+2);
    for (int a=0; a<2; a++) {
      for (int p=0; p<NCCL_NUM_PROTOCOLS; p++) {
        if (t[a][p] >= 0) comm->threadThresholds[a][p] = t[a][p];
      }
    }
  }

  INFO(NCCL_INIT, "threadThresholds %ld/%ld/%ld | %ld/%ld/%ld | %ld | %ld",
      comm->threadThresholds[NCCL_ALGO_TREE][NCCL_PROTO_LL],
      comm->threadThresholds[NCCL_ALGO_TREE][NCCL_PROTO_LL128],
      comm->threadThresholds[NCCL_ALGO_TREE][NCCL_PROTO_SIMPLE],
      comm->threadThresholds[NCCL_ALGO_RING][NCCL_PROTO_LL],
      comm->threadThresholds[NCCL_ALGO_RING][NCCL_PROTO_LL128],
      comm->threadThresholds[NCCL_ALGO_RING][NCCL_PROTO_SIMPLE],
      comm->threadThresholds[NCCL_ALGO_COLLNET_DIRECT][NCCL_PROTO_SIMPLE],
      comm->threadThresholds[NCCL_ALGO_COLLNET_CHAIN][NCCL_PROTO_SIMPLE]);
  return ncclSuccess;
}
```

```cpp
// /home/e01592/work/nccl/src/include/device.h

// `ncclDevFuncIndex()` needs to be in sync with "all_functions()" in "src/device/generate.py"
inline int ncclDevFuncId(int coll, int devRedOp, int type, int algo, int proto) {
  #if defined(__CUDA_BF16_TYPES_EXIST__)
  constexpr int NumTypes = ncclNumTypes;
  #else
  constexpr int NumTypes = ncclNumTypes + 1;
  #endif
  int row;
  do {
    row = 0; // ncclDevFuncIndex_P2p
    if (coll == ncclFuncSendRecv) break;
    row += 1;

    int nAlgos = 4;
    if (coll == ncclFuncAllGather) {
      int algo1 = algo == NCCL_ALGO_RING ? 0 :
                  algo == NCCL_ALGO_COLLNET_DIRECT ? 1 :
                  algo == NCCL_ALGO_NVLS ? 2 :
                /*algo == NCCL_ALGO_PAT*/ 3;
      row += algo1*NCCL_NUM_PROTOCOLS + proto;
      break;
    }
    row += nAlgos*NCCL_NUM_PROTOCOLS;

    nAlgos = 1;
    if (coll == ncclFuncBroadcast) {
      row += proto;
      break;
    }
    row += nAlgos*NCCL_NUM_PROTOCOLS;

    nAlgos = 6;
    if (coll == ncclFuncAllReduce) {
      row += ((devRedOp*NumTypes + type)*nAlgos + algo)*NCCL_NUM_PROTOCOLS + proto;
      break;
    }
    row += ncclNumDevRedOps*NumTypes*nAlgos*NCCL_NUM_PROTOCOLS;

    nAlgos = 1;
    if (coll == ncclFuncReduce) {
      row += (devRedOp*NumTypes + type)*NCCL_NUM_PROTOCOLS + proto;
      break;
    }
    row += ncclNumDevRedOps*NumTypes*nAlgos*NCCL_NUM_PROTOCOLS;

    nAlgos = 4;
    if (coll == ncclFuncReduceScatter) {
      int algo1 = algo == NCCL_ALGO_RING ? 0 :
                  algo == NCCL_ALGO_COLLNET_DIRECT ? 1 :
                  algo == NCCL_ALGO_NVLS ? 2 :
                /*algo == NCCL_ALGO_PAT*/ 3;
      row += ((devRedOp*NumTypes + type)*nAlgos + algo1)*NCCL_NUM_PROTOCOLS + proto;
      break;
    }
    row += ncclNumDevRedOps*NumTypes*nAlgos*NCCL_NUM_PROTOCOLS;
  } while (false);

  return ncclDevFuncRowToId[row];
}
```

```cpp
// /home/e01592/work/nccl/src/misc/profiler.cc

static uint8_t ncclStringToAlgo(const char* algo) {
  if (0 == strcmp(algo, "TREE")) return NCCL_ALGO_TREE;
  if (0 == strcmp(algo, "RING")) return NCCL_ALGO_RING;
  if (0 == strcmp(algo, "COLLNET_DIRECT")) return NCCL_ALGO_COLLNET_DIRECT;
  if (0 == strcmp(algo, "COLLNET_CHAIN")) return NCCL_ALGO_COLLNET_CHAIN;
  if (0 == strcmp(algo, "NVLS")) return NCCL_ALGO_NVLS;
  if (0 == strcmp(algo, "NVLS_TREE")) return NCCL_ALGO_NVLS_TREE;
  return NCCL_ALGO_PAT;
}
```

```cpp

ncclResult_t ncclRegisterCollBuffers(
    struct ncclComm* comm, struct ncclTaskColl* info,
    void* outRegBufSend[NCCL_MAX_LOCAL_RANKS],
    void* outRegBufRecv[NCCL_MAX_LOCAL_RANKS],
    struct ncclIntruQueue<struct ncclCommCallback, &ncclCommCallback::next>* cleanupQueue,
    bool* regNeedConnect
  ) {
  ncclResult_t result = ncclSuccess;

  info->regBufType = NCCL_REGULAR_BUFFER;
  *regNeedConnect = true;
  if (!(ncclParamLocalRegister() || (comm->planner.persistent && ncclParamGraphRegister()))) goto exit;
#if CUDART_VERSION >= 11030
  if (info->algorithm == NCCL_ALGO_NVLS || info->algorithm == NCCL_ALGO_NVLS_TREE) {
    /* this part of nvls reg code is temporarily not used and obsolete. */
    if (!comm->nvlsRegSupport || info->opDev.op == ncclDevPreMulSum) goto exit;
    int nvlsReged = 0;
    int collnetReged = 0;
    const void *sendbuff = info->sendbuff;
    void *recvbuff = info->recvbuff;
    void *recvHandle = NULL, *sendHandle = NULL;
    if (info->func == ncclFuncAllGather) sendbuff = NULL;
    if (info->func == ncclFuncReduceScatter) recvbuff = NULL;
    size_t elementSize = ncclTypeSize(info->datatype);
    size_t sendbuffSize = elementSize*ncclFuncSendCount(info->func, comm->nRanks, info->count);
    size_t recvbuffSize = elementSize*ncclFuncRecvCount(info->func, comm->nRanks, info->count);

    /* first try local registration. */
    if (ncclParamLocalRegister()) {
      ncclNvlsLocalRegisterBuffer(comm, sendbuff, recvbuff, sendbuffSize, recvbuffSize, &nvlsReged, outRegBufSend, outRegBufRecv);
    }

    if (nvlsReged == 0 && comm->planner.persistent && ncclParamGraphRegister()) {
      ncclNvlsGraphRegisterBuffer(comm, sendbuff, recvbuff, sendbuffSize, recvbuffSize, &nvlsReged, outRegBufSend, outRegBufRecv, cleanupQueue, &info->nCleanupQueueElts);
    }

    if (comm->nNodes > 1 && info->algorithm == NCCL_ALGO_NVLS) {
      if (ncclParamLocalRegister()) {
        ncclCollnetLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetSend, &collnetReged, &sendHandle);
        if (collnetReged) ncclCollnetLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetRecv, &collnetReged, &recvHandle);
      }

      if (collnetReged == 0 && comm->planner.persistent && ncclParamGraphRegister()) {
        ncclCollnetGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetSend, &collnetReged, &sendHandle, cleanupQueue, &info->nCleanupQueueElts);
        if (collnetReged) ncclCollnetGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetRecv, &collnetReged, &recvHandle, cleanupQueue, &info->nCleanupQueueElts);
      }
    }

    if (nvlsReged) {
      *regNeedConnect = 0;
      /* tweak NVLS channels usage; for registered NVLS buffer, we only need 4/5 channels to
       * saturate bandwidth. */
      if (comm->nNodes == 1) {
        if (info->func == ncclFuncReduceScatter)
          info->nMaxChannels = std::max(comm->config.minCTAs, std::min(comm->config.maxCTAs, 5));
        else
          info->nMaxChannels = std::max(comm->config.minCTAs, std::min(comm->config.maxCTAs, 4));
      } else {
        info->nMaxChannels = std::max(comm->config.minCTAs, std::min(comm->config.maxCTAs, 6));
      }
      info->regBufType |= NCCL_NVLS_REG_BUFFER;
    }

    if (collnetReged) {
      info->regBufType |= NCCL_NET_REG_BUFFER;
      info->sendMhandle = sendHandle;
      info->recvMhandle = recvHandle;
    }
  } else if (info->protocol == NCCL_PROTO_SIMPLE) {
    // IPC buffer registration
    if (info->func == ncclFuncReduceScatter && info->algorithm != NCCL_ALGO_COLLNET_DIRECT) goto exit;
    if (info->algorithm == NCCL_ALGO_RING && ((info->func == ncclFuncAllReduce && info->sendbuff == info->recvbuff) || info->func == ncclFuncReduce)) goto exit;
    if ((info->algorithm == NCCL_ALGO_TREE || info->algorithm == NCCL_ALGO_COLLNET_CHAIN) && info->sendbuff == info->recvbuff) goto exit;
    if (info->func == ncclFuncAllGather && info->algorithm == NCCL_ALGO_PAT) goto exit;

    int peerRanks[NCCL_MAX_LOCAL_RANKS];
    int nPeers = 0;
    size_t elementSize = ncclTypeSize(info->datatype);
    size_t sendbuffSize = elementSize*ncclFuncSendCount(info->func, comm->nRanks, info->count);
    size_t recvbuffSize = elementSize*ncclFuncRecvCount(info->func, comm->nRanks, info->count);
    int regBufFlag = 0;
    memset(peerRanks, 0xff, sizeof(int) * NCCL_MAX_LOCAL_RANKS);

    if (info->algorithm == NCCL_ALGO_COLLNET_DIRECT) {
      struct ncclChannel* channel = comm->channels;
      int ipcRegFlag = 0, netSendRegFlag = 0, netRecvRegFlag = 0;
      void *sendHandle, *recvHandle;
      if (info->func != ncclFuncReduceScatter && comm->intraNodeP2pSupport) {
        for (int r = 0; r < NCCL_MAX_DIRECT_ARITY; ++r) {
          for (int down = 0; down < 2; ++down) {
            int peer = down ? channel->collnetDirect.down[r] : channel->collnetDirect.up[r];
            if (peer != -1) {
              struct ncclConnector* peerConn = &channel->peers[peer]->recv[0];
              bool needReg = false;

              NCCLCHECK(registerCheckP2PConnection(comm, peerConn, &comm->graphs[NCCL_ALGO_COLLNET_DIRECT], peer, &needReg));
              if (needReg) {
                bool found = false;
                for (int p = 0; p < nPeers; ++p) {
                  if (peerRanks[p] == peer) {
                    found = true;
                    break;
                  }
                }
                if (!found) peerRanks[nPeers++] = peer;
              }
            }
          }
        }

        if (nPeers > 0) {
          if (comm->planner.persistent && ncclParamGraphRegister()) {
            ncclIpcGraphRegisterBuffer(comm, info->sendbuff, sendbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &ipcRegFlag, &info->sendbuffOffset, &info->sendbuffRmtAddrs, cleanupQueue, &info->nCleanupQueueElts);
            if (ipcRegFlag) ncclIpcGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &ipcRegFlag, &info->recvbuffOffset, &info->recvbuffRmtAddrs, cleanupQueue, &info->nCleanupQueueElts);
          }
          if (!ipcRegFlag && ncclParamLocalRegister()) {
            ncclIpcLocalRegisterBuffer(comm, info->sendbuff, sendbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &ipcRegFlag, &info->sendbuffOffset, &info->sendbuffRmtAddrs);
            if (ipcRegFlag) ncclIpcLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &ipcRegFlag, &info->recvbuffOffset, &info->recvbuffRmtAddrs);
          }
        }
        if (ipcRegFlag) {
          info->regBufType |= NCCL_IPC_REG_BUFFER;
        }
      }

      // register collnet buffer
      if (info->opDev.op != ncclDevPreMulSum && info->opDev.op != ncclDevSumPostDiv && !(info->func == ncclFuncAllReduce && !comm->isOneRPN)) {
        if (comm->planner.persistent && ncclParamGraphRegister()) {
          ncclCollnetGraphRegisterBuffer(comm, info->sendbuff, sendbuffSize, collNetSend, &netSendRegFlag, &sendHandle, cleanupQueue, &info->nCleanupQueueElts);
          info->sendMhandle = sendHandle;
          if (netSendRegFlag) {
            ncclCollnetGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetRecv, &netRecvRegFlag, &recvHandle, cleanupQueue, &info->nCleanupQueueElts);
            info->recvMhandle = recvHandle;
          }
        }

        if ((netSendRegFlag == 0 || netRecvRegFlag == 0) && ncclParamLocalRegister()) {
          if (!netSendRegFlag) {
            ncclCollnetLocalRegisterBuffer(comm, info->sendbuff, sendbuffSize, collNetSend, &netSendRegFlag, &sendHandle);
            info->sendMhandle = sendHandle;
          }
          if (netSendRegFlag && !netRecvRegFlag) {
            ncclCollnetLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetRecv, &netRecvRegFlag, &recvHandle);
            info->recvMhandle = recvHandle;
          }
        }
      }

      if (netSendRegFlag && netRecvRegFlag) {
        if (comm->isOneRPN) info->nMaxChannels = 1;
        info->regBufType |= NCCL_NET_REG_BUFFER;
      }
    } else if (info->algorithm == NCCL_ALGO_RING) {
      struct ncclReg* recvRegRecord = NULL;
      struct ncclReg* sendRegRecord = NULL;
      int sendNetPeers = comm->nChannels;
      int recvNetPeers = comm->nChannels;
      struct ncclConnector** sendNetConns = NULL;
      struct ncclConnector** recvNetConns = NULL;
      void** sendNetHandles = NULL;
      void** recvNetHandles = NULL;
      void** srecvNetHandles = NULL;
      bool hasRecvNetPeer = false;
      bool hasSendNetPeer = false;

      NCCLCHECK(ncclRegFind(comm, info->recvbuff, recvbuffSize, &recvRegRecord));
      if (recvRegRecord == NULL && !(comm->planner.persistent && ncclParamGraphRegister())) goto exit;
      NCCLCHECK(ncclRegFind(comm, info->sendbuff, sendbuffSize, &sendRegRecord));
      if (sendRegRecord == NULL && !(comm->planner.persistent && ncclParamGraphRegister())) goto exit;
      NCCLCHECK(ncclCalloc(&sendNetConns, comm->nChannels));
      NCCLCHECK(ncclCalloc(&sendNetHandles, comm->nChannels));
      NCCLCHECK(ncclCalloc(&recvNetConns, comm->nChannels));
      NCCLCHECK(ncclCalloc(&recvNetHandles, comm->nChannels));
      NCCLCHECK(ncclCalloc(&srecvNetHandles, comm->nChannels));

      for (int c = 0; c < comm->nChannels; ++c) {
        struct ncclChannel* channel = comm->channels + c;
        for (int r = 0; r < 2; ++r) {
          int peer;
          struct ncclConnector* peerConn;
          if (r == 0) {
            peer = channel->ring.prev;
            peerConn = &channel->peers[peer]->recv[0];
            if (peerConn->conn.flags & NCCL_DIRECT_NIC) {
              recvNetConns[c] = peerConn;
              hasRecvNetPeer = true;
            }
          } else {
            peer = channel->ring.next;
            peerConn = &channel->peers[peer]->send[0];
            if (peerConn->conn.flags & NCCL_DIRECT_NIC) {
              sendNetConns[c] = peerConn;
              hasSendNetPeer = true;
            }
          }
          if (peerConn->conn.flags & (NCCL_P2P_READ | NCCL_P2P_WRITE)) {
            bool found = false;
            for (int p = 0; p < nPeers; ++p) {
              if (peerRanks[p] == peer) {
                found = true;
                break;
              }
            }
            if (!found) peerRanks[nPeers++] = peer;
          }
        }
      }
      if (nPeers > 0 && comm->intraNodeP2pSupport) {
        if (comm->planner.persistent && ncclParamGraphRegister()) {
          ncclIpcGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &regBufFlag, &info->recvbuffOffset, &info->recvbuffRmtAddrs, cleanupQueue, &info->nCleanupQueueElts);
        }
        if (!regBufFlag && ncclParamLocalRegister()) {
          ncclIpcLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &regBufFlag, &info->recvbuffOffset, &info->recvbuffRmtAddrs);
        }
      }
      if (regBufFlag) {
        info->regBufType = NCCL_IPC_REG_BUFFER;
      }

      // start net registration
      regBufFlag = 0;
      if (!comm->useNetPXN && comm->useGdr && comm->netDeviceType != NCCL_NET_DEVICE_UNPACK) {
        if (comm->planner.persistent && ncclParamGraphRegister()) {
          if (hasSendNetPeer) {
            ncclNetGraphRegisterBuffer(comm, info->sendbuff, sendbuffSize, sendNetConns, sendNetPeers, &regBufFlag, sendNetHandles, cleanupQueue, &info->nCleanupQueueElts);
            if (regBufFlag)
              ncclNetGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, sendNetConns, sendNetPeers, &regBufFlag, srecvNetHandles, cleanupQueue, &info->nCleanupQueueElts);
          }
          if ((regBufFlag || !hasSendNetPeer) && hasRecvNetPeer)
            ncclNetGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, recvNetConns, recvNetPeers, &regBufFlag, recvNetHandles, cleanupQueue, &info->nCleanupQueueElts);
        }
        if (!regBufFlag && ncclParamLocalRegister()) {
          if (hasSendNetPeer) {
            ncclNetLocalRegisterBuffer(comm, info->sendbuff, sendbuffSize, sendNetConns, sendNetPeers, &regBufFlag, sendNetHandles);
            if (regBufFlag)
              ncclNetLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, sendNetConns, sendNetPeers, &regBufFlag, srecvNetHandles);
          }
          if ((regBufFlag || !hasSendNetPeer) && hasRecvNetPeer)
            ncclNetLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, recvNetConns, recvNetPeers, &regBufFlag, recvNetHandles);
        }
      }

      if (regBufFlag) {
        info->regBufType |= NCCL_NET_REG_BUFFER;
        info->sendNetHandles = sendNetHandles;
        info->recvNetHandles = recvNetHandles;
        info->srecvNetHandles = srecvNetHandles;
        if (comm->isOneRPN && (info->func == ncclFuncAllGather || info->func == ncclFuncBroadcast)) {
          info->nMaxChannels = 1;
        }
      } else {
        free(sendNetHandles);
        free(recvNetHandles);
        free(srecvNetHandles);
      }

      free(sendNetConns);
      free(recvNetConns);
    } else if (info->algorithm == NCCL_ALGO_TREE || info->algorithm == NCCL_ALGO_COLLNET_CHAIN) {
      struct ncclReg* recvRegRecord;
      int netSendRegFlag = 0, netRecvRegFlag = 0;
      void *sendHandle, *recvHandle;
      NCCLCHECK(ncclRegFind(comm, info->recvbuff, recvbuffSize, &recvRegRecord));
      if (recvRegRecord == NULL && !(comm->planner.persistent && ncclParamGraphRegister())) goto exit;
      if (comm->intraNodeP2pSupport) {
        for (int c = 0; c < comm->nChannels; ++c) {
          struct ncclChannel* channel = comm->channels + c;
          struct ncclTree* tree = NULL;
          int peers[NCCL_MAX_TREE_ARITY + 1];

          if (info->algorithm == NCCL_ALGO_TREE)
            tree = &channel->tree;
          else
            tree = &channel->collnetChain;
          for (int p = 0; p < NCCL_MAX_TREE_ARITY; ++p) peers[p] = tree->down[p];
          peers[NCCL_MAX_TREE_ARITY] = tree->up;
          for (int p = 0; p < NCCL_MAX_TREE_ARITY + 1; ++p) {
            int peer = peers[p];
            bool peerNeedReg = false;
            struct ncclConnector* recvConn = NULL;
            // P2P transport
            if (peer == -1 || peer == comm->nRanks) continue;
            recvConn = &channel->peers[peer]->recv[0];
            NCCLCHECK(registerCheckP2PConnection(comm, recvConn, &comm->graphs[info->algorithm], peer, &peerNeedReg));

            if (peerNeedReg) {
              bool found = false;
              for (int pindex = 0; pindex < nPeers; ++pindex) {
                if (peerRanks[pindex] == peer) {
                  found = true;
                  break;
                }
              }
              if (!found) peerRanks[nPeers++] = peer;
            }
          }
        }
        if (nPeers > 0) {
          if (comm->planner.persistent && ncclParamGraphRegister()) {
            ncclIpcGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &regBufFlag, &info->recvbuffOffset, &info->recvbuffRmtAddrs, cleanupQueue, &info->nCleanupQueueElts);
          }
          if (!regBufFlag && ncclParamLocalRegister()) {
            ncclIpcLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, peerRanks, nPeers, NCCL_IPC_COLLECTIVE, &regBufFlag, &info->recvbuffOffset, &info->recvbuffRmtAddrs);
          }
        }
        if (regBufFlag) {
          info->regBufType = NCCL_IPC_REG_BUFFER;
        }
      }

      // register collnet chain 1RPN buffer
      if (info->algorithm == NCCL_ALGO_COLLNET_CHAIN && info->opDev.op != ncclDevPreMulSum && info->opDev.op != ncclDevSumPostDiv && comm->isOneRPN) {
        if (comm->planner.persistent && ncclParamGraphRegister()) {
          ncclCollnetGraphRegisterBuffer(comm, info->sendbuff, sendbuffSize, collNetSend, &netSendRegFlag, &sendHandle, cleanupQueue, &info->nCleanupQueueElts);
          info->sendMhandle = sendHandle;
          if (netSendRegFlag) {
            ncclCollnetGraphRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetRecv, &netRecvRegFlag, &recvHandle, cleanupQueue, &info->nCleanupQueueElts);
            info->recvMhandle = recvHandle;
          }
        }

        if ((netSendRegFlag == 0 || netRecvRegFlag == 0) && ncclParamLocalRegister()) {
          if (!netSendRegFlag) {
            ncclCollnetLocalRegisterBuffer(comm, info->sendbuff, sendbuffSize, collNetSend, &netSendRegFlag, &sendHandle);
            info->sendMhandle = sendHandle;
          }
          if (netSendRegFlag && !netRecvRegFlag) {
            ncclCollnetLocalRegisterBuffer(comm, info->recvbuff, recvbuffSize, collNetRecv, &netRecvRegFlag, &recvHandle);
            info->recvMhandle = recvHandle;
          }
        }
      }

      if (netSendRegFlag && netRecvRegFlag) {
        if (comm->isOneRPN) info->nMaxChannels = 1;
        info->regBufType |= NCCL_NET_REG_BUFFER;
      }
    }

    if (info->regBufType == NCCL_IPC_REG_BUFFER && comm->nNodes == 1 && 16 < info->nMaxChannels && info->nMaxChannels <= 24) {
      info->nMaxChannels = 16;
    }
  }
exit:
#endif
  return result;
}
```




# Search Result 详解

