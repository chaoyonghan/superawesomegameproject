//----------------------------------------------------------------------------------
// File:   VarianceShadowMapping.cpp
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

// This is the main cpp file containing all setup code and the main loops.

#include "DXUT.h"
#include "SDKmesh.h"
#include <crtdbg.h>
#include "sdkmisc.h"
#include <NVUTmesh.h>
#include <DXUTGui.h>
#include "DXUTsettingsDlg.h"
#include "DXUTcamera.h"
#include "VarianceShadowMap.h"
#include "EffectWithOneTechnique.h"
#include "D3D10Quad.h"

static struct 
{
    //Pool pointer
    ID3D10EffectPool* pPool;

    //Handles to variables
    ID3D10EffectMatrixVariable* pWorldMatrix;
    ID3D10EffectMatrixVariable* pViewMatrix;
    ID3D10EffectMatrixVariable* pProjectionMatrix;
    ID3D10EffectMatrixVariable* pShadowView;
    ID3D10EffectMatrixVariable* pShadowProj;

	ID3D10EffectVectorVariable* pVecDiffuseColor;
	ID3D10EffectVectorVariable* pVectorLightPos;

    ID3D10EffectScalarVariable* pTextureWidth;
    ID3D10EffectScalarVariable* pBoolSpecular;
    ID3D10EffectScalarVariable* pBoolShowVariance;
    ID3D10EffectScalarVariable* pBoolbShowMD;
	ID3D10EffectScalarVariable* pBoolShowCheb;
	ID3D10EffectScalarVariable* pfFilterWidth;
	ID3D10EffectScalarVariable* pbDiffuseMap;
	ID3D10EffectScalarVariable* pbNormalMap;

	ID3D10EffectShaderResourceVariable* pShadowMap;
	ID3D10EffectShaderResourceVariable* ptexDiffuse;
	ID3D10EffectShaderResourceVariable* ptexNormal;
} g_EffectPool;

//Our shadowmap targets and effect
VarianceShadowMap*                g_pVarianceSM        = NULL;
ID3D10InputLayout*                g_pShadowIA          = NULL;

//Our primary effect for drawing the scene
EffectWithOneTechnique*           g_pSceneEffect       = NULL;
ID3D10InputLayout*                g_pSceneIA           = NULL;

//Mesh and matrix
NVUTMesh						  g_Mesh;
D3DXMATRIX                        g_mMesh;

//Quad for the floor
D3D10Quad*                        g_pFloorQuad        = NULL;
D3DXMATRIX                        g_mQuad;
ID3D10Texture2D*					g_pQuadDiffuseTexture = NULL;
ID3D10ShaderResourceView*			g_pQuadDiffuseSRV = NULL;

ID3D10Texture2D*					g_pQuadNormalTexture = NULL;
ID3D10ShaderResourceView*			g_pQuadNormalSRV = NULL;

bool								g_bVSM = true;
ID3D10InfoQueue* g_pInfoQueue = NULL;

static void sResetScene();

//--------------------------------------------------------------------------------------
// UI controls
//--------------------------------------------------------------------------------------
enum
{
    IDC_TOGGLEFULLSCREEN,
    IDC_TOGGLEREF,
    IDC_CHANGEDEVICE,
	IDC_LOADMODEL,
	IDC_RESET,
	IDC_TOGGLE_TEXTURE,
	IDC_TOGGLE_FLOOR,
	IDC_FILTER_SELECTOR_VSM,
	IDC_FILTER_SELECTOR_PCF,
    IDC_FILTER_SLIDER_STATIC,
    IDC_FILTER_SLIDER,
	IDC_LIGHT_DISTANCE_SLIDER_STATIC,
	IDC_LIGHT_DISTANCE_SLIDER,
    IDC_DEBUG_SHOW_VARIANCE,
    IDC_DEBUG_SHOW_MD,
	IDC_DEBUG_SHOW_CHEB,
};


CDXUTDialogResourceManager                g_DialogResourceManager;
CDXUTDialog                               g_HUD;
CD3DSettingsDlg                           g_D3DSettingsDlg;         // Device settings dialog

ID3DX10Font*                              g_pFont = NULL;         // Font for drawing text
ID3DX10Sprite*                            g_pSprite = NULL;       // Sprite for batching text drawing
CDXUTTextHelper*                          g_pTxtHelper = NULL;
bool                                      g_bShowHelp = false;

CModelViewerCamera                        g_Camera;
D3D10_VIEWPORT                            g_SceneVP;

