#pragma once
//----------------------------------------------------------------------------------
// File:   NVUTShader.h
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
    Base class used by the NVUTVertexShader and NVUTPixelShader classes.  The common oepration is to compile an HLSL shader text file into bytecode.

    That is all this class does.
*/
class NVUTShader
{
protected:
    class CustomIncludeInterface : public ID3D10Include
    {
    public:
        CustomIncludeInterface(){}
        STDMETHOD(Open)(THIS_ D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
    //    HRESULT Open( D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
        {
            *ppData = NULL;
            return E_NOTIMPL;
        }

        STDMETHOD(Close)(THIS_ LPCVOID pData)
        //HRESULT Close( LPCVOID pData)
        {
            delete pData;
            return E_NOTIMPL;
        }

        LPVOID *pData;
    };


    NVUTShader()
    {
        m_pReflectedShader = NULL;
        m_pShaderByteCode = NULL;
        m_pCompileErrors = NULL;
    
    };
    virtual ~NVUTShader(){};

    virtual HRESULT CompileShader(ID3D10Device* pd3dDevice, LPCWSTR pSrcFile, CONST D3D10_SHADER_MACRO* pDefines, 
        LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2)
    {
        HRESULT hr;
        NVUTShader::CustomIncludeInterface includeInterface;
        V(D3DX10CompileFromFile(pSrcFile,pDefines,&includeInterface,pFunctionName,pProfile,Flags1,Flags2,NULL,&m_pShaderByteCode,&m_pCompileErrors,&hr));
        V(D3D10ReflectShader(m_pShaderByteCode->GetBufferPointer(),m_pShaderByteCode->GetBufferSize(),&m_pReflectedShader));
        return hr;
    }

public:


    virtual void Release()
    {
        SAFE_RELEASE(m_pReflectedShader);
        SAFE_RELEASE(m_pShaderByteCode);
        SAFE_RELEASE(m_pCompileErrors);
    }

    virtual UINT GetSlotForConstantBuffer(LPCSTR pName)
    {
        D3D10_SHADER_DESC sdesc;
        m_pReflectedShader->GetDesc(&sdesc);

        // Constants, find our common buffer slot
        for(int iCB=0;iCB<(int)sdesc.ConstantBuffers;iCB++)
        {
            ID3D10ShaderReflectionConstantBuffer *pRCB = m_pReflectedShader->GetConstantBufferByIndex(iCB);
            D3D10_SHADER_BUFFER_DESC sbdesc;
            pRCB->GetDesc(&sbdesc);
            if(strncmp(pName,sbdesc.Name,16)==0)
            {
                return iCB;
            }
        }
        return 0;
    }

    virtual UINT GetConstantBufferSize(UINT slot)
    {
        ID3D10ShaderReflectionConstantBuffer *pRCB = m_pReflectedShader->GetConstantBufferByIndex(slot);
        D3D10_SHADER_BUFFER_DESC sbdesc;
        pRCB->GetDesc(&sbdesc);
        return sbdesc.Size;
    }

    ID3D10ShaderReflection *m_pReflectedShader;
    ID3D10Blob *m_pShaderByteCode;
    ID3D10Blob *m_pCompileErrors;

};