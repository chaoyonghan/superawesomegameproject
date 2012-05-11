//----------------------------------------------------------------------------------
// File:   NVUTSkybox.cpp
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

// Desc: Encapsulation of skybox geometry and textures

#include <DXUT.h>
#include <NVUTSkybox.h>

#define MAX_PATH_STR 512

// Input layout definitions
D3D10_INPUT_ELEMENT_DESC NVUTskyboxLayout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};

//-----------------------------------------------------------------------------
// Name: Skybox
// Desc: Constructor
//-----------------------------------------------------------------------------
NVUTSkybox::NVUTSkybox()
{
    m_EnvTex       = NULL;

    m_d3dDevice    = NULL;

    m_VertexLayout = NULL;
    m_VertexBuffer = NULL;

    m_Effect       = NULL;
    m_Technique    = NULL;
    m_EnvMap       = NULL;

    m_VarInvWVP    = NULL;
    m_VarEnvMap    = NULL;

}

//-----------------------------------------------------------------------------
// Name: Skybox
// Desc: Destructor
//-----------------------------------------------------------------------------
NVUTSkybox::~NVUTSkybox()
{

}

//-----------------------------------------------------------------------------
// Name: OnCreateDevice
// Desc:
//-----------------------------------------------------------------------------
HRESULT NVUTSkybox::OnCreateDevice( ID3D10Device *pd3dDevice )
{
    HRESULT hr = S_OK;

    m_d3dDevice = pd3dDevice;

    // Save original media search path
    WCHAR strMediaSearchPath[MAX_PATH_STR+1];
    StringCchCopy( strMediaSearchPath, MAX_PATH, DXUTGetMediaSearchPath() );

    // Create the effect
    WCHAR str[MAX_PATH_STR+1];
    V_RETURN( DXUTSetMediaSearchPath(L"..\\Common\\NVUT") );
    if ( FAILED( DXUTFindDXSDKMediaFileCch( str, MAX_PATH_STR, L"nvutskybox.fx" ) ) ) {
        V_RETURN( DXUTSetMediaSearchPath(L"..\\Source\\Common\\NVUT") );
        hr = DXUTFindDXSDKMediaFileCch( str, MAX_PATH_STR, L"nvutskybox.fx" );
    }

    hr |= D3DX10CreateEffectFromFile( str, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, m_d3dDevice, NULL, NULL, &m_Effect, NULL, NULL );
    if( FAILED( hr ) ) {
        MessageBox(NULL, L"Unable to create effect from NVUTSkybox.fx", L"ERROR", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
        return FALSE;
    }

    // Set the search path back to original
    V_RETURN( DXUTSetMediaSearchPath( strMediaSearchPath ) );

    // Obtain the technique
    m_Technique          = m_Effect->GetTechniqueByName( "RenderSkybox" );
    m_TechniqueWithColor = m_Effect->GetTechniqueByName( "RenderSkyboxWithFloorColor" );

    // Obtain pointers to the shader variales
    m_VarInvWVP          = m_Effect->GetVariableByName( "g_mInvWorldViewProjection" )->AsMatrix();
    m_VarEnvMap          = m_Effect->GetVariableByName( "g_EnvironmentTexture" )->AsShaderResource();
    m_VarFloorColor      = m_Effect->GetVariableByName( "g_FloorColor" )->AsVector();
    m_VarSkyboxIntensity = m_Effect->GetVariableByName( "g_SkyboxIntensity" )->AsScalar();

    // Create the input layout
    D3D10_PASS_DESC PassDesc;

    m_Technique->GetPassByIndex( 0 )->GetDesc( &PassDesc );

    if( FAILED( m_d3dDevice->CreateInputLayout( NVUTskyboxLayout, 1, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_VertexLayout ) ) )
        return FALSE;

    // Create vertex buffer
    D3DXVECTOR2 vertices[4] = {
        D3DXVECTOR2( 1.0f,  1.0f ),
        D3DXVECTOR2( 1.0f, -1.0f ),
        D3DXVECTOR2(-1.0f,  1.0f ),
        D3DXVECTOR2(-1.0f, -1.0f )
    };

    D3D10_BUFFER_DESC      bd;
    D3D10_SUBRESOURCE_DATA InitData;

    bd.Usage          = D3D10_USAGE_IMMUTABLE;
    bd.ByteWidth      = sizeof( D3DXVECTOR2 ) * 4;
    bd.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags      = 0;
    InitData.pSysMem  = vertices;

    if( FAILED( m_d3dDevice->CreateBuffer( &bd, &InitData, &m_VertexBuffer ) ) )
        return hr;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc:
//-----------------------------------------------------------------------------
void NVUTSkybox::OnFrameRender( const D3DXMATRIX &mWVP )
{
    if( m_Effect == NULL )
        return;

    // Set the input layout
    m_d3dDevice->IASetInputLayout( m_VertexLayout );

    // Set vertex buffer
    UINT stride = sizeof( D3DXVECTOR2 );
    UINT offset = 0;
    m_d3dDevice->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );
    m_d3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    // Render the VB
    D3D10_TECHNIQUE_DESC techDesc;

    // Set effects variables
    D3DXMATRIX mInvWorldViewProj;
    D3DXMatrixInverse( &mInvWorldViewProj, NULL, &mWVP );

    m_VarInvWVP->SetMatrix( mInvWorldViewProj );
    m_VarEnvMap->SetResource( m_EnvMap );

    m_Technique->GetDesc( &techDesc );

    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        m_Technique->GetPassByIndex( p )->Apply( 0 );
        m_d3dDevice->Draw(4,  0);
    }
}

