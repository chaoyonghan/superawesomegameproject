#pragma once
//----------------------------------------------------------------------------------
// File:   NVUTDXGIFormatHelper.h
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


/*
    Just provides some handy byte sizes for use in creation of vertex structures.
*/
class NVUTDXGIFormatHelper
{
public:
	static int GetElementSizeBytes(unsigned int format)
	{
		switch(format)
		{
		case DXGI_FORMAT_UNKNOWN : 
			return -1;
		case DXGI_FORMAT_R32G32B32A32_TYPELESS : 
		case DXGI_FORMAT_R32G32B32A32_FLOAT : 
		case DXGI_FORMAT_R32G32B32A32_UINT : 
		case DXGI_FORMAT_R32G32B32A32_SINT : 
			return 16;
		case DXGI_FORMAT_R32G32B32_TYPELESS : 
		case DXGI_FORMAT_R32G32B32_FLOAT : 
		case DXGI_FORMAT_R32G32B32_UINT : 
		case DXGI_FORMAT_R32G32B32_SINT : 
			return 12;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS : 
		case DXGI_FORMAT_R16G16B16A16_FLOAT : 
		case DXGI_FORMAT_R16G16B16A16_UNORM : 
		case DXGI_FORMAT_R16G16B16A16_UINT : 
		case DXGI_FORMAT_R16G16B16A16_SNORM : 
		case DXGI_FORMAT_R16G16B16A16_SINT : 
		case DXGI_FORMAT_R32G32_TYPELESS : 
		case DXGI_FORMAT_R32G32_FLOAT : 
		case DXGI_FORMAT_R32G32_UINT : 
		case DXGI_FORMAT_R32G32_SINT : 
		case DXGI_FORMAT_R32G8X24_TYPELESS : 
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT : 
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : 
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT : 
			return 8;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS : 
		case DXGI_FORMAT_R10G10B10A2_UNORM : 
		case DXGI_FORMAT_R10G10B10A2_UINT : 
		case DXGI_FORMAT_R11G11B10_FLOAT : 
		case DXGI_FORMAT_R8G8B8A8_TYPELESS : 
		case DXGI_FORMAT_R8G8B8A8_UNORM : 
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : 
		case DXGI_FORMAT_R8G8B8A8_UINT : 
		case DXGI_FORMAT_R8G8B8A8_SNORM : 
		case DXGI_FORMAT_R8G8B8A8_SINT : 
		case DXGI_FORMAT_R16G16_TYPELESS : 
		case DXGI_FORMAT_R16G16_FLOAT : 
		case DXGI_FORMAT_R16G16_UNORM : 
		case DXGI_FORMAT_R16G16_UINT : 
		case DXGI_FORMAT_R16G16_SNORM : 
		case DXGI_FORMAT_R16G16_SINT : 
		case DXGI_FORMAT_R32_TYPELESS : 
		case DXGI_FORMAT_D32_FLOAT : 
		case DXGI_FORMAT_R32_FLOAT : 
		case DXGI_FORMAT_R32_UINT : 
		case DXGI_FORMAT_R32_SINT : 
		case DXGI_FORMAT_R24G8_TYPELESS : 
		case DXGI_FORMAT_D24_UNORM_S8_UINT : 
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS : 
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT : 
			return 4;
		case DXGI_FORMAT_R8G8_TYPELESS : 
		case DXGI_FORMAT_R8G8_UNORM : 
		case DXGI_FORMAT_R8G8_UINT : 
		case DXGI_FORMAT_R8G8_SNORM : 
		case DXGI_FORMAT_R8G8_SINT : 
		case DXGI_FORMAT_R16_TYPELESS : 
		case DXGI_FORMAT_R16_FLOAT : 
		case DXGI_FORMAT_D16_UNORM : 
		case DXGI_FORMAT_R16_UNORM : 
		case DXGI_FORMAT_R16_UINT : 
		case DXGI_FORMAT_R16_SNORM : 
		case DXGI_FORMAT_R16_SINT : 
			return 2;
		case DXGI_FORMAT_R8_TYPELESS : 
		case DXGI_FORMAT_R8_UNORM : 
		case DXGI_FORMAT_R8_UINT : 
		case DXGI_FORMAT_R8_SNORM : 
		case DXGI_FORMAT_R8_SINT : 
		case DXGI_FORMAT_A8_UNORM : 
			return 1;
		case DXGI_FORMAT_R1_UNORM : 
			return -1;
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP : 
		case DXGI_FORMAT_R8G8_B8G8_UNORM : 
		case DXGI_FORMAT_G8R8_G8B8_UNORM : 
			return 4;
		case DXGI_FORMAT_BC1_TYPELESS : 
		case DXGI_FORMAT_BC1_UNORM : 
		case DXGI_FORMAT_BC1_UNORM_SRGB : 
		case DXGI_FORMAT_BC2_TYPELESS : 
		case DXGI_FORMAT_BC2_UNORM : 
		case DXGI_FORMAT_BC2_UNORM_SRGB : 
		case DXGI_FORMAT_BC3_TYPELESS : 
		case DXGI_FORMAT_BC3_UNORM : 
		case DXGI_FORMAT_BC3_UNORM_SRGB : 
		case DXGI_FORMAT_BC4_TYPELESS : 
		case DXGI_FORMAT_BC4_UNORM : 
		case DXGI_FORMAT_BC4_SNORM : 
		case DXGI_FORMAT_BC5_TYPELESS : 
		case DXGI_FORMAT_BC5_UNORM : 
		case DXGI_FORMAT_BC5_SNORM : 
			return -1;
		case DXGI_FORMAT_B5G6R5_UNORM : 
		case DXGI_FORMAT_B5G5R5A1_UNORM : 
			return 2;
		case DXGI_FORMAT_B8G8R8A8_UNORM : 
		case DXGI_FORMAT_B8G8R8X8_UNORM : 
			return 4;
		case DXGI_FORMAT_FORCE_UINT : 
		default:
			return -1;


		}
	}
};