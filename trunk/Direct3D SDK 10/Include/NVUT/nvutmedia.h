//----------------------------------------------------------------------------------
// File:   NVUTMedia.h
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

#pragma once
#ifndef NVUTMEDIA_H
#define NVUTMEDIA_H

#include <windows.h>

HRESULT WINAPI NVUTFindDXSDKMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename );

HRESULT WINAPI NVUTFindDXSDKMediaFileCchT( LPTSTR strDestPath, int cchDest, LPCTSTR strFilename );

#endif // NVUTMEDIA_H