class SeparableBlurEffectAndBuffer : public EffectWithOneTechnique
{
public:
	ID3D10EffectScalarVariable*             pBoolBlurVerticle;
	ID3D10EffectScalarVariable*				pFloatStepSize;

	SeparableBlurEffectAndBuffer(ID3D10Device* pD3D10Device, ID3D10EffectPool* pPool, ID3D10ShaderResourceView* pSRV, ID3D10RenderTargetView* pRTV ) : EffectWithOneTechnique( pD3D10Device, L"SeparableBlur.fx", pPool )
	{
		pBlurTexture = m_pEffect->GetVariableByName( "blurTexture" )->AsShaderResource();
		_ASSERT( pBlurTexture->IsValid() );
	
		pBoolBlurVerticle = m_pEffect->GetVariableByName( "bVertical" )->AsScalar();
		_ASSERT( pBoolBlurVerticle->IsValid() );

		pFloatStepSize = m_pEffect->GetVariableByName( "fStepSize" )->AsScalar();
		_ASSERT( pFloatStepSize->IsValid() );

		pSourceSRV = pSRV;

		D3D10_TEXTURE2D_DESC texDesc;
		ID3D10Texture2D* pRes = NULL;
		pSRV->GetResource( (ID3D10Resource**)&pRes );
		pRes->GetDesc( &texDesc );
		SAFE_RELEASE( pRes );

		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;

		pD3D10Device->CreateTexture2D( &texDesc, NULL, &pPingBuffer );
		_ASSERT( pPingBuffer );

		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		pSRV->GetDesc( &SRVDesc );  //To Grab the correct format
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		pD3D10Device->CreateShaderResourceView( pPingBuffer, &SRVDesc, &pPingBufferSRV );
		_ASSERT( pPingBufferSRV );

		D3D10_RENDER_TARGET_VIEW_DESC RTVDesc;
		pRTV->GetDesc( &RTVDesc ); //To Grab the correct format
		RTVDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;

		pD3D10Device->CreateRenderTargetView( pPingBuffer, &RTVDesc, &pPingBufferRTV );
		_ASSERT( pPingBuffer );
	}

	void BlurBuffer( ID3D10Device* pD3D10Device, ID3D10RenderTargetView* pRTV, float fStepSize )
	{
		ID3D10Buffer *pNullBuffer[] = { NULL };
		unsigned pStrides[] = { 0 };
		unsigned pOffsets[] = { 0 };
		pD3D10Device->IASetVertexBuffers(0, 1, pNullBuffer, pStrides, pOffsets);
		pD3D10Device->IASetInputLayout( NULL );
		pD3D10Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pFloatStepSize->SetFloat( fStepSize );

		//========================================
		// Annoying workaround for a DX runtime warning
		//========================================
		ID3D10RenderTargetView* pNullRTV = NULL;
		pD3D10Device->OMSetRenderTargets( 1, &pNullRTV, NULL );
		//========================================

		pBlurTexture->SetResource( pSourceSRV );
		pBoolBlurVerticle->SetBool( true );
		ApplyPass( 0 );

		pD3D10Device->OMSetRenderTargets( 1, &pPingBufferRTV, NULL );

		pD3D10Device->Draw( 4, 0 );

		//========================================
		// Annoying workaround for a DX runtime warning
		//========================================
		pD3D10Device->OMSetRenderTargets( 1, &pNullRTV, NULL );
		//========================================

		pBlurTexture->SetResource( pPingBufferSRV );
		pBoolBlurVerticle->SetBool( false );
		ApplyPass( 0 );

		pD3D10Device->OMSetRenderTargets( 1, &pRTV, NULL );

		pD3D10Device->Draw( 4, 0 );
	}
	~SeparableBlurEffectAndBuffer()
	{
		SAFE_RELEASE( pPingBuffer );
		SAFE_RELEASE( pPingBufferRTV );
		SAFE_RELEASE( pPingBufferSRV);
	}
protected:
	ID3D10EffectShaderResourceVariable* pBlurTexture;

private:
	ID3D10Texture2D* pPingBuffer;
	ID3D10ShaderResourceView* pPingBufferSRV;
	ID3D10RenderTargetView* pPingBufferRTV;

	ID3D10ShaderResourceView* pSourceSRV;
};

static SeparableBlurEffectAndBuffer* g_pBlur = NULL;

