//----------------------------------------------------------------------------------
// File:   GenerateVSM.fx
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


struct VSIN
{
    float3 pos : SV_Position;
    float3 norm : NORMAL;
};

struct PSIN
{
    float4 pos : SV_Position;
    float fDepth : fDepth;
};

PSIN VSGenerateVSM( VSIN input )
{
    PSIN output = (PSIN)0.0f;
    
    float4 worldPos = mul( float4( input.pos, 1.0f ), mWorld );
    
    output.pos = mul( mul( worldPos, mShadowView ), mShadowProj );
    output.fDepth = length( vLightPos.xyz - worldPos.xyz );

    return output;
}

float4 PSGenerateVSM( PSIN input ) : SV_Target
{
    return float4( input.fDepth, input.fDepth * input.fDepth, 0.0f, 1.0f );
}

technique10
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSGenerateVSM() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSGenerateVSM() ) );
        
        SetRasterizerState( noCullRS );
        SetBlendState( RGBABS, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xffffffff );
        SetDepthStencilState( zWriteNoStencilDS, 0xffffffff );
    }
}