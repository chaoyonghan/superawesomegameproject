//----------------------------------------------------------------------------------
// File:   DrawScene.fx
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

#define SPECULAR_COLOR float4( 0.0f, 0.2f, 0.4f, 1.0f )

struct VSIn
{
    float3 pos : SV_Position;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
};
struct PSIn
{
    float4 pos : SV_Position;
    float2 tex : TEXCOORD0;
    float4 lightViewPos : lightviewpos;
    float3 wNorm : wNorm;
    float3 wLight : wLight;
    float fDepth : fDepth;
};

PSIn VSDrawScene( VSIn input )
{
    PSIn output = (PSIn)0.0f;
    
    output.tex = input.tex;
    
    float4 worldPos = mul( float4( input.pos, 1.0f ), mWorld );
    float4 lightViewPos = mul( worldPos, mShadowView );
    float3 lightVec = vLightPos.xyz - worldPos.xyz;

    output.pos = mul( mul( worldPos, mView ), mProj );
    output.lightViewPos = mul( lightViewPos, mShadowProj );
    output.wNorm = normalize( mul( float4( input.norm, 1.0f ), mWorld ) );
    output.wLight = normalize( lightVec );
    
    output.fDepth = length( vLightPos.xyz - worldPos.xyz );

    return output;
}

float4 VSM_DEBUG( float2 tex, float fragDepth )
{
    float2 moments = texShadowMap.Sample( FILT_LINEAR, tex );
    
    float E_x2 = moments.y;
    float Ex_2 = moments.x * moments.x;
    float variance = E_x2 - Ex_2;    
    float mD = (moments.x - fragDepth );
    float mD_2 = mD * mD;
    float p = variance / (variance + mD_2 );
    
	return float4( bShowVariance ? variance : 0.0f, bShowMD ? mD_2 : 0.0f, bShowCheb ? p : 0.0f, 1.0f );
}


float PCF_FILTER( float2 tex, float fragDepth )
{
    //PreShader - This should all be optimized away by the compiler
    //====================================
    float fStartOffset = BoxFilterStart( fFilterWidth );
    float texOffset = 1.0f / fTextureWidth;
    //====================================
    
    fragDepth -= 0.0001f;
    tex -= fStartOffset * texOffset;
    
    float lit = 0.0f;
		for( int i = 0; i < fFilterWidth; ++i )
			for( int j = 0; j < fFilterWidth; ++j )
			{	
				lit += texShadowMap.SampleCmpLevelZero( FILT_PCF,    float2( tex.x + i * texOffset, tex.y + j * texOffset ), fragDepth );
			}
	return lit / ( fFilterWidth * fFilterWidth );
}

float VSM_FILTER( float2 tex, float fragDepth )
{
    float lit = (float)0.0f;
    float2 moments = texShadowMap.Sample( FILT_LINEAR,    float3( tex, 0.0f ) );
    
    float E_x2 = moments.y;
    float Ex_2 = moments.x * moments.x;
    float variance = E_x2 - Ex_2;    
    float mD = (moments.x - fragDepth );
    float mD_2 = mD * mD;
    float p = variance / (variance + mD_2 );
    lit = max( p, fragDepth <= moments.x );
    
    return lit;
}

float4 PSDrawScene( uniform bool bVSM, PSIn input ) : SV_Target
{
	float4 output = (float4)0.0f;

    input.lightViewPos.xy /= input.lightViewPos.w;
    
    float2 tex = input.lightViewPos.xy * float2( 0.5f, -0.5f ) + 0.5f;
       
	if( bShowVariance || bShowMD || bShowCheb )  //Debug code for visualizing VSM
	{
		output = VSM_DEBUG( tex, input.lightViewPos.z );
		return output;
	}
	
    float lit = 0.0f;
    if( bVSM )
		lit = VSM_FILTER( tex, input.fDepth );
    else
		lit = PCF_FILTER( tex, input.lightViewPos.z / input.lightViewPos.w );
		
	float3 wLight = normalize( input.wLight );
	float3 wNormal;
	
	float2 texScaled = input.tex * 5.0f;
	
	if( bNormalMap )
	{
		float3 objNormal;
		objNormal  = texNormal.SampleLevel( FILT_LINEAR_WRAP, texScaled, 0 ) * 2.0f - 1.0f;
		wNormal = normalize( mul( float4( objNormal, 1.0f ), mWorld ) );
	}
	else
		wNormal = normalize( input.wNorm );

	float3 diffuse = bDiffuseMap ? texDiffuse.Sample( FILT_LINEAR_WRAP, texScaled ) : 1.0f;
	diffuse *= dot( normalize( wNormal ), wLight );
	return  float4( diffuse, 1.0f ) * lit;
}

technique10
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSDrawScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSDrawScene(false) ) );
        
        SetRasterizerState( backCullMSAARS );
        SetBlendState( RGBABS, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xffffffff );
        SetDepthStencilState( zWriteNoStencilDS, 0xffffffff );
    }
}

technique10
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSDrawScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSDrawScene(true) ) );
        
        SetRasterizerState( backCullMSAARS );
        SetBlendState( RGBABS, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xffffffff );
        SetDepthStencilState( zWriteNoStencilDS, 0xffffffff );
    }
}