static void sSetShadowViewMatrix( D3DXVECTOR3& vLightPos )
{
	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vOrigin = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXMATRIX mLightView;

	g_EffectPool.pVectorLightPos->SetRawValue( (float*)&vLightPos, 0, sizeof( D3DXVECTOR3) );

	D3DXMatrixLookAtLH(&mLightView, &vLightPos, &vOrigin, &vUp );
	g_EffectPool.pShadowView->SetMatrix( (float*)&mLightView );
}

HRESULT loadModel(ID3D10Device* pd3dDevice, LPCWSTR pFileName)
{
	// Define the input layout for our model
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	g_Mesh.Destroy();
	g_Mesh.Create( pd3dDevice, pFileName, layout, 3 );
	float yScale = g_Mesh.m_vBox[1].y - g_Mesh.m_vBox[0].y;
	float fScale = 5.0f / yScale;

	//Setup our object matrix for our model
	D3DXMATRIX mTempA, mTempB;
	D3DXMatrixTranslation( &mTempA, 0.0f, -(g_Mesh.m_vBox[0].y + 0.000001f ), 0.0f );
	D3DXMatrixScaling( &mTempB, fScale, fScale, fScale );
	
	D3DXMatrixMultiply( &g_mMesh, &mTempA, &mTempB );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    WCHAR str[MAX_PATH];
    int iCurValue = 0;
	float fCurValue;
    BOOL bCurValue;
	bool bUpdateSlider = false;
	D3DXVECTOR3 vLightPos = D3DXVECTOR3( 0.5f, 1.0f, -1.0f );//D3DXVECTOR3( 0.5f, 1.0f, -1.0f );

    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:    DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:           DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:          g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() ); break;
		case IDC_RESET:  sResetScene(); break;
		case IDC_LOADMODEL:
			OPENFILENAME info;
			ZeroMemory(&info, sizeof (OPENFILENAME));
			str[0] = NULL;

			info.lStructSize = sizeof (OPENFILENAME);
			info.hwndOwner = DXUTGetHWND();
			info.lpstrFilter = L"X file meshes (.x)\0*.x\0\0";
			info.nFilterIndex = 0;
			info.lpstrFile = str;
			info.nMaxFile = MAX_PATH;
			info.lpstrTitle = L"Open model";
			info.nMaxFileTitle = sizeof(info.lpstrTitle);
			info.dwReserved = OFN_EXPLORER;

			if (GetOpenFileName(&info))
			{
				if (FAILED( loadModel(DXUTGetD3D10Device(), info.lpstrFile) ) )
				{
					MessageBox(NULL, L"Couldn't load the file", NULL, MB_ICONSTOP);
				}
			}
			break;
		case IDC_FILTER_SELECTOR_VSM: 
			g_bVSM = true; 
			bUpdateSlider = true; 
			g_EffectPool.pShadowMap->SetResource( g_pVarianceSM->pSRV() );
			break;
		case IDC_FILTER_SELECTOR_PCF: 
			g_bVSM = false; 
			bUpdateSlider = true;
			g_EffectPool.pShadowMap->SetResource( g_pVarianceSM->pZBufferSRV() );
			break;
        case IDC_FILTER_SLIDER:
            iCurValue = g_HUD.GetSlider( IDC_FILTER_SLIDER )->GetValue();
            if( !(iCurValue % 2 ) )
                g_HUD.GetSlider( IDC_FILTER_SLIDER )->SetValue( iCurValue + 1 );

			g_EffectPool.pfFilterWidth->SetFloat( (float)g_HUD.GetSlider( IDC_FILTER_SLIDER )->GetValue() );
			bUpdateSlider = true;
            break;
		case IDC_LIGHT_DISTANCE_SLIDER:
			fCurValue = (float)g_HUD.GetSlider( IDC_LIGHT_DISTANCE_SLIDER )->GetValue();
			vLightPos *= fCurValue;
			sSetShadowViewMatrix( vLightPos );

			StringCchPrintf( str, MAX_PATH, L"Light distance %.1f", fCurValue );
			g_HUD.GetStatic( IDC_LIGHT_DISTANCE_SLIDER_STATIC )->SetText( str );
			break;
        case IDC_DEBUG_SHOW_VARIANCE:
            g_EffectPool.pBoolShowVariance->GetBool( &bCurValue );
            g_EffectPool.pBoolShowVariance->SetBool( !bCurValue );
			g_HUD.GetCheckBox( IDC_DEBUG_SHOW_VARIANCE )->SetChecked( !bCurValue );
            break;
        case IDC_DEBUG_SHOW_MD:
            g_EffectPool.pBoolbShowMD->GetBool( &bCurValue );
            g_EffectPool.pBoolbShowMD->SetBool( !bCurValue );
			g_HUD.GetCheckBox( IDC_DEBUG_SHOW_MD )->SetChecked( !bCurValue );
            break;
		case IDC_DEBUG_SHOW_CHEB:
			g_EffectPool.pBoolShowCheb->GetBool( &bCurValue );
			g_EffectPool.pBoolShowCheb->SetBool( !bCurValue );
			g_HUD.GetCheckBox( IDC_DEBUG_SHOW_CHEB )->SetChecked( !bCurValue );
			break;
    }
	if( bUpdateSlider )
	{
		if( g_bVSM )
		{
			StringCchPrintf( str, MAX_PATH, L"%d x %d separable box filter", g_HUD.GetSlider( IDC_FILTER_SLIDER )->GetValue(), g_HUD.GetSlider( IDC_FILTER_SLIDER )->GetValue() );
		}
		else
		{
			StringCchPrintf( str, MAX_PATH, L"%d x %d box filter", g_HUD.GetSlider( IDC_FILTER_SLIDER )->GetValue(), g_HUD.GetSlider( IDC_FILTER_SLIDER )->GetValue() );
		}

		g_HUD.GetCheckBox( IDC_DEBUG_SHOW_VARIANCE )->SetEnabled( g_bVSM );
		g_HUD.GetCheckBox( IDC_DEBUG_SHOW_MD )->SetEnabled( g_bVSM );
		g_HUD.GetCheckBox( IDC_DEBUG_SHOW_CHEB )->SetEnabled( g_bVSM );

		g_HUD.GetStatic( IDC_FILTER_SLIDER_STATIC )->SetText( str );
	}
}

