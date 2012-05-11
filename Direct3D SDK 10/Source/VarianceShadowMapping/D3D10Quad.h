//----------------------------------------------------------------------------------
// File:   D3D10Quad.h
// Author: Kevin Myers
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

// This file contains class that creates and manages an index and vertex buffer for an
// quad tesselated as specified by iTessLevel

#pragma once

#include <d3d10.h>

const D3D10_INPUT_ELEMENT_DESC quadLayout[] =
{
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD0", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};


class D3D10Quad
{
protected:
    ID3D10Buffer* m_pVB;
    ID3D10Buffer* m_pIB;
    UINT m_iIndexCount;
    UINT m_VertCount;

public:
    D3D10Quad( ID3D10Device* pD3D10Device, UINT iTessLevel = 0);

    void Render(ID3D10Device* pD3D10Device, ID3D10EffectTechnique* pTechnique );

    const UINT GetVertCount()
    {
        return m_VertCount;
    }
    ~D3D10Quad()
    {
        SAFE_RELEASE( m_pVB );
        SAFE_RELEASE( m_pIB );
    }
};
