//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUChainTracking.h
/// \author David Rohr

#ifndef GPUCHAINTRACKING_H
#define GPUCHAINTRACKING_H

#include "GPUChain.h"
#include "GPUReconstructionHelpers.h"
#include <atomic>
#include <array>
class AliHLTTPCClusterMCLabel;
struct AliHLTTPCRawCluster;

namespace o2
{
namespace trd
{
class TRDGeometryFlat;
}
} // namespace o2

namespace o2
{
namespace TPC
{
struct ClusterNativeAccessFullTPC;
struct ClusterNative;
} // namespace TPC
} // namespace o2

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUTPCSliceOutput;
class GPUTPCSliceOutTrack;
class GPUTPCSliceOutCluster;
class GPUTPCGMMergedTrack;
struct GPUTPCGMMergedTrackHit;
class GPUTRDTrackletWord;
class GPUTPCMCInfo;
class GPUTRDTracker;
class GPUTPCGPUTracker;
struct GPUTPCClusterData;
struct ClusterNativeAccessExt;
struct GPUTRDTrackletLabels;
class GPUDisplay;
class GPUQA;
class GPUTRDGeometry;
class TPCFastTransform;

class GPUChainTracking : public GPUChain, GPUReconstructionHelpers::helperDelegateBase
{
  friend class GPUReconstruction;

 public:
  ~GPUChainTracking() override;
  void RegisterPermanentMemoryAndProcessors() override;
  void RegisterGPUProcessors() override;
  int Init() override;
  int PrepareEvent() override;
  int Finalize() override;
  int RunStandalone() override;
  void MemorySize(size_t& gpuMem, size_t& pageLockedHostMem) override;

  // Structures for input and output data
  struct InOutPointers {
    InOutPointers() = default;
    InOutPointers(const InOutPointers&) = default;

    const GPUTPCClusterData* clusterData[NSLICES] = { nullptr };
    unsigned int nClusterData[NSLICES] = { 0 };
    const AliHLTTPCRawCluster* rawClusters[NSLICES] = { nullptr };
    unsigned int nRawClusters[NSLICES] = { 0 };
    const o2::TPC::ClusterNativeAccessFullTPC* clustersNative = nullptr;
    const GPUTPCSliceOutTrack* sliceOutTracks[NSLICES] = { nullptr };
    unsigned int nSliceOutTracks[NSLICES] = { 0 };
    const GPUTPCSliceOutCluster* sliceOutClusters[NSLICES] = { nullptr };
    unsigned int nSliceOutClusters[NSLICES] = { 0 };
    const AliHLTTPCClusterMCLabel* mcLabelsTPC = nullptr;
    unsigned int nMCLabelsTPC = 0;
    const GPUTPCMCInfo* mcInfosTPC = nullptr;
    unsigned int nMCInfosTPC = 0;
    const GPUTPCGMMergedTrack* mergedTracks = nullptr;
    unsigned int nMergedTracks = 0;
    const GPUTPCGMMergedTrackHit* mergedTrackHits = nullptr;
    unsigned int nMergedTrackHits = 0;
    const GPUTRDTrack* trdTracks = nullptr;
    unsigned int nTRDTracks = 0;
    const GPUTRDTrackletWord* trdTracklets = nullptr;
    unsigned int nTRDTracklets = 0;
    const GPUTRDTrackletLabels* trdTrackletsMC = nullptr;
    unsigned int nTRDTrackletsMC = 0;
    friend class GPUReconstruction;
  } mIOPtrs;

  struct InOutMemory {
    InOutMemory();
    ~InOutMemory();
    InOutMemory(InOutMemory&&);
    InOutMemory& operator=(InOutMemory&&);

    std::unique_ptr<GPUTPCClusterData[]> clusterData[NSLICES];
    std::unique_ptr<AliHLTTPCRawCluster[]> rawClusters[NSLICES];
    std::unique_ptr<o2::TPC::ClusterNative[]> clustersNative[NSLICES * GPUCA_ROW_COUNT];
    std::unique_ptr<GPUTPCSliceOutTrack[]> sliceOutTracks[NSLICES];
    std::unique_ptr<GPUTPCSliceOutCluster[]> sliceOutClusters[NSLICES];
    std::unique_ptr<AliHLTTPCClusterMCLabel[]> mcLabelsTPC;
    std::unique_ptr<GPUTPCMCInfo[]> mcInfosTPC;
    std::unique_ptr<GPUTPCGMMergedTrack[]> mergedTracks;
    std::unique_ptr<GPUTPCGMMergedTrackHit[]> mergedTrackHits;
    std::unique_ptr<GPUTRDTrack[]> trdTracks;
    std::unique_ptr<GPUTRDTrackletWord[]> trdTracklets;
    std::unique_ptr<GPUTRDTrackletLabels[]> trdTrackletsMC;
  } mIOMem;

