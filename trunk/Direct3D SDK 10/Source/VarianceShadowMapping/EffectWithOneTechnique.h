//----------------------------------------------------------------------------------
// File:   EffectWithOneTechnique.h
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

// This file contains a simple wrapper class that helps with effect creation.  It simply
// takes in the name D3D10 device, the name of the file, and the pool the effect will be
// added to.  It assumes there is only 1 technque per file, and also assumes all 
// all variables will be set through the pool since it doesn't expose the underlying 
// effect.

#pragma once

#include <crtdbg.h>
#include <d3d10.h>

class EffectWithOneTechnique
{
public:
    ID3D10EffectTechnique*    pTechnique;

    EffectWithOneTechnique( ID3D10Device* pD3D10Device, LPCWSTR pSrcFile , ID3D10EffectPool* pPool = NULL )
    {
        m_pEffect = NULL;
        pTechnique = NULL;
        m_pD3D10Device = pD3D10Device;

        //Create our effect
        WCHAR str[MAX_PATH];
		HRESULT hr;
        V( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, pSrcFile ) );
        D3DX10CreateEffectFromFile( str, NULL, NULL, "fx_4_0", 0, pPool ? D3D10_EFFECT_COMPILE_CHILD_EFFECT : NULL, pD3D10Device, pPool, NULL, &m_pEffect, NULL, &hr );
        _ASSERT( m_pEffect && m_pEffect->IsValid() );

        pTechnique = m_pEffect->GetTechniqueByIndex( 0 );
        _ASSERT( pTechnique->IsValid() );

        D3D10_TECHNIQUE_DESC techDesc;
        pTechnique->GetDesc( &techDesc );
        m_nPasses = techDesc.Passes;
        _ASSERT( m_nPasses < 100 && m_nPasses > 0 ); //Make sure this number isn't crazy

        //Cache pass descriptions
        m_pPassDescs = new D3D10_PASS_DESC[ m_nPasses ];
        for(UINT i = 0; i < m_nPasses; ++i )
            pTechnique->GetPassByIndex( i )->GetDesc( &m_pPassDescs[i] ); 
    }
    void CreateIA( UINT iPassIndex, const D3D10_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, ID3D10InputLayout **ppInputLayout )
    {
        _ASSERT( ppInputLayout );
        m_pD3D10Device->CreateInputLayout( pInputElementDescs, NumElements, m_pPassDescs[iPassIndex].pIAInputSignature, m_pPassDescs[iPassIndex].IAInputSignatureSize, ppInputLayout );
        _ASSERT( *ppInputLayout );
    }
    ID3D10EffectTechnique* GetTechnique(UINT i)
    {
        return m_pEffect->GetTechniqueByIndex( i );
    }
    void ApplyPass( UINT i )
    {
        pTechnique->GetPassByIndex( i )->Apply( 0 );
    }
    ~EffectWithOneTechnique()
    {
        SAFE_RELEASE( m_pEffect );
        SAFE_DELETE( m_pPassDescs );
    }

protected:
    ID3D10Effect*            m_pEffect;
    D3D10_PASS_DESC*        m_pPassDescs;

    ID3D10Device*            m_pD3D10Device;
    UINT                    m_nPasses;
private:
};    
