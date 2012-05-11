#pragma once

#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTgui.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "d3dx11effect.h"
#include "Terrain.h"
#include <d3dx11.h>
#include <vector>

class PSystem
{
	struct ParticleVertex
	{
		D3DXVECTOR3 initialPos;
		D3DXVECTOR3 initialVel;
		D3DXVECTOR2 size;
		float age;
		D3DXVECTOR3 tex;
		UINT type;
		
	};

	struct sMaterial
	{
		D3DXVECTOR4 mKe; // Coefficiente di riflessione emissiva
		D3DXVECTOR4 mKa; // Coefficiente di riflessione ambientale
		D3DXVECTOR4 mKd; // Coefficiente di riflessione ambientale
		D3DXVECTOR4 mKs; // Coefficiente di riflessione speculare
		float mShininess; // esponente shininess
		ID3D11ShaderResourceView *g_pTextureRV; // Texture materiale
	};

	sMaterial g_MaterialCube;

	std::vector<ParticleVertex> mVertices;

	//DX Member -----------------------------------------------------------
	ID3D11Device* mD3DDevice;
	ID3D11DeviceContext* iD3DImmediateContext;
	ID3D11InputLayout *g_pVertexLayout;
	// FX Member
	ID3DX11EffectTechnique *g_pTechniqueG;
	ID3DX11EffectTechnique *g_pTechniqueD;
	ID3DX11Effect *g_pEffect;

	ID3D11ShaderResourceView* mRandomTexRV;
	ID3D11ShaderResourceView* mVolumeDiffTex;
	ID3D11ShaderResourceView* mColorGradient;

	D3DXMATRIX g_WorldCube;

	sMaterial ParticleMaterial;
	ID3D11Buffer* mVB;	
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	D3DXVECTOR4 mEmitPosW;
	D3DXVECTOR4 mEmitDirW;

	float mGameTime;
	float mTimeStep;
	float mAge;

	bool mFirstRun;
public:

	PSystem()
	{
		mFirstRun = true;
		mGameTime = 0.0f;
		mTimeStep = 0.0f;
		mAge      = 0.0f;

		mEmitPosW = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
		mEmitDirW = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.0f);
	}
	~PSystem()
	{

	}

	// Time elapsed since the system was reset.
	float getAge()const;

	HRESULT init(ID3D11Device* device)
	{
		mD3DDevice = device;
		mD3DDevice->GetImmediateContext(&iD3DImmediateContext);

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
		// Il settaggio di questo flag permette di eseguire il debug diretto dello shader, senza
		// impattare in modo marcato sulle performance.
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3D10Blob*	pBlob = NULL;
		ID3D10Blob*	pErrorBlob = NULL;

		// Creazione effetto in g_pEffect, caricandolo dal file fx
		HRESULT hr = D3DX11CompileFromFile(L"particle_effect.fx", NULL, NULL,NULL, "fx_5_0", dwShaderFlags, 0,
			NULL, &pBlob, &pErrorBlob, NULL);


		if( FAILED( hr ) )
		{
			MessageBox( NULL,
				L"Problema nel caricamento di particle_effect.fx.", L"Error", MB_OK );

			if (pErrorBlob)	 {
				MessageBoxA(0, (char*)pErrorBlob->GetBufferPointer(), 0, 0);
			}

			return hr;
		}

		buildRandomTex();

		D3DX11CreateEffectFromMemory(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, mD3DDevice ,&g_pEffect);

		// Trova la tecnica definita nel .fx - nota come un effetto.fx può avere diverse tecniche.
		g_pTechniqueG = g_pEffect->GetTechniqueByName( "DrawTech" );
		g_pTechniqueD = g_pEffect->GetTechniqueByName( "StreamOutTech" );

		hr = SetResource();

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"VELOCITY",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"SIZE",   0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"AGE",   0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"SLICE", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TYPE",   0, DXGI_FORMAT_R32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
			
		};

		UINT numElements = 6;

		// Creiamo e configuriamo il layout di input
		D3DX11_PASS_DESC PassDesc;
		g_pTechniqueG->GetPassByIndex( 0 )->GetDesc( &PassDesc );
		hr = mD3DDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout );
		if( FAILED( hr ) )
			return hr;

		hr = buildVB();

		D3DXMATRIX translation;
		D3DXMATRIX scale;

		// Inizializziamo le proprietà del materiale cubo
		ParticleMaterial.mKa = D3DXVECTOR4(0.1f,0.1f,0.1f,1.0f); // Coefficiente di riflessione ambientale
		ParticleMaterial.mKd = D3DXVECTOR4(0.1f,0.1f,0.1f,1.0f); // Coefficiente di riflessione diffusiva
		ParticleMaterial.mKe = D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f); // Coefficiente di riflessione ambientale
		ParticleMaterial.mKs = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f); // Coefficiente di riflessione speculare
		ParticleMaterial.mShininess = 10; // esponente shininess

		D3DXMatrixIdentity(&g_WorldCube);
		D3DXMatrixScaling(&scale,1.0f,1.0f,1.0f);
		D3DXMatrixTranslation(&translation,0.0f,0.0f,0.0f);
		D3DXMatrixMultiply(&g_WorldCube, &scale, &translation);
		if( FAILED( hr ) )
		{
			MessageBox( NULL,
				(LPCWSTR)__FUNCDNAME__, L"Error", MB_OK );

			if (pErrorBlob)	 {
				MessageBoxA(0, (char*)pErrorBlob->GetBufferPointer(), 0, 0);
			}

			return hr;
		}
	}

	void reset()
	{

	}

	void setEmitPos(const D3DXVECTOR3& emitPosW)
	{
		mEmitPosW = D3DXVECTOR4(emitPosW.x, emitPosW.y, emitPosW.z, 1.0f);
	}

	void setEmitDir(const D3DXVECTOR3& emitDirW)
	{
		mEmitDirW = D3DXVECTOR4(emitDirW.x, emitDirW.y, emitDirW.z, 0.0f);
	}

	void update(float dt, float gameTime)
	{
		mGameTime = gameTime;
		mTimeStep = dt;

		mAge += dt;
	}

	void draw(D3DXMATRIX& view, D3DXMATRIX& proj,D3DXVECTOR3& vieweyevec)
	{
		D3DX11_TECHNIQUE_DESC techDesc;

		// Aggiorna le matrici di Viewing (telecamera) e projection
		HRESULT hr = g_pEffect->GetVariableByName("View")->AsMatrix()->SetMatrix( ( float* )&view );
		hr = g_pEffect->GetVariableByName("Projection")->AsMatrix()->SetMatrix( ( float* )&proj );
		hr = g_pEffect->GetVariableByName("CameraPosition")->AsVector()->SetFloatVector( ( float* )vieweyevec );

		// Setta material per il cubo
		hr = SetMaterial(ParticleMaterial);
		// Setta matrice mondo mesh flat
		hr = g_pEffect->GetVariableByName("World")->AsMatrix()->SetMatrix(g_WorldCube);
		// Rendering mesh cubo
		iD3DImmediateContext->IASetInputLayout( g_pVertexLayout );
		// Setta primitiva
		iD3DImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		// Settiamo i buffer
		UINT stride = sizeof( ParticleVertex );
		UINT offset = 0;
		if(mFirstRun)
			iD3DImmediateContext->IASetVertexBuffers( 0, 1, &mVB, &stride, &offset );
		else
			iD3DImmediateContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

		iD3DImmediateContext->SOSetTargets(1, &mStreamOutVB, &offset);
		//iD3DImmediateContext->IASetIndexBuffer( mIB, DXGI_FORMAT_R32_UINT, 0 );
		// Ottieni la tecnica ed effettua il rendering
		hr=g_pTechniqueD->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			g_pTechniqueD->GetPassByIndex( p )->Apply( 0, iD3DImmediateContext);
			if(mFirstRun)
			{
				iD3DImmediateContext->Draw(2,0);
				mFirstRun = false;
			}
			else
			{
				iD3DImmediateContext->DrawAuto();
			}
			
		}
		ID3D11Buffer* bufferArray[1] = {0};
		iD3DImmediateContext->SOSetTargets(1, bufferArray, &offset);

		// ping-pong the vertex buffers
		std::swap(mDrawVB, mStreamOutVB);

		//
		// Draw the updated particle system we just streamed-out. 
		//
		iD3DImmediateContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

		hr = g_pTechniqueG->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			g_pTechniqueG->GetPassByIndex( p )->Apply(0, iD3DImmediateContext);

			iD3DImmediateContext->DrawAuto();
		}
		//D3DXMatrixIdentity(&g_WorldCube);
		if( FAILED( hr ) )
		{
			MessageBox( NULL,
				(LPCWSTR)__FUNCDNAME__, L"Error", MB_OK );


			return;
		}
	}

