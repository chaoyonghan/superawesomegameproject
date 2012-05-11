//----------------------------------------------------------------------------------
// File:   NVUTPixelShader.cpp
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

#include "DXUT.h"
#include "NVUTPixelShader.h"

NVUTPixelShader::NVUTPixelShader(): NVUTShader()
{


    m_pPS = NULL;
}

NVUTPixelShader::~NVUTPixelShader()
{
}

HRESULT NVUTPixelShader::CreateFromFile(ID3D10Device* pd3dDevice, LPCWSTR pSrcFile, CONST D3D10_SHADER_MACRO* pDefines,
    LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2)
{
    HRESULT hr;
    // Inherited compile and reflect
    V(CompileShader(pd3dDevice,pSrcFile,pDefines,pFunctionName,pProfile,Flags1,Flags2));

    V(pd3dDevice->CreatePixelShader(m_pShaderByteCode->GetBufferPointer(),m_pShaderByteCode->GetBufferSize(),&m_pPS));
    return hr;
}

void NVUTPixelShader::Release()
{
    NVUTShader::Release();
    SAFE_RELEASE(m_pPS);
}
