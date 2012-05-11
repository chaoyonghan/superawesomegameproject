//--------------------------------------------------------------------------------------
// File: MGC_example09.cpp
// 
// **************************************
// Texture Mapping - Utilizzo del color texture mapping
// **************************************
//
// Copyright (c) Roberto Toldo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------

#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTgui.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "d3dx11effect.h"
#include "Terrain.h"
#include "Light.h"
#include "ParticlesSystem.h"


//--------------------------------------------------------------------------------------
// Dichiarazione strutture
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Tex;
};
// Struttura che definisce un materiale associato
struct sMaterial{
	D3DXVECTOR4 mKe; // Coefficiente di riflessione emissiva
	D3DXVECTOR4 mKa; // Coefficiente di riflessione ambientale
	D3DXVECTOR4 mKd; // Coefficiente di riflessione ambientale
	D3DXVECTOR4 mKs; // Coefficiente di riflessione speculare
	float mShininess; // esponente shininess
	ID3D11ShaderResourceView *g_pTextureRV; // Texture materiale
};

//--------------------------------------------------------------------------------------
// Dichiarazione variabili globali
//--------------------------------------------------------------------------------------
CDXUTTextHelper *gmTextHelper = NULL;				 // Text Helper Class provided by DXUT
bool gbToggleFps = false;							// Fps Text Switch
bool gbWireFrame = false;
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs (included text)
bool gbToggleLights;
bool gbToggleTexture;
ID3DX11Effect*           g_pEffect = NULL;
ID3DX11EffectTechnique*  g_pTechniqueGouradShading = NULL;
ID3DX11EffectTechnique*  g_pTechniquePhongShading = NULL;
ID3DX11EffectTechnique*  g_pTechnique = NULL;
ID3D11InputLayout*      g_pVertexLayout = NULL;
ID3DX11EffectShaderResourceVariable *g_pTextureVariable; // Variabile di collegamento per la texture nello shader
ID3D11RasterizerState *g_pRasterizerStateWF = NULL;
ID3D11RasterizerState *g_pRasterizerStateS = NULL;
// Definiamo i buffer per contenere le strutture del cubo
ID3D11Buffer* g_pVertexBufferCube = NULL;
ID3D11Buffer* g_pIndexBufferCube = NULL;
// Definiamo anche il materiale
sMaterial g_MaterialCube;
// Definiamo la matrice mondo relativa al cubo
D3DXMATRIX  g_WorldCube; // Posizione orientazione nel mondo luce 1

PSystem par;

//Terrain
Terrain pTerrain(513);

//Lights

Lights pLight;

// Matrici di trasformazione
D3DXMATRIX                  g_View;
D3DXMATRIX                  g_Projection;

// Parametri telecamera
D3DXVECTOR3 g_ViewUpVector; // Vettore Up
D3DXVECTOR3 g_ViewEyeVector; // Posizione
D3DXVECTOR3 g_ViewAtVector; // Punto fissato
D3DXVECTOR3 g_ViewViewVector; // Vettore vista - At = Eye + View
D3DXVECTOR3 g_ViewRightVector; // Vettore dx




//--------------------------------------------------------------------------------------
// Dichiarazione costanti
//--------------------------------------------------------------------------------------
#define CAMERASPEED 50.0f
#define CAMERAANGLESPEED 0.3f

//--------------------------------------------------------------------------------------
// Dichiarazione prototipi funzione 
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
									  void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
										  const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
						  void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

void PrintTitleAndFps();
void SetMaterial(sMaterial &MeshMaterial);
void SetupLights();

//--------------------------------------------------------------------------------------
// Funzione main del programma. Inizializza la finestra utilizzando DXUT ed entra nel main loop.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Settiamo le callback di DXUT
	DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
	DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
	DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
	DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
	DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );
	DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );

	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( OnKeyboard );
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

	DXUTInit( true, // Effettua il parsing della linea di comando
		true, // mostra errori come message box
		NULL ); // nessun comando extra alla linea di comando
	DXUTSetCursorSettings( true, true ); // Mostra il cursore e nascondilo quando in full screen
	DXUTCreateWindow( L"Sarevok: Bumped Terrain & Smoke Particle" );
	DXUTCreateDevice( D3D_FEATURE_LEVEL_10_1, true, 640, 480 );
	DXUTMainLoop(); // entra nel main loop.

	return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Controlla i device disponibili e Rifiuta ogni device non compabile.
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext){
	return true;
}