void NVUTSkybox::OnFrameRender( const D3DXMATRIX &mWVP, float SkyboxIntensity, const D3DXVECTOR4& FloorColor )
{
    if( m_Effect == NULL )
        return;

    // Set the input layout
    m_d3dDevice->IASetInputLayout( m_VertexLayout );

    // Set vertex buffer
    UINT stride = sizeof( D3DXVECTOR2 );
    UINT offset = 0;
    m_d3dDevice->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &stride, &offset );
    m_d3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    // Render the VB
    D3D10_TECHNIQUE_DESC techDesc;

    // Set effects variables
    D3DXMATRIX mInvWorldViewProj;
    D3DXMatrixInverse( &mInvWorldViewProj, NULL, &mWVP );

    m_VarInvWVP->SetMatrix(mInvWorldViewProj);
    m_VarEnvMap->SetResource( m_EnvMap );

    m_VarFloorColor->SetFloatVector( (float*)&FloorColor );
    m_VarSkyboxIntensity->SetFloat( SkyboxIntensity );


    m_TechniqueWithColor->GetDesc( &techDesc );

    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        m_TechniqueWithColor->GetPassByIndex( p )->Apply( 0 );
        m_d3dDevice->Draw(4,  0);
    }
}

//-----------------------------------------------------------------------------
// Name: OnCreateDevice
// Desc:
//-----------------------------------------------------------------------------
void NVUTSkybox::OnDestroyDevice()
{
    // Release all memory
    SAFE_RELEASE( m_VertexLayout );
    SAFE_RELEASE( m_VertexBuffer );
    SAFE_RELEASE( m_Effect );
    SAFE_RELEASE( m_EnvMap );
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void NVUTSkybox::OnReleasingSwapChain( )
{
    // doesn't really have to do anything here
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void NVUTSkybox::OnResizedSwapChain(ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC *pBackBufferSurfaceDesc)
{
    // doesn't really have to do anything here
}

void NVUTSkybox::SetTexture( ID3D10ShaderResourceView* ppEnvCubeRV )
{
    SAFE_RELEASE(m_EnvMap); // release ref is reseting this SRV
    m_EnvMap = ppEnvCubeRV;
    if(m_EnvMap) // maintain our own ref so caller has the option of immediately releasing.
        m_EnvMap->AddRef();
};