  // Read / Dump / Clear Data
  void ClearIOPointers();
  void AllocateIOMemory();
  using GPUChain::DumpData;
  void DumpData(const char* filename);
  using GPUChain::ReadData;
  int ReadData(const char* filename);
  void DumpSettings(const char* dir = "") override;
  void ReadSettings(const char* dir = "") override;

  // Converter / loader functions
  int ConvertNativeToClusterData();
  void ConvertNativeToClusterDataLegacy();

  // Getters for external usage of tracker classes
  GPUTRDTracker* GetTRDTracker() { return &processors()->trdTracker; }
  GPUTPCTracker* GetTPCSliceTrackers() { return processors()->tpcTrackers; }
  const GPUTPCTracker* GetTPCSliceTrackers() const { return processors()->tpcTrackers; }
  const GPUTPCGMMerger& GetTPCMerger() const { return processors()->tpcMerger; }
  GPUTPCGMMerger& GetTPCMerger() { return processors()->tpcMerger; }
  GPUDisplay* GetEventDisplay() { return mEventDisplay.get(); }
  const GPUQA* GetQA() const { return mQA.get(); }
  GPUQA* GetQA() { return mQA.get(); }

  // Processing functions
  int RunTPCTrackingSlices();
  virtual int RunTPCTrackingMerger();
  virtual int RunTRDTracking();
  int DoTRDGPUTracking();

  // Getters / setters for parameters
  const TPCFastTransform* GetTPCTransform() const { return mTPCFastTransform.get(); }
  const GPUTRDGeometry* GetTRDGeometry() const { return (GPUTRDGeometry*)mTRDGeometry.get(); }
  const ClusterNativeAccessExt* GetClusterNativeAccessExt() const { return mClusterNativeAccess.get(); }
  void SetTPCFastTransform(std::unique_ptr<TPCFastTransform> tpcFastTransform);
  void SetTRDGeometry(const o2::trd::TRDGeometryFlat& geo);
  void LoadClusterErrors();

  const void* mConfigDisplay = nullptr; // Abstract pointer to Standalone Display Configuration Structure
  const void* mConfigQA = nullptr;      // Abstract pointer to Standalone QA Configuration Structure

 protected:
  struct GPUTrackingFlatObjects : public GPUProcessor {
    GPUChainTracking* mChainTracking = nullptr;
    TPCFastTransform* mTpcTransform = nullptr;
    char* mTpcTransformBuffer = nullptr;
    o2::trd::TRDGeometryFlat* mTrdGeometry = nullptr;
    void* SetPointersFlatObjects(void* mem);
    short mMemoryResFlat = -1;
  };

  struct eventStruct // Must consist only of void* ptr that will hold the GPU event ptrs!
  {
    void* selector[NSLICES];
    void* stream[GPUCA_MAX_STREAMS];
    void* init;
    void* constructor;
  };

  GPUChainTracking(GPUReconstruction* rec);

  int ReadEvent(int iSlice, int threadId);
  void WriteOutput(int iSlice, int threadId);
  int GlobalTracking(int iSlice, int threadId);

  int PrepareProfile();
  int DoProfile();

  // Pointers to tracker classes
  GPUTrackingFlatObjects mFlatObjectsShadow; // Host copy of flat objects that will be used on the GPU
  GPUTrackingFlatObjects mFlatObjectsDevice; // flat objects that will be used on the GPU

  // Display / QA
  std::unique_ptr<GPUDisplay> mEventDisplay;
  bool mDisplayRunning = false;
  std::unique_ptr<GPUQA> mQA;
  bool mQAInitialized = false;

  // Ptr to reconstruction detecto objects
  std::unique_ptr<ClusterNativeAccessExt> mClusterNativeAccess; // Internal memory for clusterNativeAccess
  std::unique_ptr<TPCFastTransform> mTPCFastTransform;          // Global TPC fast transformation object
  std::unique_ptr<o2::trd::TRDGeometryFlat> mTRDGeometry;       // TRD Geometry

  HighResTimer timerTPCtracking[NSLICES][10];
  eventStruct mEvents;
  std::ofstream mDebugFile;

#ifdef __ROOT__ // ROOT5 BUG: cint doesn't do volatile
#define volatile
#endif
  volatile int mSliceOutputReady = 0;
  volatile char mSliceLeftGlobalReady[NSLICES] = { 0 };
  volatile char mSliceRightGlobalReady[NSLICES] = { 0 };
#ifdef __ROOT__
#undef volatile
#endif
  std::array<char, NSLICES> mGlobalTrackingDone;
  std::array<char, NSLICES> mWriteOutputDone;

 private:
  int RunTPCTrackingSlices_internal();
  std::atomic_flag mLockAtomic = ATOMIC_FLAG_INIT;

  int HelperReadEvent(int iSlice, int threadId, GPUReconstructionHelpers::helperParam* par);
  int HelperOutput(int iSlice, int threadId, GPUReconstructionHelpers::helperParam* par);
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