//--------------------------------------------------------------------------------------
// Inizializza le risorse da utilizzare. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
									  void* pUserContext )
{
	
	g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, DXUTGetD3D11DeviceContext());

	gmTextHelper = new CDXUTTextHelper( pd3dDevice, DXUTGetD3D11DeviceContext(),&g_DialogResourceManager,15 );

	gbToggleFps = false;
	HRESULT hr;

	pTerrain.Init(pd3dDevice);
	pLight.Init(pd3dDevice);
	par.init(pd3dDevice);
	par.setEmitPos(D3DXVECTOR3(0.0f,5.0f,0.0f));
	g_ViewEyeVector = D3DXVECTOR3( 0.0f, 1.0f, -5.0f );
	g_ViewUpVector = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	g_ViewAtVector = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	// Il vettore vista è semplicemente eye - at normalizzato
	g_ViewViewVector = (g_ViewAtVector - g_ViewEyeVector); 
	D3DXVec3Normalize(&g_ViewViewVector, &g_ViewViewVector);
	// Il vettore dx è ottenuto come cross-product del vettore up e del vettore vista. Anch'esso è normalizzato
	D3DXVec3Cross(&g_ViewRightVector,&g_ViewUpVector,&g_ViewViewVector);

	// Costruisce matrice di view
	D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );

	// Initialize the projection matrix
	D3DXMatrixPerspectiveFovLH( &g_Projection, ( float )D3DX_PI * 0.25f,((float)DXUTGetWindowWidth() / (float)DXUTGetWindowHeight()), 0.1f, 200.0f );

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc,sizeof(D3D11_RASTERIZER_DESC) );
	rsDesc.CullMode = D3D11_CULL_NONE;	
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;

	hr = pd3dDevice->CreateRasterizerState(&rsDesc, &g_pRasterizerStateWF);
	
	rsDesc.FillMode = D3D11_FILL_SOLID;

	hr = pd3dDevice->CreateRasterizerState(&rsDesc, &g_pRasterizerStateS);

	DXUTGetD3D11DeviceContext()->RSSetState(g_pRasterizerStateS);
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Crea ogni risorsa dipendendente dal back buffer.
// Resetta l'aspectratio modificando viewport o matrice di proiezione.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext ){

	g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBufferSurfaceDesc );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Effettua il rendering
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext){

	// Pulisci back buffer
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView(); // Ottieni il render target in cui stiamo disegnando
	pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );

	// Setta e pulisci lo Z-Buffer (depth stencil)
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );
	pTerrain.draw(g_View,g_Projection,g_ViewEyeVector);
	pLight.draw(pTerrain.GetEffect(),pTerrain.getTechnique(),pTerrain.getLayout(),g_View,g_Projection,g_ViewEyeVector);
	par.update(fElapsedTime, fTime);
	par.draw(g_View,g_Projection,g_ViewEyeVector);
	PrintTitleAndFps();
}

//--------------------------------------------------------------------------------------
// Stampa nella parte alta dello schermo titolo e frame per secondo
//--------------------------------------------------------------------------------------
void PrintTitleAndFps(){

	gmTextHelper->Begin();
	gmTextHelper->SetInsertionPos( 1, 1 );
	gmTextHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );		

	// Stampa la stringa contenente i FPS
	if(gbToggleFps){
		gmTextHelper->DrawTextLine(DXUTGetFrameStats(true));
	}

	// Stampa la stringa contenente il titolo a sinistra
	gmTextHelper->DrawTextLine(DXUTGetWindowTitle());

	gmTextHelper->End();
}

//--------------------------------------------------------------------------------------
// Rilascia eventuali risorse create in OnD3D11ResizedSwapChain
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Rilascia eventuali risorse create in OnD3D11CreateDevice 
// Device-Swapbuffer e rendertarget sono rilasciate in automatico.
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	if(gmTextHelper)
		delete gmTextHelper;
	gmTextHelper = 0;

	g_DialogResourceManager.OnD3D11DestroyDevice();

	if( g_pVertexLayout ) g_pVertexLayout->Release();
	if( g_pEffect ) g_pEffect->Release();
	if( g_pVertexBufferCube ) g_pVertexBufferCube->Release();
	if( g_pIndexBufferCube ) g_pIndexBufferCube->Release();
	if( g_MaterialCube.g_pTextureRV) g_MaterialCube.g_pTextureRV->Release();

	//g_MeshSphere.Destroy();

}

//--------------------------------------------------------------------------------------
// Chiamata prima di inizializzare un device, ci permette di specificare parametri personalizzati per il device
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	pDeviceSettings->d3d11.AutoCreateDepthStencil = true; // Disabilitiamo per questo esempio la creazione del depth stencil buffer.

	return true;
}