private:

	HRESULT SetResource()
	{
		HRESULT hr;
		//D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"flare0.dds",NULL,NULL,&(ParticleMaterial.g_pTextureRV),&hr );
		hr = createTexArray("flare0.dds");
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"smokevol1.dds",NULL,NULL,&(mVolumeDiffTex),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"colorgradient.dds",NULL,NULL,&(mColorGradient),&hr );
		return hr;
	}

	HRESULT SetMaterial(sMaterial &MeshMaterial)
	{
		HRESULT hr;
		// Inizializziamo le proprietà del materiale cubo
		g_MaterialCube.mKa = D3DXVECTOR4(0.1f,0.1f,0.1f,1.0f); // Coefficiente di riflessione ambientale
		g_MaterialCube.mKd = D3DXVECTOR4(0.1f,0.1f,0.1f,1.0f); // Coefficiente di riflessione diffusiva
		g_MaterialCube.mKe = D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f); // Coefficiente di riflessione ambientale
		g_MaterialCube.mKs = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f); // Coefficiente di riflessione speculare
		g_MaterialCube.mShininess = 10; // esponente shininess
		// TODO: Agganciare le proprietà del materiale alle variabili uniform del file fx.
		hr = g_pEffect->GetVariableByName("g_material")->GetMemberByName("Kd")->AsVector()->SetFloatVector(MeshMaterial.mKd);
		hr = g_pEffect->GetVariableByName("g_material")->GetMemberByName("Ks")->AsVector()->SetFloatVector(MeshMaterial.mKs);
		hr = g_pEffect->GetVariableByName("g_material")->GetMemberByName("Ka")->AsVector()->SetFloatVector(MeshMaterial.mKa);
		hr = g_pEffect->GetVariableByName("g_material")->GetMemberByName("Ke")->AsVector()->SetFloatVector(MeshMaterial.mKe);
		hr = g_pEffect->GetVariableByName("g_material")->GetMemberByName("Shininess")->AsScalar()->SetFloat(MeshMaterial.mShininess);

		hr = g_pEffect->GetVariableByName("gTexArray")->AsShaderResource()-> SetResource(MeshMaterial.g_pTextureRV);
		hr = g_pEffect->GetVariableByName("gRandomTex")->AsShaderResource()->SetResource(mRandomTexRV);
		hr = g_pEffect->GetVariableByName("g_txVolumeDiff")->AsShaderResource()->SetResource(mVolumeDiffTex);
		hr = g_pEffect->GetVariableByName("g_txColorGradient")->AsShaderResource()->SetResource(mColorGradient);

		hr = g_pEffect->GetVariableByName("gEmitPosW")->AsVector()->SetFloatVector((float*)mEmitPosW);
		hr = g_pEffect->GetVariableByName("gEmitDirW")->AsVector()->SetFloatVector((float*)mEmitDirW);
		hr = g_pEffect->GetVariableByName("gGameTime")->AsScalar()->SetFloat(mGameTime);
		hr = g_pEffect->GetVariableByName("gTimeStep")->AsScalar()->SetFloat(mTimeStep);
		return hr;
	}

	HRESULT buildVB()
	{
		HRESULT hr;
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.ByteWidth = sizeof(ParticleVertex) * 2;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;

		// The initial particle emitter has type 0 and age 0.  The rest
		// of the particle attributes do not apply to an emitter.
		std::vector<ParticleVertex> vp;
		for (float i=0.0f;i<1.0f;++i)
		{
			ParticleVertex p;
			ZeroMemory(&p, sizeof(ParticleVertex));

			p.initialPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
			p.size = D3DXVECTOR2(3.0f,3.0f);
			p.age  = 0.0f;
			p.type = 0; 
			p.tex = D3DXVECTOR3(0.0f,0.0f,0.0f);
			vp.push_back(p);
		}

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &vp[0];
		hr = mD3DDevice->CreateBuffer(&vbd, &vinitData, &mVB);

		vbd.ByteWidth = sizeof(ParticleVertex) * 2000000;
		vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;


		hr = mD3DDevice->CreateBuffer(&vbd, 0, &mDrawVB);
		hr = mD3DDevice->CreateBuffer(&vbd, 0, &mStreamOutVB);
		return hr;
	}

	float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

	HRESULT buildRandomTex()
	{
		// 
		// Create the random data.
		//
		D3DXVECTOR4 randomValues[1024];

		for(int i = 0; i < 1024; ++i)
		{
			randomValues[i].x = RandF(-1.0f, 1.0f);
			randomValues[i].y = RandF(-1.0f, 1.0f);
			randomValues[i].z = RandF(-1.0f, 1.0f);
			randomValues[i].w = RandF(-1.0f, 1.0f);
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = randomValues;
		initData.SysMemPitch = 1024*sizeof(D3DXVECTOR4);
		initData.SysMemSlicePitch = 1024*sizeof(D3DXVECTOR4);
		//
		// Create the texture.
		//
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = 1024;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		ID3D11Texture1D* randomTex = 0;
		mD3DDevice->CreateTexture1D(&texDesc, &initData, &randomTex);
		//
		// Create the resource view.
		//
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		return mD3DDevice->CreateShaderResourceView(randomTex, &viewDesc, &mRandomTexRV);
	}

	HRESULT createTexArray(const char* arrayName)
	{
		UINT arraySize = 1;
		HRESULT hr;
		std::vector<ID3D11Texture2D*> srcTex(arraySize, 0);
		for(UINT i = 0; i < arraySize; ++i)
		{
			D3DX11_IMAGE_LOAD_INFO loadInfo;

			loadInfo.Width  = D3DX11_FROM_FILE;
			loadInfo.Height = D3DX11_FROM_FILE;
			loadInfo.Depth  = D3DX11_FROM_FILE;
			loadInfo.FirstMipLevel = 0;
			loadInfo.MipLevels = D3DX11_FROM_FILE;
			loadInfo.Usage = D3D11_USAGE_STAGING;
			loadInfo.BindFlags = 0;
			loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			loadInfo.MiscFlags = 0;
			loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			loadInfo.Filter = D3DX11_FILTER_NONE;
			loadInfo.MipFilter = D3DX11_FILTER_NONE;
			loadInfo.pSrcInfo  = 0;

			hr = D3DX11CreateTextureFromFile(mD3DDevice, L"flare0.dds", 
				&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0);
			if( FAILED(hr))
			{

				return hr;
			}
		}

		//
		// Create the texture array.  Each element in the texture 
		// array has the same format/dimensions.
		//

		D3D11_TEXTURE2D_DESC texElementDesc;
		srcTex[0]->GetDesc(&texElementDesc);

		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width              = texElementDesc.Width;
		texArrayDesc.Height             = texElementDesc.Height;
		texArrayDesc.MipLevels          = texElementDesc.MipLevels;
		texArrayDesc.ArraySize          = arraySize;
		texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
		texArrayDesc.SampleDesc.Count   = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags     = 0;
		texArrayDesc.MiscFlags          = 0;

		ID3D11Texture2D* texArray = 0;
		mD3DDevice->CreateTexture2D( &texArrayDesc, 0, &texArray);

		//
		// Copy individual texture elements into texture array.
		//

		// for each texture element...
		for(UINT i = 0; i < arraySize; ++i)
		{
			// for each mipmap level...
			for(UINT j = 0; j < texElementDesc.MipLevels; ++j)
			{
				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				iD3DImmediateContext->Map(srcTex[i],j, D3D11_MAP_READ, 0, &mappedTex2D);

				iD3DImmediateContext->UpdateSubresource(texArray, 
					D3D11CalcSubresource(j, i, texElementDesc.MipLevels),
					0, mappedTex2D.pData, mappedTex2D.RowPitch, 0);

				iD3DImmediateContext->Unmap(srcTex[i],j);
			}
		}	

		//
		// Create a resource view to the texture array.
		//

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = arraySize;

		
		mD3DDevice->CreateShaderResourceView(texArray, &viewDesc, &ParticleMaterial.g_pTextureRV);

		//
		// Cleanup--we only need the resource view.
		//
		return hr;
	}

	PSystem(const PSystem& rhs);
	PSystem& operator=(const PSystem& rhs);
};