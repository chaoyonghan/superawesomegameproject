#pragma once
//----------------------------------------------------------------------------------
// File:   NVUTSyncConstantBuffer.h
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

/*
    Simple template that creates and manages a data linked buffer.  Be careful NOT to use any class with a virtual function table, or appropriately padd your shader side constant buffer...

    This is a templated class, so usage:    NVUTSynConstantBuffer<MyDataStruct> syncConstantBuffer;
*/
template<class DataStruct>
class NVUTSyncConstantBuffer : public DataStruct
{
public:

    NVUTSyncConstantBuffer()
    {
        m_pBuffer = NULL;
    }

    ~NVUTSyncConstantBuffer()
    {
    }

    /*
        Create the buffer of the size of the base class (should be a POD i.e. struct, etc)
    */
    HRESULT Initialize(ID3D10Device* pd3dDevice)
    {
        HRESULT hr;
        UINT bufferSize = sizeof(DataStruct);

        D3D10_BUFFER_DESC cbdesc;
        cbdesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
        cbdesc.ByteWidth = bufferSize;
        cbdesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
        cbdesc.MiscFlags = 0;
        cbdesc.Usage = D3D10_USAGE_DYNAMIC;
        V(pd3dDevice->CreateBuffer(&cbdesc,NULL,&m_pBuffer));

        return hr;
    }

    /*
        Copy in the base class data elements.  This pretty much assumes that there isn't a VFT, but it will copy if there is...
    */
    HRESULT UpdateBuffer()
    {
        HRESULT hr;
        void *pCBData = NULL;
        V(m_pBuffer->Map(D3D10_MAP_WRITE_DISCARD,0,&pCBData));

        // Base class POD comes first, so just copy that in
        memcpy(pCBData,(void*)this,sizeof(DataStruct));

        m_pBuffer->Unmap();

        return hr;
    }

    void AssignToVSSlot(ID3D10Device* pd3dDevice,UINT slot)
    {
        pd3dDevice->VSSetConstantBuffers(slot,1,&m_pBuffer);
    }

    void AssignToGSSlot(ID3D10Device* pd3dDevice,UINT slot)
    {
        pd3dDevice->GSSetConstantBuffers(slot,1,&m_pBuffer);
    }

    void AssignToPSSlot(ID3D10Device* pd3dDevice,UINT slot)
    {
        pd3dDevice->PSSetConstantBuffers(slot,1,&m_pBuffer);
    }

    void Release()
    {
        SAFE_RELEASE(m_pBuffer);
    }


    ID3D10Buffer *GetBuffer()
    {
        return m_pBuffer;
    }

protected:
    ID3D10Buffer *m_pBuffer;
};
