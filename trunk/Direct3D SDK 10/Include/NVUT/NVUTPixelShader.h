#pragma once
//----------------------------------------------------------------------------------
// File:   NVUTPixelShader.h
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

#include "NVUTShader.h"

class NVUTPixelShader : public NVUTShader
{

public:
    NVUTPixelShader();
    ~NVUTPixelShader();

    HRESULT CreateFromFile(ID3D10Device* pd3dDevice, LPCWSTR pSrcFile, CONST D3D10_SHADER_MACRO* pDefines,
        LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2);

    void Release();

    ID3D10PixelShader *GetPS(){return m_pPS;}


    ID3D10PixelShader *m_pPS;
};