static void sResetScene()
{
	D3DXVECTOR3 mEyePos = D3DXVECTOR3( -5, 10, -10 );
	D3DXVECTOR3 mOrgin = D3DXVECTOR3( 0, 0, 0 );
	D3DXMATRIX id; D3DXMatrixIdentity( &id );
	//Setup our camera
	g_Camera.SetViewParams( &mEyePos, &mOrgin );
	g_Camera.SetWorldMatrix( id );

	//Position our quad to demonstrate the shadow
	D3DXMATRIX mTmpA, mTmpB;
	D3DXMatrixScaling( &mTmpA, 10.0f, 10.0f, 10.0f );
	D3DXMatrixRotationX( &mTmpB, -D3DX_PI / 2 );
	D3DXMatrixMultiply( &mTmpA, &mTmpA, &mTmpB );
	D3DXMatrixTranslation( &mTmpB, 0.0f, 0.0f, 2.0f );
	D3DXMatrixMultiply( &g_mQuad, &mTmpA, &mTmpB );

	g_HUD.GetSlider( IDC_FILTER_SLIDER )->SetValue( 9 );
	g_HUD.GetSlider( IDC_LIGHT_DISTANCE_SLIDER )->SetValue( 10 );

	//Call GUI events to correctly print results to the screen
	OnGUIEvent( 0, IDC_FILTER_SLIDER, NULL, NULL );
	OnGUIEvent( 0, IDC_LIGHT_DISTANCE_SLIDER, NULL, NULL );
	OnGUIEvent( 0, IDC_FILTER_SELECTOR_VSM, NULL, NULL );

	if( !g_HUD.GetCheckBox( IDC_TOGGLE_TEXTURE )->GetChecked() )
		g_HUD.GetCheckBox( IDC_TOGGLE_TEXTURE )->SetChecked( true );

	if( g_HUD.GetCheckBox( IDC_DEBUG_SHOW_CHEB )->GetChecked() )
		OnGUIEvent( 0, IDC_DEBUG_SHOW_CHEB, NULL, NULL );

	if( g_HUD.GetCheckBox( IDC_DEBUG_SHOW_MD )->GetChecked() )
		OnGUIEvent( 0, IDC_DEBUG_SHOW_MD, NULL, NULL );

	if( g_HUD.GetCheckBox( IDC_DEBUG_SHOW_VARIANCE )->GetChecked() )
		OnGUIEvent( 0, IDC_DEBUG_SHOW_VARIANCE, NULL, NULL );

}

