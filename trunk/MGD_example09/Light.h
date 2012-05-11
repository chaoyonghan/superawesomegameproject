#pragma once

#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTgui.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "d3dx11effect.h"
#include "Terrain.h"
#include <vector>

#define DEG2RAD( a ) ( a * D3DX_PI / 180.f )

class Lights
{
	//friend Terrain;

	struct sPointLight{
		// Mesh
		D3DXVECTOR4 mColour; // Colore luce
		D3DXVECTOR4 mAmbientColour; // Colore ambientale
		D3DXVECTOR3 mAttenuation; // Coefficienti di attenuazione proporzionale alla distanza punto-luce (k0+k1*d+k2*d*d)
	};

	const float ROTATIONSPEED1;
	const float ROTATIONSPEED2;

	// Definiamo due luci puntuali
	sPointLight g_Light1;
	sPointLight g_Light2;
	CDXUTSDKMesh g_MeshSphere; // Mesh - per disegnare il punto della luce
	D3DXMATRIX  g_WorldLight1; // Posizione orientazione nel mondo luce 1
	D3DXMATRIX  g_WorldLight2; // Posizione orientazione nel mondo luce 2

	ID3D11InputLayout *g_pVertexLayout;

	ID3DX11Effect*         g_pEffect;
	ID3DX11EffectTechnique *g_pTechnique;

	ID3D11Device * pd3dDevice;
	ID3D11DeviceContext * iD3DImmediateContext;

	// Parametri rotazioe luci
	float g_CurrentLightRotation1;
	float g_CurrentLightRotation2;

	D3DXMATRIX translation;
	D3DXMATRIX scale;

public:

	Lights()
		:g_CurrentLightRotation2(0)
		,g_CurrentLightRotation1(0)
		,ROTATIONSPEED1(40.0f)
		,ROTATIONSPEED2(30.0f)
	{

	}

	HRESULT Init( ID3D11Device * iDevice)
	{
		pd3dDevice = iDevice;
		pd3dDevice->GetImmediateContext(&iD3DImmediateContext);
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
		// Il settaggio di questo flag permette di eseguire il debug diretto dello shader, senza
		// impattare in modo marcato sulle performance.
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
		HRESULT hr;	

		// creiamo la sfera relativa alla luce
		g_MeshSphere.Create( pd3dDevice, L"ball.sdkmesh" );

		// Proprietà luce 1
		g_Light1.mAmbientColour = D3DXVECTOR4(0.5f,0.5f,0.5f,1.0f);
		g_Light1.mColour = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
		g_Light1.mAttenuation = D3DXVECTOR3(1.0f,1.0f,1.0f);
		D3DXMatrixIdentity(&g_WorldLight1);
		D3DXMatrixScaling(&scale,0.5f,0.5f,0.5f);
		D3DXMatrixTranslation(&translation,0.0f,0.0f,0.0f);
		D3DXMatrixMultiply(&g_WorldLight1, &scale, &translation);


		// Proprietà luce 2
		g_Light2.mAmbientColour = D3DXVECTOR4(0.5f,0.5f,0.5f,1.0f);
		g_Light2.mColour = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
		g_Light2.mAttenuation = D3DXVECTOR3(1.0f,1.0f,1.0f);
		D3DXMatrixIdentity(&g_WorldLight2);
		D3DXMatrixMultiply(&g_WorldLight2, &scale, &translation);

		return S_OK;
	}

	void draw(ID3DX11Effect *iEffect,ID3DX11EffectTechnique *iTechnique, ID3D11InputLayout *iLayout,D3DXMATRIX& view, D3DXMATRIX& proj,D3DXVECTOR3& vieweyevec)
	{
		g_pEffect = iEffect;
		g_pTechnique = iTechnique;
		g_pVertexLayout = iLayout;
		g_pEffect->GetVariableByName("View")->AsMatrix()->SetMatrix( ( float* )&view );
		g_pEffect->GetVariableByName("Projection")->AsMatrix()->SetMatrix( ( float* )&proj );

		D3DX11_TECHNIQUE_DESC techDesc;
		D3DXMatrixTranslation(&g_WorldLight1,vieweyevec.x+2.0f,vieweyevec.y-5.0f,vieweyevec.z+10.0f);
		D3DXMatrixTranslation(&g_WorldLight2,vieweyevec.x-2.0f,vieweyevec.y-5.0f,vieweyevec.z+10.0f);
		iD3DImmediateContext->IASetInputLayout( g_pVertexLayout );
		g_pEffect->GetVariableByName("World")->AsMatrix()->SetMatrix(g_WorldLight1);
		g_pEffect->GetVariableByName("DefaultColor")->AsVector()->SetFloatVector( ( float* )g_Light1.mColour );
		g_pEffect->GetVariableByName("CameraPosition")->AsVector()->SetFloatVector( ( float* )vieweyevec );
		SetupLights();
		g_pTechnique->GetDesc( &techDesc );
		// Otteniamo il descrittore della tecnica ed eseguiamo i vari passaggi 
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			g_pTechnique->GetPassByIndex( p )->Apply(0,iD3DImmediateContext);
			g_MeshSphere.Render(iD3DImmediateContext);
		}