//--------------------------------------------------------------------------------------
// Gestisce modifiche alla scena.  Le animazioni andranno inserite qui. Chiamata prima di OnD3D11FrameRender nel main loop
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{

	//pLight.OnFrameMove(fElapsedTime);

	// Gestione movimenti telecamera

	if(DXUTIsKeyDown(VK_LEFT)){
		// Spostamento negativo rispetto direzione del right vector (sinistra)
		g_ViewEyeVector -= DXUTGetElapsedTime() * CAMERASPEED * g_ViewRightVector;
		g_ViewAtVector -= DXUTGetElapsedTime() * CAMERASPEED * g_ViewRightVector;
		D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );
	}

	if(DXUTIsKeyDown(VK_RIGHT)){
		// Spostamento positivo rispetto direzione del right vector (destra)
		g_ViewEyeVector += fElapsedTime * CAMERASPEED * g_ViewRightVector;
		g_ViewAtVector += fElapsedTime * CAMERASPEED * g_ViewRightVector;
		D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );
	}

	if(DXUTIsKeyDown(VK_UP)){
		// Spostamento positivo rispetto alla direzione di vista (avanti)
		g_ViewEyeVector += fElapsedTime * CAMERASPEED * g_ViewViewVector;
		g_ViewAtVector += fElapsedTime * CAMERASPEED * g_ViewViewVector;
		D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );
	}

	if(DXUTIsKeyDown(VK_DOWN)){
		// Spostamento negativo rispetto alla direzione di vista (indietro)
		g_ViewEyeVector -= fElapsedTime * CAMERASPEED * g_ViewViewVector;
		g_ViewAtVector -= fElapsedTime * CAMERASPEED * g_ViewViewVector;
		D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );
	}

	if(DXUTIsKeyDown('q') || DXUTIsKeyDown('Q')){
		// Spostamento positivo rispetto alla direzione dell'Up Vector (Su)
		g_ViewEyeVector += fElapsedTime * CAMERASPEED * g_ViewUpVector;
		g_ViewAtVector += fElapsedTime * CAMERASPEED * g_ViewUpVector;
		D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );
	}

	if(DXUTIsKeyDown('a') || DXUTIsKeyDown('A')){
		// Spostamento negativo rispetto alla direzione dell'Up Vector (Giù)
		g_ViewEyeVector -= fElapsedTime * CAMERASPEED * g_ViewUpVector;
		g_ViewAtVector -= fElapsedTime * CAMERASPEED * g_ViewUpVector;
		D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );
	}

	if(gbWireFrame)
	{
		DXUTGetD3D11DeviceContext()->RSSetState(g_pRasterizerStateWF);
	}
	else
	{
		DXUTGetD3D11DeviceContext()->RSSetState(g_pRasterizerStateS);
	}

	static bool bMoving = false;
	static POINT prevCoord;
	if(DXUTIsMouseButtonDown(VK_LBUTTON)){
		POINT coord;
		GetCursorPos(&coord);
		if(bMoving){

			// Moto relativo a X e Y
			float motionX = (float)coord.x - (float)prevCoord.x;
			float motionY = (float)coord.y - (float)prevCoord.y;

			// Costruiamo la matrice di Yaw (rotazione rispetto a Y) con un angolo dipendente dalla direzione relativa rispetto a X
			D3DXMATRIX yawMatrix;
			D3DXMatrixRotationAxis(&yawMatrix, &g_ViewUpVector, DEG2RAD(motionX * CAMERAANGLESPEED) ); 
			// Applichiamo la trasformazione ai vettori view e right.
			// Dato che la matrice di rotazione è ortogonale non abbiamo bisogno di rinormalizzare i vettori dopo la trasformazione
			D3DXVec3TransformCoord(&g_ViewViewVector, &g_ViewViewVector, &yawMatrix);
			D3DXVec3TransformCoord(&g_ViewRightVector, &g_ViewRightVector, &yawMatrix); 

			// Costruiamo la matrice di Pitch (rotazione rispetto a X) con un angolo dipendente dalla direzione relativa rispetto a Y
			D3DXMATRIX pitchMatrix;
			D3DXMatrixRotationAxis(&pitchMatrix, &g_ViewRightVector, DEG2RAD(motionY * CAMERAANGLESPEED) ); 
			// Applichiamo la trasformazione ai vettori view e Up.
			// Dato che la matrice di rotazione è ortogonale non abbiamo bisogno di rinormalizzare i vettori dopo la trasformazione
			D3DXVec3TransformCoord(&g_ViewViewVector, &g_ViewViewVector, &pitchMatrix);
			D3DXVec3TransformCoord(&g_ViewUpVector, &g_ViewUpVector, &pitchMatrix); 

			// Aggiorniamo la posizione osservata e ricostruiamo la matrice di view
			g_ViewAtVector = g_ViewEyeVector + g_ViewViewVector;
			D3DXMatrixLookAtLH( &g_View, &g_ViewEyeVector, &g_ViewAtVector, &g_ViewUpVector );

		}
		prevCoord = coord;
		bMoving = true;
	}
	else{
		bMoving = false;
	}

}


//--------------------------------------------------------------------------------------
// Gestisce coda messaggi
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
						  void* pUserContext )
{
	return 0;
}

//--------------------------------------------------------------------------------------
// Gestisce eventi da tastiera
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if( bKeyDown )
	{
		switch( nChar )
		{
			case 'f': 
			case 'F': 
				gbToggleFps = !gbToggleFps;
				break;
			case VK_F10:
				DXUTToggleFullScreen();
				break;	
			case 's': 
			case 'S':
				if(g_pTechnique == g_pTechniqueGouradShading)
					g_pTechnique = g_pTechniquePhongShading;
				else
					g_pTechnique = g_pTechniqueGouradShading;				
				break;	
			case 'l': 
			case 'L': 
				gbToggleLights = !gbToggleLights;
				break;	
			// TODO: Disabilita texture alla pressione del tasto 't'
			case 'r': 
			case 'R': 
				gbWireFrame = !gbWireFrame;
				break;	
		}
	}
}