//--------------------------------------------------------------------------------------
// Render the help and statistics text
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 2, 0 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats(true) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

    // Draw help
    if( g_bShowHelp )
    {
        UINT nBackBufferHeight = ( DXUTIsAppRenderingWithD3D9() ) ? DXUTGetD3D9BackBufferSurfaceDesc()->Height : DXUTGetDXGIBackBufferSurfaceDesc()->Height;
        g_pTxtHelper->SetInsertionPos( 2, nBackBufferHeight-15*6 );
        g_pTxtHelper->SetForegroundColor( D3DXCOLOR(1.0f, 0.75f, 0.0f, 1.0f ) );
        g_pTxtHelper->DrawTextLine( L"Controls:" );

        g_pTxtHelper->SetInsertionPos( 20, nBackBufferHeight-15*5 );
        g_pTxtHelper->DrawTextLine( L"Rotate model:  Left mouse button\n"
            L"Zoom camera:  Mouse wheel scroll\n"
            L"Rotate camera: Right mouse button");

        g_pTxtHelper->SetInsertionPos( 250, nBackBufferHeight-15*5 );
        g_pTxtHelper->DrawTextLine( L"Hide help: F1\n" 
            L"Quit: ESC\n" );
    }
    else
    {
        g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        g_pTxtHelper->DrawTextLine( L"Press F1 for help" );
    }

    g_pTxtHelper->End();
}

