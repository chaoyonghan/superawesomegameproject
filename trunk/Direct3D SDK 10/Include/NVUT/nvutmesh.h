//----------------------------------------------------------------------------------
// File:   NVUTMesh.h
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
#ifndef NVUTMESH_H
#define NVUTMESH_H

#include <sdkmesh_old.h>

class NVUTMesh : public CDXUTMesh10
{
public:
    NVUTMesh();

    // method first loads .x file using standard CDXUTMesh10::Create() method, then
    // computes bounding box for the mesh and stores it into m_vBox
    HRESULT Create(ID3D10Device *pDev10, LPCTSTR szFileName, D3D10_INPUT_ELEMENT_DESC* playout, UINT cElements, bool bOptimize=true);


    // Methods to load and render with normal maps, functions of based mesh are hidden for objects of NVUTMesh class.
    HRESULT CreateWithNormalMaps(ID3D10Device *pDev10, LPCTSTR szFileName, D3D10_INPUT_ELEMENT_DESC* playout, UINT cElements, bool bOptimize=true, const char*szNormalSuffix="_nm");
    void Render( ID3D10Device *pDev );
    void Render( ID3D10Device *pDev, 
                 ID3D10EffectTechnique *pTechnique, 
                 ID3D10EffectShaderResourceVariable* ptxDiffuse = NULL,
                 ID3D10EffectVectorVariable* pvDiffuse = NULL, 
                 ID3D10EffectVectorVariable* pvSpecular = NULL, 
                 DWORD dwSubset = (DWORD)-1,
                 ID3D10EffectShaderResourceVariable* ptxNormal = NULL);
    void RenderInstanced( ID3D10Device *pDev, 
                          ID3D10EffectTechnique *pTechnique, 
                          UINT uiInstanceCount,
                          ID3D10EffectShaderResourceVariable* ptxDiffuse = NULL,
                          ID3D10EffectVectorVariable* pvDiffuse = NULL, 
                          ID3D10EffectVectorVariable* pvSpecular = NULL );
    void Destroy();


    D3DXVECTOR3 m_vBox[2]; ///< bounding box for the mesh

    // Vars to to hold loaded normal maps if used.
    ID3D10Texture2D **m_ppNormalTextures;
    ID3D10ShaderResourceView **m_ppNormalSRVs;
    
};

#endif // NVUTMESH_H
