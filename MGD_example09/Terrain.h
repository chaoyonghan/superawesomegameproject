#pragma once
#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTgui.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "d3dx11effect.h"
#include "HeightMap.h"
#include "Light.h"
#include <vector>

class Terrain
{
	//friend Lights;

	struct TerrainVertex
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR3 Normal;
		D3DXVECTOR3 Tan;
		D3DXVECTOR3 Binorm;
		D3DXVECTOR2 Tex;
	};

	struct sMaterial
	{
		D3DXVECTOR4 mKe; // Coefficiente di riflessione emissiva
		D3DXVECTOR4 mKa; // Coefficiente di riflessione ambientale
		D3DXVECTOR4 mKd; // Coefficiente di riflessione ambientale
		D3DXVECTOR4 mKs; // Coefficiente di riflessione speculare
		float mShininess; // esponente shininess
		ID3D11ShaderResourceView *g_pTextureRV; // Texture materiale
		ID3D11ShaderResourceView* mLayer0;
		ID3D11ShaderResourceView* mLayer1;
		ID3D11ShaderResourceView* mLayer2;
		ID3D11ShaderResourceView* mLayer3;
		ID3D11ShaderResourceView* mLayer4;

		ID3D11ShaderResourceView* mLayer0N;
		ID3D11ShaderResourceView* mLayer1N;
		ID3D11ShaderResourceView* mLayer2N;
		ID3D11ShaderResourceView* mLayer3N;
		ID3D11ShaderResourceView* mLayer4N;

		ID3D11ShaderResourceView* mBlendMap;
	};

	std::vector<TerrainVertex> mVertices;

	float mHeight;
	float mWidth;

	float mNumVertices;
	float mNumIndices;
	float mBlockSizeWidth;
	float mBlockSizeHeight;

	float halfWidth;
	float halfHeight;

	HeightMap mHeightMap;

	D3DXVECTOR2 mOrigin;

	//DX Membet -----------------------------------------------------------
	ID3D11Device* mD3DDevice;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11InputLayout *g_pVertexLayout;
	// Tecnica di default
	ID3DX11EffectTechnique *g_pTechnique;
	ID3DX11Effect *g_pEffect;
	D3DXMATRIX g_WorldCube;

	sMaterial g_MaterialCube;

	Lights mLight;

	ID3D11DeviceContext* iD3DImmediateContext;

	void fillData()
	{
		mNumVertices= mHeightMap.getWidth()*mHeightMap.getHeight();
		mNumIndices = ((mHeightMap.getWidth()-1) * (mHeightMap.getHeight()-1)) * 6;
		mBlockSizeWidth = mWidth / mHeightMap.getWidth();
		mBlockSizeHeight = mHeight / mHeightMap.getHeight();
		halfWidth = mWidth / 2;
		halfHeight = mHeight / 2;
	}