//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr = S_OK;

	//Turn off some spurios warning messages
	if( SUCCEEDED( pd3dDevice->QueryInterface( __uuidof(ID3D10InfoQueue), reinterpret_cast<void**> (&g_pInfoQueue) ) ) )
	{
		D3D10_INFO_QUEUE_FILTER filtList;
		ZeroMemory( &filtList, sizeof( D3D10_INFO_QUEUE_FILTER ) );
		D3D10_MESSAGE_ID idList[1] =  { D3D10_MESSAGE_ID_DEVICE_DRAW_RESOURCE_FORMAT_SAMPLE_C_UNSUPPORTED }; 
		filtList.DenyList.NumIDs = 1;
		filtList.DenyList.pIDList = idList;
		g_pInfoQueue->PushStorageFilter( &filtList );
	}


    g_DialogResourceManager.OnD3D10CreateDevice( pd3dDevice );
    g_D3DSettingsDlg.OnD3D10CreateDevice( pd3dDevice );

    V_RETURN( D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
        L"Arial", &g_pFont ) );
    V_RETURN( D3DX10CreateSprite( pd3dDevice, 512, &g_pSprite ) );

    g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont, g_pSprite, 15 );

    //Create our effect pool
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"VarianceShadowMapping.fxh" ) );
    D3DX10CreateEffectPoolFromFile( str, NULL, NULL, "fx_4_0", 0, 0, pd3dDevice, NULL, &g_EffectPool.pPool, NULL, &hr );
    g_EffectPool.pWorldMatrix = g_EffectPool.pPool->AsEffect()->GetVariableByName( "mWorld" )->AsMatrix();
    _ASSERT( g_EffectPool.pWorldMatrix->IsValid() );
    g_EffectPool.pViewMatrix = g_EffectPool.pPool->AsEffect()->GetVariableByName( "mView" )->AsMatrix();
    _ASSERT( g_EffectPool.pViewMatrix->IsValid() );
    g_EffectPool.pProjectionMatrix = g_EffectPool.pPool->AsEffect()->GetVariableByName( "mProj" )->AsMatrix();
    _ASSERT( g_EffectPool.pProjectionMatrix->IsValid() );
    g_EffectPool.pShadowView = g_EffectPool.pPool->AsEffect()->GetVariableByName( "mShadowView" )->AsMatrix();
    _ASSERT( g_EffectPool.pShadowView->IsValid() );
    g_EffectPool.pShadowProj = g_EffectPool.pPool->AsEffect()->GetVariableByName( "mShadowProj" )->AsMatrix();
    _ASSERT( g_EffectPool.pShadowProj->IsValid() );
    g_EffectPool.pTextureWidth = g_EffectPool.pPool->AsEffect()->GetVariableByName( "fTextureWidth" )->AsScalar();
    _ASSERT( g_EffectPool.pTextureWidth->IsValid() );
    g_EffectPool.pVecDiffuseColor = g_EffectPool.pPool->AsEffect()->GetVariableByName( "vDiffuseColor" )->AsVector();
    _ASSERT( g_EffectPool.pVecDiffuseColor->IsValid() );
    g_EffectPool.pBoolSpecular = g_EffectPool.pPool->AsEffect()->GetVariableByName( "bSpecular" )->AsScalar();
    _ASSERT( g_EffectPool.pBoolSpecular->IsValid() );
    g_EffectPool.pBoolShowVariance = g_EffectPool.pPool->AsEffect()->GetVariableByName( "bShowVariance" )->AsScalar();
    _ASSERT( g_EffectPool.pBoolShowVariance->IsValid() );
    g_EffectPool.pBoolbShowMD = g_EffectPool.pPool->AsEffect()->GetVariableByName( "bShowMD" )->AsScalar();
    _ASSERT( g_EffectPool.pBoolbShowMD->IsValid() );
	g_EffectPool.pBoolShowCheb = g_EffectPool.pPool->AsEffect()->GetVariableByName( "bShowCheb" )->AsScalar();
	_ASSERT( g_EffectPool.pBoolShowCheb->IsValid() );
	g_EffectPool.pfFilterWidth = g_EffectPool.pPool->AsEffect()->GetVariableByName( "fFilterWidth" )->AsScalar();
	_ASSERT( g_EffectPool.pfFilterWidth->IsValid() );
	g_EffectPool.pVectorLightPos = g_EffectPool.pPool->AsEffect()->GetVariableByName( "vLightPos" )->AsVector();
	_ASSERT( g_EffectPool.pVectorLightPos->IsValid() );
	g_EffectPool.ptexDiffuse = g_EffectPool.pPool->AsEffect()->GetVariableByName( "texDiffuse" )->AsShaderResource();
	_ASSERT( g_EffectPool.ptexDiffuse->IsValid() );
	g_EffectPool.ptexNormal = g_EffectPool.pPool->AsEffect()->GetVariableByName( "texNormal" )->AsShaderResource();
	_ASSERT( g_EffectPool.ptexNormal->IsValid() );
	g_EffectPool.pbNormalMap = g_EffectPool.pPool->AsEffect()->GetVariableByName( "bNormalMap" )->AsScalar();
	_ASSERT( g_EffectPool.pbNormalMap->IsValid() );
	g_EffectPool.pbDiffuseMap = g_EffectPool.pPool->AsEffect()->GetVariableByName( "bDiffuseMap" )->AsScalar();
	_ASSERT( g_EffectPool.pbDiffuseMap->IsValid() );

    // Define the input layout for our mesh
    const D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

    //Create our scene effect
    g_pSceneEffect = new EffectWithOneTechnique( pd3dDevice, L"DrawScene.fx", g_EffectPool.pPool );
    g_pSceneEffect->CreateIA( 0, layout, 3, &g_pSceneIA );

    //VarianceShadowMap contains the buffers needed to blur the VSM as well as the VSM shader that writes out depth and depth ^ 2
    const float fSMWidth = 1024.0f;
    g_pVarianceSM = new VarianceShadowMap( (UINT)fSMWidth, (UINT)fSMWidth, pd3dDevice, g_EffectPool.pPool );
    g_pVarianceSM->CreateIA( 0, layout, 3, &g_pShadowIA );
    g_EffectPool.pShadowMap = g_EffectPool.pPool->AsEffect()->GetVariableByName( "texShadowMap" )->AsShaderResource();
    g_EffectPool.pTextureWidth->SetFloat( fSMWidth ); //Set texture width

    g_pBlur = new SeparableBlurEffectAndBuffer( pd3dDevice, g_EffectPool.pPool, g_pVarianceSM->pSRV(), g_pVarianceSM->pRTV() );

    //Create our mesh
	
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"goof_knight.x" ) );
    loadModel( pd3dDevice, str );

	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"tiles_WhiteAndGrey_01_diffuse.dds" ) );
	V_RETURN( D3DX10CreateTextureFromFile( pd3dDevice, str, NULL, NULL, (ID3D10Resource**)&g_pQuadDiffuseTexture, &hr ) );
	V_RETURN( pd3dDevice->CreateShaderResourceView( g_pQuadDiffuseTexture, NULL, &g_pQuadDiffuseSRV ) );

	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"tiles_WhiteAndGrey_01_normal.dds" ) );
	V_RETURN( D3DX10CreateTextureFromFile( pd3dDevice, str, NULL, NULL, (ID3D10Resource**)&g_pQuadNormalTexture, &hr ) );
	V_RETURN( pd3dDevice->CreateShaderResourceView( g_pQuadNormalTexture, NULL, &g_pQuadNormalSRV ) );

    D3DXMATRIX mLightProj;
    
    D3DXMatrixPerspectiveFovLH( &mLightProj, D3DX_PI * 0.5f, 1.0f, .5f, 100.f );
    g_EffectPool.pShadowProj->SetMatrix( (float*)mLightProj );

    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.01f, 1000.0f );
    
    g_pFloorQuad = new D3D10Quad( pd3dDevice );

	sResetScene();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain *pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    //Set viewport
    ZeroMemory( &g_SceneVP, sizeof( D3D10_VIEWPORT ) );
    g_SceneVP.Height = pBackBufferSurfaceDesc->Height;
    g_SceneVP.Width = pBackBufferSurfaceDesc->Width;
    g_SceneVP.MaxDepth = 1.0f;


    g_DialogResourceManager.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc );
    g_D3DSettingsDlg.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc );
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 135, 0 );
    g_HUD.SetSize( 135, 150 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    g_Camera.FrameMove( fElapsedTime );
    // Clear render target and the depth stencil 
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    ID3D10RenderTargetView* pBB = DXUTGetD3D10RenderTargetView();
    ID3D10DepthStencilView* pZB = DXUTGetD3D10DepthStencilView();

    pd3dDevice->ClearRenderTargetView( pBB, ClearColor );
    pd3dDevice->ClearDepthStencilView( pZB, D3D10_CLEAR_DEPTH, 1.0, 0 );

    g_EffectPool.pViewMatrix->SetMatrix( (float*)g_Camera.GetViewMatrix() );
    g_EffectPool.pProjectionMatrix->SetMatrix( (float*)g_Camera.GetProjMatrix() );
    
	D3DXMATRIX mMeshWorld, mQuadWorld;
    D3DXMatrixMultiply( &mMeshWorld, &g_mMesh, g_Camera.GetWorldMatrix() );
	D3DXMatrixMultiply( &mQuadWorld, &g_mQuad, g_Camera.GetWorldMatrix() );

    

    //Render our model to the shadowmap
    g_pVarianceSM->SetTargetsApplyEffect();
    pd3dDevice->IASetInputLayout( g_pShadowIA );

	g_EffectPool.pWorldMatrix->SetMatrix( (float*)&mMeshWorld );
    g_Mesh.Render( pd3dDevice, g_pVarianceSM->pTechnique );

	g_EffectPool.pWorldMatrix->SetMatrix( (float*)&mQuadWorld );

	if( g_bVSM )
		g_pFloorQuad->Render( pd3dDevice, g_pVarianceSM->pTechnique );

    g_pVarianceSM->Resolve();

    //Do the separable blur if using VSM
	if( g_bVSM )
		g_pBlur->BlurBuffer( pd3dDevice, g_pVarianceSM->pRTV(), 1.0f );

    g_pVarianceSM->GenerateMips();
    
    //Render our scene
    pd3dDevice->RSSetViewports( 1, &g_SceneVP );
    pd3dDevice->OMSetRenderTargets( 1, &pBB, pZB );

    pd3dDevice->IASetInputLayout( g_pSceneIA );

    //Render our model
    static const float modelDiffuse[4] = {0.75f, 0.75f, 1.0f, 1.0f };
    g_EffectPool.pVecDiffuseColor->SetFloatVector( (float*)modelDiffuse );
    
	g_EffectPool.pBoolSpecular->SetBool( true );
	g_EffectPool.pbDiffuseMap->SetBool( false );
	g_EffectPool.pbNormalMap->SetBool( false );

	g_EffectPool.pWorldMatrix->SetMatrix( (float*)&mMeshWorld );

    g_Mesh.Render( pd3dDevice, g_pSceneEffect->GetTechnique( g_bVSM ) );

    //Render the floor
    static const float floorDiffuse[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
    g_EffectPool.pVecDiffuseColor->SetFloatVector( (float*)floorDiffuse );
   
	bool bTextureFloor = g_HUD.GetCheckBox( IDC_TOGGLE_TEXTURE )->GetChecked();
	g_EffectPool.pBoolSpecular->SetBool( false );
	g_EffectPool.pbDiffuseMap->SetBool( bTextureFloor );
	g_EffectPool.pbNormalMap->SetBool( bTextureFloor );
	
	g_EffectPool.ptexDiffuse->SetResource( g_pQuadDiffuseSRV );
	g_EffectPool.ptexNormal->SetResource( g_pQuadNormalSRV );

    g_EffectPool.pWorldMatrix->SetMatrix( (float*)&mQuadWorld );
	if( g_HUD.GetCheckBox( IDC_TOGGLE_FLOOR )->GetChecked() )
		g_pFloorQuad->Render( pd3dDevice, g_pSceneEffect->GetTechnique( g_bVSM ) );

    // If the settings dialog is being shown, then
    // render it instead of rendering the app's scene
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.OnRender( fElapsedTime );
        return;
    }

    g_HUD.OnRender( fElapsedTime );

    RenderText();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{
	SAFE_RELEASE( g_pInfoQueue );
    SAFE_DELETE( g_pSceneEffect );
    SAFE_DELETE( g_pVarianceSM );
    SAFE_DELETE( g_pFloorQuad );
    SAFE_DELETE( g_pBlur );

    SAFE_RELEASE( g_EffectPool.pPool );
    SAFE_RELEASE( g_pShadowIA );
    SAFE_RELEASE( g_pSceneIA );

    SAFE_RELEASE( g_pFont );
    SAFE_RELEASE( g_pSprite );

    SAFE_DELETE( g_pTxtHelper );

	SAFE_RELEASE( g_pQuadDiffuseTexture );
	SAFE_RELEASE( g_pQuadDiffuseSRV );

	SAFE_RELEASE( g_pQuadNormalTexture );
	SAFE_RELEASE( g_pQuadNormalSRV );

    g_Mesh.Destroy();
    g_DialogResourceManager.OnD3D10DestroyDevice();
    g_D3DSettingsDlg.OnD3D10DestroyDevice();
    
}




//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{

    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );

    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );

    if( *pbNoFurtherProcessing )
        return 0;

    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
        case VK_F1: g_bShowHelp = !g_bShowHelp;
            break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
                       int xPos, int yPos, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available on the system depending on which D3D callbacks are set below
    g_HUD.Init( &g_DialogResourceManager );
    g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.SetCallback( OnGUIEvent );

    int iY = 10;
    g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 125, 22 );
	g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += 24, 125, 22, VK_F3 );
    g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += 24, 125, 22, VK_F2 );
	g_HUD.AddButton(IDC_LOADMODEL, L"Load Model (F4)", 0, iY += 24, 125, 22, VK_F4 );
	g_HUD.AddButton(IDC_RESET, L"Reset", 0, iY += 24, 125, 22 );

	g_HUD.AddRadioButton( IDC_FILTER_SELECTOR_VSM, 0, L"VSM", -50, iY += 24, 60, 22, true );
	g_HUD.AddRadioButton( IDC_FILTER_SELECTOR_PCF, 0, L"PCF", 30, iY , 60, 22 );
    
	g_HUD.AddStatic(IDC_FILTER_SLIDER_STATIC, L"", -70, iY += 24, 200, 22 );
    g_HUD.AddSlider(IDC_FILTER_SLIDER, -50, iY += 24, 150, 22, 1, 50, 15 );

	g_HUD.AddStatic(IDC_LIGHT_DISTANCE_SLIDER_STATIC, L"", -70, iY += 24, 200, 22 );
	g_HUD.AddSlider(IDC_LIGHT_DISTANCE_SLIDER, -50, iY += 24, 150, 22, 10, 50, 10 ); //Clamp to 10 so we don't clip the ground

    g_HUD.AddCheckBox( IDC_DEBUG_SHOW_VARIANCE, L"Show variance (RED)", -50, iY += 24, 175, 22 );
    g_HUD.AddCheckBox( IDC_DEBUG_SHOW_MD, L"Show mD_2 (GREEN)", -50, iY += 24, 175, 22 );
	g_HUD.AddCheckBox( IDC_DEBUG_SHOW_CHEB, L"Show Chebyshev’s equation\n(BLUE )", -50, iY += 24, 175, 22 );

	g_HUD.AddCheckBox( IDC_TOGGLE_FLOOR, L"Draw floor", -50, iY += 24, 175, 22, true );
	g_HUD.AddCheckBox( IDC_TOGGLE_TEXTURE, L"Texture floor", -50, iY += 24, 175, 22, true );

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMouse( OnMouse );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );

   // Set the D3D10 DXUT callbacks. Remove these sets if the app doesn't need to support D3D10
    DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
    DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
    DXUTSetCallbackD3D10SwapChainResized( OnD3D10ResizedSwapChain );
    DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );
    DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
    DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );

    // IMPORTANT: set SDK media search path to include source directory of this sample, when started from .\Bin
    HRESULT hr;
    V_RETURN(DXUTSetMediaSearchPath(L"..\\Source\\VarianceShadowMapping"));
    
    // Perform any application-level initialization here
    
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"VarianceShadowMapping" );
    DXUTCreateDevice( true, 1024, 768 );  
    DXUTMainLoop(); // Enter into the DXUT render loop

    // Perform any application-level cleanup here

    return DXUTGetExitCode();
}


