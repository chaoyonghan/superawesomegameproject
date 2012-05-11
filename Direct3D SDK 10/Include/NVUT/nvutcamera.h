//----------------------------------------------------------------------------------
// File:   NVUTCamera.h
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
#ifndef NVUTCAMERA_H
#define NVUTCAMERA_H

#include <dxutcamera.h>

class NVUTCamera : public CModelViewerCamera
{
public:
    // automatically set near and far planes according to object AABB
    // note that function uses view matrix passed from the outside, not the one
    // stored inside the camera
    virtual void SetProjParams(FLOAT fFOV, FLOAT fAspect, D3DXMATRIX mView, D3DXVECTOR3 vBBox[2]);
    void SetProjParams(D3DXMATRIX mView, D3DXVECTOR3 vBBox[2]);
    void SetModelRot(const D3DXMATRIX &m)
    { m_mModelRot = m; }
};

#endif // NVUTCAMERA_H