		iD3DImmediateContext->IASetInputLayout( g_pVertexLayout );
		g_pEffect->GetVariableByName("World")->AsMatrix()->SetMatrix(g_WorldLight2);
		g_pEffect->GetVariableByName("DefaultColor")->AsVector()->SetFloatVector( ( float* )g_Light2.mColour );
		g_pTechnique->GetDesc( &techDesc );
		// Otteniamo il descrittore della tecnica ed eseguiamo i vari passaggi 
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			g_pTechnique->GetPassByIndex( p )->Apply(0,iD3DImmediateContext);
			g_MeshSphere.Render(iD3DImmediateContext);
		}
	}

	void OnFrameMove(float fElapsedTime)
	{
		D3DXMATRIX rotation;

		// Animiamo le luci
		g_CurrentLightRotation1 = fmod(g_CurrentLightRotation1 + ROTATIONSPEED1 * fElapsedTime ,360.f);
		g_CurrentLightRotation2 = fmod(g_CurrentLightRotation2 + ROTATIONSPEED2 * fElapsedTime ,360.f);

		// Computa trasformazioni
		D3DXMatrixRotationY(&rotation,DEG2RAD(g_CurrentLightRotation1));
		//D3DXMatrixTranslation(&translation,2.0f,2.0f,0.0f);
		//D3DXMatrixScaling(&scale,0.5f,0.5f,0.5f);

		D3DXMatrixIdentity( &g_WorldLight1 );
		D3DXMatrixMultiply(&g_WorldLight1, &g_WorldLight1, &scale);
		D3DXMatrixMultiply(&g_WorldLight1, &g_WorldLight1, &translation);
		D3DXMatrixMultiply(&g_WorldLight1, &g_WorldLight1, &rotation);

		// Computa trasformazioni luce 2
		D3DXMatrixRotationY(&rotation,-DEG2RAD(g_CurrentLightRotation2));
		//D3DXMatrixTranslation(&translation,-2.0f,2.0f,0.0f);
		//D3DXMatrixScaling(&scale,0.5f,0.5f,0.5f);

		D3DXMatrixIdentity( &g_WorldLight2 );
		D3DXMatrixMultiply(&g_WorldLight2, &g_WorldLight2, &scale);
		D3DXMatrixMultiply(&g_WorldLight2, &g_WorldLight2, &translation);
		D3DXMatrixMultiply(&g_WorldLight2, &g_WorldLight2, &rotation);
	}


	void SetupLights(){

		D3DXVECTOR3 poslight1(g_WorldLight1._41,g_WorldLight1._42, g_WorldLight1._43 );
		g_pEffect->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Position")->AsVector()->SetFloatVector(poslight1);
		g_pEffect->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Color")->AsVector()->SetFloatVector(g_Light1.mColour);
		g_pEffect->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Ambient")->AsVector()->SetFloatVector(g_Light1.mAmbientColour);
		g_pEffect->GetVariableByName("g_lights")->GetElement(0)->GetMemberByName("Attenuation")->AsVector()->SetFloatVector(g_Light1.mAttenuation);


		D3DXVECTOR3 poslight2(g_WorldLight2._41,g_WorldLight2._42, g_WorldLight2._43 );
		g_pEffect->GetVariableByName("g_lights")->GetElement(1)->GetMemberByName("Position")->AsVector()->SetFloatVector(poslight2);
		g_pEffect->GetVariableByName("g_lights")->GetElement(1)->GetMemberByName("Color")->AsVector()->SetFloatVector(g_Light2.mColour);
		g_pEffect->GetVariableByName("g_lights")->GetElement(1)->GetMemberByName("Ambient")->AsVector()->SetFloatVector(g_Light2.mAmbientColour);
		g_pEffect->GetVariableByName("g_lights")->GetElement(1)->GetMemberByName("Attenuation")->AsVector()->SetFloatVector(g_Light2.mAttenuation);

	}
};