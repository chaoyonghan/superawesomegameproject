#pragma once
//----------------------------------------------------------------------------------
// File:   NVUTVertexShader.h
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

/*
    This is a simple wrapper for compiling and loading a vertex shader for use in the NVIDIA DX10 SDK.  
    It's only purpose is to wrap up common operations to allow the main sample code to be more readable.
*/
class NVUTVertexShader : public NVUTShader
{
public:

    NVUTVertexShader();
    ~NVUTVertexShader();

    /*
        Load, Compile, and Create a Vertex Shader from an HLSL shader text file.
    */
    HRESULT CreateFromFile(ID3D10Device* pd3dDevice, LPCWSTR pSrcFile, CONST D3D10_SHADER_MACRO* pDefines,
        LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2);

    void Release();

    UINT GetVertexSize(){return m_VertexSize;}
    UINT GetVertexElementCount(){return m_NumIEDElements;}
    D3D10_INPUT_ELEMENT_DESC *GetInputElementLayout(){return m_pIEDLayout;}
    ID3D10InputLayout* GetInputLayout(){return m_pInputLayout;}
    ID3D10VertexShader *GetVS(){return m_pVS;};
    
public:
    ID3D10VertexShader *m_pVS;

    UINT m_NumIEDElements;
    D3D10_INPUT_ELEMENT_DESC *m_pIEDLayout;
    ID3D10InputLayout *m_pInputLayout;

    UINT m_VertexSize;
};