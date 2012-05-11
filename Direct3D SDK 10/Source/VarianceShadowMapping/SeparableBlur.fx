//----------------------------------------------------------------------------------
// File:   SeparableBlur.fx
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

#include "VarianceShadowMapping.fxh"

bool bVertical = false;
float fStepSize = 1.0f;
Texture2D blurTexture;

float4 PSFilter( VSQuadOut input ) : SV_Target
{
    //PreShader - This should all be optimized away by the compiler
    //====================================
    float fStartOffset = BoxFilterStart( fFilterWidth );
    float2 fTexelOffset = float2( bVertical * ( fStepSize / fTextureWidth ), !bVertical * ( fStepSize / fTextureWidth ) );
    //====================================
    
    float2 fTexStart = input.tex - ( fStartOffset * fTexelOffset );
    float4 output = (float4)0.0f;
    
    for( int i = 0; i < fFilterWidth; ++i )
        output += blurTexture.Sample( FILT_LINEAR, float2( fTexStart + fTexelOffset * i) );
    
    return output / fFilterWidth;
}


technique10
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSFullscreenQuad() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSFilter() ) );
    
        SetRasterizerState( noCullRS );
        SetBlendState( RGBABS, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xffffffff );
        SetDepthStencilState( zWriteNoStencilDS, 0xffffffff );
    }
}