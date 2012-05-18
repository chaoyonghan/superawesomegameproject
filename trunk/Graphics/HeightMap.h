#pragma once
#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTgui.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "d3dx11effect.h"
#include <vector>
#include <fstream>
#include <sstream>

class HeightMap
{
public:

	HeightMap(float iNumRows,float iNumCols, std::wstring HeightmapFilename
		, float iHeightScale, float iHeightOffset)
		:mHeight(iNumRows)
		,mWidth(iNumCols)
	{
		// A height for each vertex
		std::vector<unsigned char> in( mHeight*mWidth );

		// Open the file.
		std::ifstream inFile;
		inFile.open(HeightmapFilename.c_str(), std::ios_base::binary);

		if(inFile)
		{
			// Read the RAW bytes.
			inFile.read((char*)&in[0], (std::streamsize)in.size());

			// Done with file.
			inFile.close();
		}

		// Copy the array data into a float array, and scale and offset the heights.
		mHeightMap.resize( mHeight*mWidth , 0);
		for(UINT i = 0; i <  mHeight*mWidth ; ++i)
		{
			mHeightMap[i] = (float)in[i] * iHeightScale + iHeightOffset;
		}
		smooth();
	}

	float getHeight()
	{
		return mHeight;
	}

	float getWidth()
	{
		return mWidth;
	}

	void smooth()
	{
		std::vector<float> dest( mHeightMap.size(),0 );

		for(UINT i = 0; i < mHeight; ++i)
		{
			for(UINT j = 0; j < mWidth; ++j)
			{
				dest[i*mWidth+j] = average(i,j);
			}
		}

		// Replace the old heightmap with the filtered one.
		mHeightMap = dest;
	}

	bool inBounds(UINT i, UINT j)
	{
		// True if ij are valid indices; false otherwise.
		return 
			i >= 0 && i < mHeight && 
			j >= 0 && j < mWidth;
	}

	float average(UINT i, UINT j)
	{
		// Function computes the average height of the ij element.
		// It averages itself with its eight neighbor pixels.  Note
		// that if a pixel is missing neighbor, we just don't include it
		// in the average--that is, edge pixels don't have a neighbor pixel.
		//
		// ----------
		// | 1| 2| 3|
		// ----------
		// |4 |ij| 6|
		// ----------
		// | 7| 8| 9|
		// ----------

		float avg = 0.0f;
		float num = 0.0f;

		for(UINT m= i-1; m <= i+1; ++m)
		{
			for(UINT n = j-1; n <= j+1; ++n)
			{
				if( inBounds(m,n) )
				{
					avg += mHeightMap[m*mWidth + n];
					num += 1.0f;
				}
			}
		}

		/*if(avg == 0)
			return 0;*/
		return avg / num;
	}

	float& operator [](const int& b)
	{
		return mHeightMap[b];
	}

protected:

private:

	float mHeight;
	float mWidth;
	std::vector<float> mHeightMap;

};