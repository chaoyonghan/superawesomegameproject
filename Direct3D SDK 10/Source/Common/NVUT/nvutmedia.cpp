//----------------------------------------------------------------------------------
// File:   NVUTMedia.cpp
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

#include <tchar.h>
#include <AtlBase.h>
#include <AtlConv.h>
#include <DXUT.h>
#include <SDKMisc.h>
#include "nvutmedia.h"


HRESULT WINAPI NVUTFindDXSDKMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename )
{
    HRESULT hr = DXUTFindDXSDKMediaFileCch( strDestPath, cchDest, strFilename );

    if (FAILED(hr))
    {
        WCHAR strFullError[MAX_PATH] = L"The file ";
        StringCchCat( strFullError, MAX_PATH, strFilename );
        StringCchCat( strFullError, MAX_PATH, L" was not found in the search path.\nCannot continue - exiting." );

        MessageBoxW(NULL, strFullError, L"Media file not found", MB_OK);
        exit(0);
    }

    return hr;
}

HRESULT WINAPI NVUTFindDXSDKMediaFileCchT( LPTSTR strDestPath, int cchDest, LPCTSTR strFilename )
{
    HRESULT hr(S_OK);
    USES_CONVERSION;

    CT2W wstrFileName(strFilename);
    LPWSTR wstrDestPath = new WCHAR[cchDest];
    
    hr = NVUTFindDXSDKMediaFileCch(wstrDestPath, cchDest, wstrFileName);
    
    if(!FAILED(hr))
    {
        LPTSTR tstrDestPath = W2T(wstrDestPath);
        _tcsncpy_s(strDestPath, cchDest, tstrDestPath, cchDest);
    }

    delete[] wstrDestPath;

    return hr;
}
