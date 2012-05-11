
//----------------------------------------------------------------------------------
// File:   nvutPCH.h
// Email:  sdkfeedback@nvidia.com
// 
// Copyright 1993-2010 NVIDIA Corporation.  All rights reserved. 
// 
// Please refer to the NVIDIA end user license agreement (EULA) associated
// with this source code for terms and conditions that govern your use of 
// this software.  Any use, reproduction, disclosure, or distribution of 
// this software and related documentation outside the terms of the EULA 
// is strictly prohibited.
//
//----------------------------------------------------------------------------------

// Desc: Precompiled header to include tchar before DXUT.h

#pragma once
#ifndef NVUTPCH_H
#define NVUTPCH_H

#include <tchar.h>
#include "DXUT.h"

#ifndef SAFE_ACQUIRE
#define SAFE_ACQUIRE(dst, p)      { if(dst) { SAFE_RELEASE(dst); } if (p) { (p)->AddRef(); } dst = (p); }
#endif

#endif
