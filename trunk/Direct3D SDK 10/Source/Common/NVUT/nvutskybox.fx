//----------------------------------------------------------------------------------
// File:   NVUTSkybox.fx
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

// Parameters

float4x4 g_mInvWorldViewProjection;
TextureCube g_EnvironmentTexture;
float4 g_FloorColor;
float g_SkyboxIntensity = 1.0;

SamplerState BilinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

DepthStencilState DisableDepth
{
    DepthEnable = false;
    DepthWriteMask = 0;
};

RasterizerState DisableCulling
{
    CullMode = None; //Back
};

DepthStencilState EnableDepth
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less_Equal;
};

BlendState DisableBlend
{
    BlendEnable[0] = false;
};

//-----------------------------------------------------------------------------
// Skybox stuff
//-----------------------------------------------------------------------------
struct SkyboxVS_Input
{
    float2 Pos : POSITION;
};

struct SkyboxVS_Output
{
    float4 Pos : SV_POSITION;
    float3 Tex : TEXCOORD0;
};

//
// SHADERS
SkyboxVS_Output SkyboxVS( SkyboxVS_Input Input )
{
    SkyboxVS_Output Output;

    Output.Pos = float4( Input.Pos.xy, 1.0, 1.0 );
    Output.Tex = normalize( mul( float4( Input.Pos.xy, 1.0, 1.0 ), g_mInvWorldViewProjection ) );

    return Output;
}

float4 SkyboxPS( SkyboxVS_Output Input ) : SV_Target
{
    float4 Color = g_EnvironmentTexture.Sample( BilinearSampler, Input.Tex );

    return Color;
}

float4 SkyboxWithFloorColorPS(SkyboxVS_Output Input) : SV_Target
{
    float t =  saturate( -10.0 * Input.Tex.y );

    float4 Color = lerp( g_SkyboxIntensity * g_EnvironmentTexture.Sample( BilinearSampler, Input.Tex ), g_FloorColor, t );

    return Color;
}


//
// techniques
//
technique10 RenderSkybox
{
    pass p0
    {
        SetBlendState( DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );

        SetVertexShader( CompileShader( vs_4_0, SkyboxVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, SkyboxPS() ) );
    }
}

technique10 RenderSkyboxWithFloorColor
{
    pass p0
    {
        SetBlendState( DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );

        SetVertexShader( CompileShader( vs_4_0, SkyboxVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, SkyboxWithFloorColorPS() ) );
    }
}

