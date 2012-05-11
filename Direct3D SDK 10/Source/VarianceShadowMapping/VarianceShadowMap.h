//----------------------------------------------------------------------------------
// File:   VarianceShadowMap.h
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

// This file contains utility code for creating a multisampled Variance Shadow Map.  The
// class creates two textures and the associated resource views.  One texuture is 
// multisampled, this is the one that is rendered to.  The multisampled texture is 
// resolved into the other texture which has a complete mipchain.

#pragma once

#define SM_FORMAT_TYPELESS DXGI_FORMAT_R32G32_TYPELESS
#define SM_FORMAT_TYPED DXGI_FORMAT_R32G32_FLOAT
#define SM_ZBUFFER_TYPELESS DXGI_FORMAT_R32_TYPELESS
#define SM_ZBUFFER_SRV_TYPED DXGI_FORMAT_R32_FLOAT
#define SM_ZBUFFER_DSV_TYPED DXGI_FORMAT_D32_FLOAT

#include "EffectWithOneTechnique.h"

class VarianceShadowMap : public EffectWithOneTechnique
{
public:
    VarianceShadowMap( UINT iWidth, UINT iHeight, ID3D10Device* pD3D10Device, ID3D10EffectPool* pPool ) : EffectWithOneTechnique( pD3D10Device,  L"GenerateVSM.fx" , pPool )
    {
        m_pD3D10Device = pD3D10Device;

        //NULL out resources
        m_pShadowMapZ = NULL;
        m_pShadowMapTarget = NULL;
        m_pDSV = NULL;
        m_pShadowMapSRV = NULL; 

        //Create our textures
        D3D10_TEXTURE2D_DESC Desc;
        Desc.ArraySize = 1;
        Desc.CPUAccessFlags = 0;
        Desc.Format = SM_FORMAT_TYPELESS;
        Desc.Width = iWidth;
        Desc.Height = iHeight;
        Desc.MipLevels = 0; //Full mip-chain
        Desc.MiscFlags = D3D10_RESOURCE_MISC_GENERATE_MIPS;
        Desc.SampleDesc.Count = 1;
        Desc.SampleDesc.Quality = 0;
        Desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
        Desc.Usage = D3D10_USAGE_DEFAULT;

        //Create our renderable 2-component texture array
        m_pD3D10Device->CreateTexture2D( &Desc, NULL, &m_pShadowMapTarget);

        _ASSERT( m_pShadowMapTarget );
        m_pShadowMapTarget->GetDesc( &Desc );
        iMipLevels = Desc.MipLevels;

        Desc.Width = iWidth;
        Desc.Height = iHeight;
        Desc.CPUAccessFlags = 0;
        Desc.Usage = D3D10_USAGE_DEFAULT;
        Desc.Format = SM_ZBUFFER_TYPELESS;
        Desc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
        Desc.MipLevels = 1;
		Desc.ArraySize = 1;
        Desc.MiscFlags = 0;

        //Create our MSAA depth buffer
        m_pD3D10Device->CreateTexture2D( &Desc, NULL, &m_pShadowMapZ );

        D3D10_DEPTH_STENCIL_VIEW_DESC DSVDesc;
        DSVDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
        DSVDesc.Format = SM_ZBUFFER_DSV_TYPED;
		DSVDesc.Texture2D.MipSlice = 0;

        //Create our depth stencil view
        m_pD3D10Device->CreateDepthStencilView( m_pShadowMapZ, &DSVDesc, &m_pDSV );

        D3D10_RENDER_TARGET_VIEW_DESC RTVDesc;
        RTVDesc.Format = SM_FORMAT_TYPED;
        RTVDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;

        //Create our render target view
		m_pD3D10Device->CreateRenderTargetView( m_pShadowMapTarget, &RTVDesc, &m_pRTV );

        D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;

        SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Format = SM_FORMAT_TYPED;
		SRVDesc.Texture2D.MipLevels = iMipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;

        //Create our shader resource view
        m_pD3D10Device->CreateShaderResourceView( m_pShadowMapTarget, &SRVDesc, &m_pShadowMapSRV );
		_ASSERT( m_pShadowMapSRV );

		SRVDesc.Format = SM_ZBUFFER_SRV_TYPED;
		SRVDesc.Texture2D.MipLevels = 1;
		m_pD3D10Device->CreateShaderResourceView( m_pShadowMapZ, &SRVDesc, &m_pDepthSRV );
		_ASSERT( m_pDepthSRV );

        ZeroMemory( &m_ViewPort, sizeof( D3D10_VIEWPORT ) );
        m_ViewPort.Height = iHeight;
        m_ViewPort.Width = iWidth;
        m_ViewPort.MaxDepth = 1.0f;
    };
    //We expose the base level of our mip chain as a RenderTargetView so that it can be blurred
    ID3D10RenderTargetView* pRTV( UINT i = 0 )
    {
        return m_pRTV;
    }
    //We expose the entire mip chain as a ShaderResourceView so that it can be sampled in the lighting shader
    ID3D10ShaderResourceView* pSRV()
    {
        return m_pShadowMapSRV;
    }
	//We expose the zbuffer for PCF shadow mapping
	ID3D10ShaderResourceView* pZBufferSRV()
	{
		return m_pDepthSRV;
	}

    //SetTargetsApplyEffect() should be called before rendering the shadow map as it sets the RenderTargetView and DepthStencilView
    void SetTargetsApplyEffect()
	{
        float Zmax = 1.e4f;
        float clrColor[4] = { Zmax, Zmax*Zmax, 1.0f, 1.0f };
		
		ApplyPass( 0 );
        m_pD3D10Device->ClearRenderTargetView( m_pRTV, clrColor );
        m_pD3D10Device->ClearDepthStencilView( m_pDSV, D3D10_CLEAR_DEPTH, 1.0f, 0 );
        m_pD3D10Device->OMSetRenderTargets( 1, &m_pRTV, m_pDSV );
        m_pD3D10Device->RSSetViewports( 1, &m_ViewPort );
       
    }

    //Resolve() resolves our MSAA target into the base mip level of our mipchain
    void Resolve()
    {
        //m_pD3D10Device->ResolveSubresource( m_pShadowMapBlurred, 0, m_pShadowMapTarget, 0, SM_FORMAT_TYPED );
    }
    //GenerateMips() uses the runtime to generate the rest of our mipchain from the base mip level
    void GenerateMips()
    {
        m_pD3D10Device->GenerateMips( m_pShadowMapSRV );
    }
    ~VarianceShadowMap()
    {
		SAFE_RELEASE( m_pRTV );
        
        SAFE_RELEASE( m_pShadowMapZ );
        SAFE_RELEASE( m_pShadowMapTarget );
    
        SAFE_RELEASE( m_pDSV );

        SAFE_RELEASE( m_pShadowMapSRV ); 

		SAFE_RELEASE( m_pDepthSRV );
    };
protected:
    
private:
    ID3D10Device* m_pD3D10Device;

    //RenderTargetViews
    ID3D10RenderTargetView* m_pRTV;

    //Resources
    ID3D10Texture2D* m_pShadowMapZ;
public:
    ID3D10Texture2D* m_pShadowMapTarget;

    //DepthStencilViews
    ID3D10DepthStencilView* m_pDSV;  //So we can render to the SM

    //ShaderResourceViews
    ID3D10ShaderResourceView* m_pShadowMapSRV; //To read back our original SM
	ID3D10ShaderResourceView* m_pDepthSRV; //To read the zbuffer when doing plain PCF

    //Viewport
    D3D10_VIEWPORT m_ViewPort;

    int iMipLevels;
};