```cpp
   __device__ __host__ PatAGAlgorithm(int stepSize, int stepDepth, size_t offset, size_t end, size_t count, int chunkCount, int rank, int nranks):
     offset(offset), end(end), count(count), chunkCount(chunkCount), rank(rank), nranks(nranks) {
    aggDelta = nrPow2 = (1<<log2Up(nranks));

    aggFactor = 1;
    size_t channelSize = end-offset;
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

`aggDelta = nrPow2 = (1<<log2Up(nranks));`

nranks | nrPow2 |
7 | 8 


phase 

1 --> 2
1 --> 0
2 --> 1





```cpp
  __device__ __host__ void getNextOp(int &recvDim, int &sendDim, size_t &inpIx, size_t &outIx, int &recvOffset, int &sendOffset, int &recvStepOffset, int &nelemOut, int &postRecv, int &postSend, int &last) {
restart:
    //printf("Phase %d as %d/%d a %d/%d scale %d\n", phase, as, aggDelta, a, lastA, scale);
    last = 0;
    nelemOut = nelem;
    inpIx = offset;
    int skip = 0;
    if (phase == 0) {
      int s = a*aggDelta + as;
      if (s >= nranks) skip = 1;
      int nextSkip = (a+1)*aggDelta + as >= nranks ? 1 : 0;
      int recvDataRank = (rank + s) % nranks;
      outIx = recvDataRank * count + offset;
      sendDim = -1;
      recvDim = 0;
      inpIx = 0;
      sendOffset = -1;
      recvOffset = (a % postFreq) * nelem;
      recvStepOffset = 0;
      postRecv = (a % postFreq == postFreq-1) || ((a+1)*aggDelta+as >= nranks) ? 1 : 0;
      postSend = 0;
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
      if (skip == 0) return;
   } else if (phase == 1) {
      int s = a*aggDelta + as;
      if (s >= nranks) skip = 1;
      sendDim = firstBitSet(s, nrPow2);
      s -= (1<<sendDim);
      int sendDataRank = (rank + nranks + s) % nranks;
      outIx = sendDataRank * count + offset;
      recvDim = s ? firstBitSet(s, nrPow2) : -1;
      sendOffset = recvOffset = (a % postFreq) * nelem;
      postSend = (a % postFreq == postFreq-1) || ((a+1)*aggDelta+as >= nranks) ? 1 : 0;
      postRecv = (sendDim == 0) && ((a % postFreq == postFreq-1) || ((a+1)*aggDelta+as-1 >= nranks)) ? 1 : 0;
      recvStepOffset = (sendDim == 0) ? 0 : a/postFreq;
      if (recvDim == -1) {
        recvOffset = -1;
        postRecv = 0;
      } else if (as - (1<<sendDim) == 0) {
        int foffset = (a*aggDelta) >> (recvDim+1);
        recvOffset = (foffset%postFreq)*nelem;
        postRecv = (sendDim == 0) && ((foffset % postFreq == postFreq-1) || ((((foffset+1)*2)+1)<<recvDim) >= nranks) ? 1 : 0;
        recvStepOffset = (sendDim == 0) ? 0 : foffset/postFreq;
      }
      if (s < nranks && sendDim == 0 && skip) {
        // Don't forget to receive at least once even if we don't send afterwards
        sendDim = -1;
        sendOffset = -1;
        postSend = 0;
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
      if (skip == 0) return;
    } else if (phase == 2) {
      int s = (2*a+1)*scale*aggDelta;
      postSend = (a % postFreq == postFreq-1) || ((2*(a+1)+1)*scale*aggDelta >= nranks) ? 1 : 0;
      postRecv = 0;
      if (s >= nranks) skip = 1;
      sendDim = firstBitSet(s, nrPow2);
      s -= (1<<sendDim);
      sendOffset = (a%postFreq) * nelem;
      recvStepOffset = a / postFreq;
      int sendDataRank = (rank + nranks + s) % nranks;
      outIx = sendDataRank * count + offset;
      recvDim = s ? firstBitSet(s, nrPow2) : -1;
      if (recvDim == -1) {
        recvOffset = -1;
      } else {
        s -= (1<<recvDim);
        int foffset = (a*2*scale*aggDelta) >> (recvDim+1);
        recvOffset = (foffset%postFreq)*nelem;
        recvStepOffset = foffset / postFreq;
      }
      if (++a == lastA) {
        scale /= 2;
        phase = scale ? 2 : 1;
        resetA();
      }
      if (skip == 0) return;
    }
    goto restart;
  }
```