public:

	Terrain(float iHeight,float iWidth, D3DXVECTOR2 iOrigin)
		:mHeight(iHeight)
		,mWidth(iWidth)
		,mOrigin(iOrigin)
		,mD3DDevice(nullptr)
		,mVB(nullptr)
		,mIB(nullptr)
		,mHeightMap(iHeight,iWidth,L"coastMountain513.raw",0.35f,-20.0f)
	{

	}

	Terrain(float iHeight,float iWidth)
		:mHeight(iHeight)
		,mWidth(iWidth)
		,mOrigin(D3DXVECTOR2(0,0))
		,mD3DDevice(nullptr)
		,mVB(nullptr)
		,mIB(nullptr)
		,mHeightMap(iHeight,iWidth,L"coastMountain513.raw",0.35f,-20.0f)
	{

	}

	Terrain(D3DXVECTOR2 iOrigin)
		:mHeight(513)
		,mWidth(513)
		,mOrigin(iOrigin)
		,mD3DDevice(nullptr)
		,mVB(nullptr)
		,mIB(nullptr)
		,mHeightMap(513.0f,513.0f,L"coastMountain513.raw",0.35f,-20.0f)
	{

	}

	Terrain(float iSize, D3DXVECTOR2 iOrigin)
		:mHeight(iSize)
		,mWidth(iSize)
		,mOrigin(iOrigin)
		,mD3DDevice(nullptr)
		,mVB(nullptr)
		,mIB(nullptr)
		,mHeightMap(iSize,iSize,L"coastMountain513.raw",0.35f,-20.0f)
	{

	}

	Terrain(float iSize)
		:mHeight(iSize)
		,mWidth(iSize)
		,mOrigin(D3DXVECTOR2(0,0))
		,mD3DDevice(nullptr)
		,mVB(nullptr)
		,mIB(nullptr)
		,mHeightMap(iSize,iSize,L"coastMountain513.raw",0.35f,-20.0f)
	{

	}

	ID3DX11EffectTechnique *getTechnique()
	{
		return g_pTechnique;
	}
	ID3DX11Effect *GetEffect()
	{
		return g_pEffect;
	}

	ID3D11InputLayout * getLayout()
	{
		return g_pVertexLayout;
	}

	HRESULT Init(ID3D11Device* iD3DDevice)
	{
		fillData();
		mD3DDevice = iD3DDevice;
		mD3DDevice->GetImmediateContext(&iD3DImmediateContext);

		DWORD dwShaderFlags
			= D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
		// Il settaggio di questo flag permette di eseguire il debug diretto dello shader, senza
		// impattare in modo marcato sulle performance.
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3D10Blob*	pBlob = NULL;
		ID3D10Blob*	pErrorBlob = NULL;

		// Creazione effetto in g_pEffect, caricandolo dal file fx
		HRESULT hr = D3DX11CompileFromFile(L"basic_terrain_effect.fx", NULL, NULL,NULL, "fx_5_0", dwShaderFlags, 0,
			NULL, &pBlob, &pErrorBlob, NULL);


		if( FAILED( hr ) )
		{
			MessageBox( NULL,
				L"Problema nel caricamento di basic_lighting_effect.fx.", L"Error", MB_OK );

			if (pErrorBlob)	 {
				MessageBoxA(0, (char*)pErrorBlob->GetBufferPointer(), 0, 0);
			}

			return hr;
		}

		D3DX11CreateEffectFromMemory(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, mD3DDevice ,&g_pEffect);

		// Trova la tecnica definita nel .fx - nota come un effetto.fx può avere diverse tecniche.
		g_pTechnique = g_pEffect->GetTechniqueByName( "GouradShading" );

		g_pEffect->GetVariableByName("EnableTexture")->AsScalar()->SetBool(true);

		buildVertexIndexBuffer();

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		UINT numElements = sizeof( layout ) / sizeof( layout[0] );

		// Creiamo e configuriamo il layout di input
		D3DX11_PASS_DESC PassDesc;
		g_pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
		hr = mD3DDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout );
		if( FAILED( hr ) )
			return hr;

		D3DXMATRIX translation;
		D3DXMATRIX scale;

		// Inizializziamo le proprietà del materiale cubo
		g_MaterialCube.mKa = D3DXVECTOR4(0.1f,0.1f,0.1f,1.0f); // Coefficiente di riflessione ambientale
		g_MaterialCube.mKd = D3DXVECTOR4(0.1f,0.1f,0.1f,1.0f); // Coefficiente di riflessione diffusiva
		g_MaterialCube.mKe = D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f); // Coefficiente di riflessione ambientale
		g_MaterialCube.mKs = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f); // Coefficiente di riflessione speculare
		g_MaterialCube.mShininess = 10; // esponente shininess
		D3DXMatrixIdentity(&g_WorldCube);
		D3DXMatrixScaling(&scale,1.0f,1.0f,1.0f);
		D3DXMatrixTranslation(&translation,0.0f,0.0f,0.0f);
		D3DXMatrixMultiply(&g_WorldCube, &scale, &translation);

		// TODO: Caricare la texture contenuta in 'colormap.jpg'
		SetShaderResources();

		setDirectionToSun(D3DXVECTOR3(0.707f, -0.707f, 0.0f));

		//mLight.g_pEffect = g_pEffect;

		if( FAILED( hr ) )
			return hr;
	}

	void setDirectionToSun(const D3DXVECTOR3& v)
	{
		D3DXVECTOR4 temp(v.x, v.y, v.z, 0.0f);
		g_pEffect->GetVariableByName("gBlendMap")->AsVector()->SetFloatVector((float*)temp);
	}

	void SetShaderResources()
	{
		HRESULT hr;

		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"grass.dds",NULL,NULL,&(g_MaterialCube.mLayer0),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"lightdirt.dds",NULL,NULL,&(g_MaterialCube.mLayer1),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"darkdirt.dds",NULL,NULL,&(g_MaterialCube.mLayer2),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"stone.dds",NULL,NULL,&(g_MaterialCube.mLayer3),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"snow.dds",NULL,NULL,&(g_MaterialCube.mLayer4),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"blend.dds",NULL,NULL,&(g_MaterialCube.mBlendMap),&hr );

		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"grass_normal.dds",NULL,NULL,&(g_MaterialCube.mLayer0N),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"lightdirt_normal.dds",NULL,NULL,&(g_MaterialCube.mLayer1N),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"darkdirt_normal.dds",NULL,NULL,&(g_MaterialCube.mLayer2N),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"stone_normal.dds",NULL,NULL,&(g_MaterialCube.mLayer3N),&hr );
		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"snow_normal.dds",NULL,NULL,&(g_MaterialCube.mLayer4N),&hr );

		D3DX11CreateShaderResourceViewFromFile(mD3DDevice,L"blend.dds",NULL,NULL,&(g_MaterialCube.mBlendMap),&hr );
	}

	void draw(D3DXMATRIX& view, D3DXMATRIX& proj,D3DXVECTOR3& vieweyevec)
	{
		D3DX11_TECHNIQUE_DESC techDesc;

		// Aggiorna le matrici di Viewing (telecamera) e projection
		g_pEffect->GetVariableByName("View")->AsMatrix()->SetMatrix( ( float* )&view );
		g_pEffect->GetVariableByName("Projection")->AsMatrix()->SetMatrix( ( float* )&proj );
		g_pEffect->GetVariableByName("CameraPosition")->AsVector()->SetFloatVector( ( float* )vieweyevec );


		// Abilita le luci
		g_pEffect->GetVariableByName("DefaultColor")->AsVector()->SetFloatVector( ( float* )D3DXVECTOR4(0.05f,0.05f,0.05f,1.0) );

		// Setta material per il cubo
		SetMaterial(g_MaterialCube);
		// Setta matrice mondo mesh flat
		g_pEffect->GetVariableByName("World")->AsMatrix()->SetMatrix(g_WorldCube);
		// Rendering mesh cubo
		iD3DImmediateContext->IASetInputLayout( g_pVertexLayout );
		// Setta primitiva
		iD3DImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		// Settiamo i buffer
		UINT stride = sizeof( TerrainVertex );
		UINT offset = 0;
		iD3DImmediateContext->IASetVertexBuffers( 0, 1, &mVB, &stride, &offset );
		iD3DImmediateContext->IASetIndexBuffer( mIB, DXGI_FORMAT_R32_UINT, 0 );
		// Ottieni la tecnica ed effettua il rendering
		g_pTechnique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			g_pTechnique->GetPassByIndex( p )->Apply( 0, iD3DImmediateContext);
			iD3DImmediateContext->DrawIndexed( mNumIndices, 0, 0 );
		}
	}

	

	void SetMaterial(sMaterial &MeshMaterial)
	{
		// TODO: Agganciare le proprietà del materiale alle variabili uniform del file fx.
		g_pEffect->GetVariableByName("g_material")->GetMemberByName("Kd")->AsVector()->SetFloatVector(MeshMaterial.mKd);
		g_pEffect->GetVariableByName("g_material")->GetMemberByName("Ks")->AsVector()->SetFloatVector(MeshMaterial.mKs);
		g_pEffect->GetVariableByName("g_material")->GetMemberByName("Ka")->AsVector()->SetFloatVector(MeshMaterial.mKa);
		g_pEffect->GetVariableByName("g_material")->GetMemberByName("Ke")->AsVector()->SetFloatVector(MeshMaterial.mKe);
		g_pEffect->GetVariableByName("g_material")->GetMemberByName("Shininess")->AsScalar()->SetFloat(MeshMaterial.mShininess);
		g_pEffect->GetVariableByName("gLayer0")->AsShaderResource()-> SetResource(MeshMaterial.mLayer0);
		g_pEffect->GetVariableByName("gLayer1")->AsShaderResource()-> SetResource(MeshMaterial.mLayer1);
		g_pEffect->GetVariableByName("gLayer2")->AsShaderResource()-> SetResource(MeshMaterial.mLayer2);
		g_pEffect->GetVariableByName("gLayer3")->AsShaderResource()-> SetResource(MeshMaterial.mLayer3);
		g_pEffect->GetVariableByName("gLayer4")->AsShaderResource()-> SetResource(MeshMaterial.mLayer4);
		g_pEffect->GetVariableByName("gLayer0N")->AsShaderResource()-> SetResource(MeshMaterial.mLayer0N);
		g_pEffect->GetVariableByName("gLayer1N")->AsShaderResource()-> SetResource(MeshMaterial.mLayer1N);
		g_pEffect->GetVariableByName("gLayer2N")->AsShaderResource()-> SetResource(MeshMaterial.mLayer2N);
		g_pEffect->GetVariableByName("gLayer3N")->AsShaderResource()-> SetResource(MeshMaterial.mLayer3N);
		g_pEffect->GetVariableByName("gLayer4N")->AsShaderResource()-> SetResource(MeshMaterial.mLayer4N);
		g_pEffect->GetVariableByName("gBlendMap")->AsShaderResource()-> SetResource(MeshMaterial.mBlendMap);

	}

	void buildVertexIndexBuffer()
	{
		std::vector<TerrainVertex> vertices(mNumVertices);

		float du = 1.0f / (mHeightMap.getWidth()-1);
		float dv = 1.0f / (mHeightMap.getHeight()-1);

		for(size_t i = 0; i < mHeightMap.getHeight() ; ++i)
		{
			for(size_t j = 0; j < mHeightMap.getWidth() ; ++j)
			{
				vertices[i * mHeightMap.getWidth() + j].Pos = D3DXVECTOR3(((float)j * mBlockSizeWidth) - halfWidth , mHeightMap[i*mHeight+j],((float)i * mBlockSizeHeight) -halfHeight);
				//TODO: we have to setting the normal
				vertices[i * mHeightMap.getWidth() + j].Normal = D3DXVECTOR3( 0.0f,  0.0f, 1.0f );\
				//Wrap one texture to whole grid terrain 
				vertices[i * mHeightMap.getWidth() + j].Tex = D3DXVECTOR2((float)j * du , (float)i * dv);
			}
		}

		float invTwoDX = 1.0f / (2.0f*mBlockSizeWidth);
		float invTwoDZ = 1.0f / (2.0f*mBlockSizeHeight);
		for(UINT i = 2; i < mHeightMap.getHeight()-1; ++i)
		{
			for(UINT j = 2; j < mHeightMap.getWidth()-1; ++j)
			{
				float t = mHeightMap[(i-1)*mHeightMap.getWidth() + j];
				float b = mHeightMap[(i+1)*mHeightMap.getWidth() + j];
				float l = mHeightMap[i*mHeightMap.getWidth() + j - 1];
				float r = mHeightMap[i*mHeightMap.getWidth() + j + 1];

				D3DXVECTOR3 tanZ(0.0f, (t-b)*invTwoDZ, 1.0f);
				D3DXVECTOR3 tanX(1.0f, (r-l)*invTwoDX, 0.0f);

				D3DXVECTOR3 N;
				D3DXVec3Cross(&N, &tanZ, &tanX);
				D3DXVec3Normalize(&N, &N);

				vertices[i*mWidth+j].Normal = N;
			}
		}
	
		std::vector<DWORD> indices(mNumIndices); // 3 indices per face

		// Iterate over each quad and compute indices.
		//int k = 0;
		//for(UINT i = 0; i < mHeight - 1; ++i)
		//{
		//	for(UINT j = 0; j < mWidth - 1; ++j)
		//	{
		//		indices[k]   = i*mWidth+j;
		//		indices[k+1] = i*mWidth+j+1;
		//		indices[k+2] = (i+1)*mWidth+j;

		//		indices[k+3] = (i+1)*mWidth+j;
		//		indices[k+4] = i*mWidth+j+1;
		//		indices[k+5] = (i+1)*mWidth+j+1;

		//		k += 6; // next quad
		//	}
		//}
		int indicesCount=0;
		for(size_t i = 0; i < mHeightMap.getHeight() - 1; ++i)
		{
			for(size_t j = 0; j < mHeightMap.getWidth() - 1 ; ++j)
			{
				unsigned long index = j + i * mHeightMap.getWidth();
				indices[indicesCount++] = index + mHeightMap.getWidth() + 1;
				indices[indicesCount++] = index;
				indices[indicesCount++] = index + mHeightMap.getWidth();

				indices[indicesCount++] = index + 1;
				indices[indicesCount++] = index;
				indices[indicesCount++] = index + mHeightMap.getWidth() + 1;
			}
		}

		for (int i=0; i < mNumIndices/3.0f; i ++){
			int v0 = indices[i*3+0];
			int v1 = indices[i*3+1];
			int v2 = indices[i*3+2];

			ComputeTangentBasis(vertices[v0].Pos, vertices[v1].Pos, vertices[v2].Pos, vertices[v0].Tex, vertices[v1].Tex, vertices[v2].Tex, 
				vertices[v0].Normal, vertices[v0].Tan, vertices[v0].Binorm);
			// Tangenti e binormali sono uguali per i vertici del triangolo.
			vertices[v1].Normal = vertices[v0].Normal;
			vertices[v1].Tan = vertices[v0].Tan;
			vertices[v1].Binorm = vertices[v0].Binorm;
			vertices[v2].Normal = vertices[v0].Normal;
			vertices[v2].Tan = vertices[v0].Tan;
			vertices[v2].Binorm = vertices[v0].Binorm;
		}

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(TerrainVertex) * mNumVertices;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = &vertices[0];
		mD3DDevice->CreateBuffer(&vbd, &vinitData, &mVB);

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(DWORD) * (mHeight - 1) * (mWidth - 1) * 6;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = &indices[0];
		mD3DDevice->CreateBuffer(&ibd, &iinitData, &mIB);
	}

	void ComputeTangentBasis(
		const D3DXVECTOR3& P1, const D3DXVECTOR3& P2, const D3DXVECTOR3& P3, 
		const D3DXVECTOR2& UV1, const D3DXVECTOR2& UV2, const D3DXVECTOR2& UV3,
		D3DXVECTOR3 &normal, D3DXVECTOR3 &tangent, D3DXVECTOR3 &bitangent )
	{
		// TODO: Dati 3 vertici, calcolare vettore normale, tangente e bitangente
		D3DXVECTOR3 Edge1 = P2 - P1;
		D3DXVECTOR3 Edge2 = P3 - P1;
		D3DXVECTOR2 Edge1uv = UV2 - UV1;
		D3DXVECTOR2 Edge2uv = UV3 - UV1;

		D3DXVec3Cross(&normal, &Edge1, &Edge2);
		D3DXVec3Normalize(&normal, &normal);

		float cp = Edge1uv.y * Edge2uv.x - Edge1uv.x * Edge2uv.y; 
		if ( fabs(cp) > 0.000001 )
		{ 
			float mul = 1.0f / cp;
			tangent = (Edge1 * -Edge2uv.y + Edge2 * Edge1uv.y) * mul;
			bitangent = (Edge1 * -Edge2uv.x + Edge2 * Edge1uv.x) * mul;
			D3DXVec3Normalize(&tangent, &tangent);
			D3DXVec3Normalize(&bitangent, &bitangent);
		}
	}
};