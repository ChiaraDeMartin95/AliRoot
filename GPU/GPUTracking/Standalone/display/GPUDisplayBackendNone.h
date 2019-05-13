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

/// \file GPUDisplayBackendNone.h
/// \author David Rohr

#ifndef GPUDISPLAYBACKENDNONE_H
#define GPUDISPLAYBACKENDNONE_H

#include "GPUDisplay.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
class GPUDisplayBackendNone : public GPUDisplayBackend
{
  GPUDisplayBackendNone() = default;
  ~GPUDisplayBackendNone() override = default;

  int StartDisplay() override { return 1; }
  void DisplayExit() override {}
  void SwitchFullscreen(bool set) override {}
  void ToggleMaximized(bool set) override {}
  void SetVSync(bool enable) override {}
  void OpenGLPrint(const char* s, float x, float y, float r, float g, float b, float a, bool fromBotton = true) override {}
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
