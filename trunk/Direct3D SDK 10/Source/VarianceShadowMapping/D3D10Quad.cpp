//----------------------------------------------------------------------------------
// File:   D3D10Quad.cpp
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

// This file contains the implementation of D3D10Quad.  sQuadWriter tesselates the quad
// to the level specified

#include "DXUT.h"
#include "D3D10Quad.h"

inline UINT vertPerSide( UINT iLevel )
{
    if( iLevel == 0 )
        return 2;
    else
        return vertPerSide( iLevel - 1 ) * 2 - 1;
}

struct QUAD_VERTEX
{
    D3DXVECTOR3 pos;
	D3DXVECTOR3 norm;
	D3DXVECTOR2 tex;
};

static void sQuadWriter( UINT iTessLevel, D3DXVECTOR3** pVerts, UINT** pIndices, UINT* pVertexCount, UINT* pIndexCount, const float fRandomPlace = 0.0f )
{

    UINT iDimension = vertPerSide( iTessLevel );
    float fTest = (float)( iDimension - 1 );
    UINT iTempIndexCount = (iDimension * 2 + 4 ) * ( iDimension - 1);
    UINT iTempVertexCount = iDimension * iDimension;

    D3DXVECTOR3* pVertTemp    = pVerts    ? new D3DXVECTOR3[ iTempVertexCount ]    : NULL;
    UINT* pIndexTemp        = pIndices    ? new UINT[ iTempIndexCount ]            : NULL;

    //srand( time( NULL ) );

    if( pVertTemp )
    {
        for( UINT i = 0; i < iDimension * iDimension; ++i )
        {
            UINT u = i % iDimension;
            UINT v = ( i - u ) / iDimension;

            float fOffset = 0.0f;
            if( fRandomPlace != 0.0f )
            {
                fOffset = ( ((float)rand() / (float)RAND_MAX ) * 2.0f * fRandomPlace ) - fRandomPlace;// ( rand() * 100 ) % iRandomPlace );
            }

            pVertTemp[ i ] = D3DXVECTOR3( ((float)u / fTest) * 2.0f - 1.0f, ((float)v / fTest) * -2.0f + 1.0f, fOffset );
        }

        *pVerts = pVertTemp;
    }

    if( pIndexTemp )
    {
        UINT vertoffset = 0;
        UINT index = 0;

        while( index < iTempIndexCount )
        {
            UINT offset = 0;
            UINT top = vertoffset;
            UINT bottom = vertoffset;

            //For stiching strips
            pIndexTemp[ index++ ] = top;
            pIndexTemp[ index++ ] = top;

            for(UINT i = vertoffset; i < vertoffset + iDimension; ++ i )
            {
                top = i;
                bottom = iDimension + i;

                pIndexTemp[ index++ ] = top;
                pIndexTemp[ index++ ] = bottom;
                ++offset;
            }

            //For stiching strips
            pIndexTemp[ index++ ] = bottom;
            pIndexTemp[ index++ ] = bottom;

            vertoffset += iDimension;// * 2;
        }

        *pIndices = pIndexTemp;
    }

    if( pVertexCount )
        *pVertexCount = iTempVertexCount;

    if( pIndexCount )
        *pIndexCount = iTempIndexCount;
}


D3D10Quad::D3D10Quad( ID3D10Device* pD3D10Device, UINT iTessLevel )
{
    m_pVB = NULL;
    m_pIB = NULL;
    m_iIndexCount = 0;
    m_VertCount = 0;

    D3DXVECTOR3* pTessed;
    UINT* pIndices;
    sQuadWriter( iTessLevel, &pTessed, &pIndices, &m_VertCount, &m_iIndexCount );

    QUAD_VERTEX* pQuadData;
    pQuadData = new QUAD_VERTEX[m_VertCount];
    for( UINT i = 0; i < m_VertCount; ++i )
    {
        pQuadData[i].pos = pTessed[i];
		pQuadData[i].tex.x = pTessed[i].x * 0.5f + 0.5f;
		pQuadData[i].tex.y = pTessed[i].y * -0.5f + 0.5f;
        pQuadData[i].norm = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    }

    D3D10_SUBRESOURCE_DATA DataUp;
    D3D10_BUFFER_DESC Desc;

    //Creating buffers
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;
    Desc.Usage = D3D10_USAGE_IMMUTABLE;

    //Create Vertex buffer
    {
        Desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
        Desc.ByteWidth = sizeof( QUAD_VERTEX ) * m_VertCount;
        DataUp.pSysMem = pQuadData;

        pD3D10Device->CreateBuffer( &Desc, &DataUp, &m_pVB );
    }

    //Create index buffer
    {
        Desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
        Desc.ByteWidth = m_iIndexCount * sizeof(UINT);
        DataUp.pSysMem = pIndices;

        pD3D10Device->CreateBuffer( &Desc, &DataUp, &m_pIB );
    }

    //Delete system copies
    SAFE_DELETE( pQuadData );
    SAFE_DELETE( pIndices );
    SAFE_DELETE( pTessed );
}

void D3D10Quad::Render(ID3D10Device* pD3D10Device, ID3D10EffectTechnique* pTechnique)
{
    UINT offsets = 0;
    UINT VBstride[] = { sizeof( QUAD_VERTEX ) };

    pD3D10Device->IASetVertexBuffers( 0, 1, &m_pVB, VBstride, &offsets);
    pD3D10Device->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R32_UINT, offsets );
    pD3D10Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    D3D10_TECHNIQUE_DESC techDesc;
    pTechnique->GetDesc( &techDesc );


    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pTechnique->GetPassByIndex(p)->Apply(0);
        pD3D10Device->DrawIndexed( m_iIndexCount, 0, 0);
    }
}