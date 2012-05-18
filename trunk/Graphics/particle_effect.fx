struct Material
{
	float4 Ks;
	// Coefficiente di riflessione speculare
	float4 Ka;
	// Coefficiente di riflessione ambientale
	float4 Kd;
	// Coefficiente di riflessione diffusiva
	float4 Ke;
	// Coefficiente di riflessione emissiva
	float Shininess;
	// Esponente di shininess
};

cbuffer cbPerFrame
{
	matrix World;
	matrix View;
	matrix Projection;
	float3 CameraPosition;


	float4 gEmitPosW;
	float4 gEmitDirW;

	float gGameTime;
	float gTimeStep;
};

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	float3 gAccelW = {3.9f, 3.9f, 0.0f};
	
	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};

	float maxAge = 2.0f;

	float4 g_directional1 = float4( 0.992, 1.0, 0.880, 0.0 );
	float4 g_directional2 = float4( 0.595, 0.6, 0.528, 0.0 );
	float4 g_ambient = float4(0.525,0.474,0.474,0.0);
};

Material g_material;

Texture2DArray gTexArray;

Texture2D g_txColorGradient;
Texture3D g_txVolumeDiff;

// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex;

#define PT_EMITTER 0
#define PT_FLARE 1
 
SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

struct VS_INPUT
{
	float3 initialPosW : POSITION;
	float3 initialVelW : VELOCITY;
	float2 sizeW       : SIZE;
	float age          : AGE;
	float3 tex		   : SLICE;
	uint type          : TYPE;
	
};

//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(gTriLinearSam, u, 0);
	
	// project onto unit sphere
	return normalize(v);
}
 
//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_EMITTER 0
#define PT_FLARE 1
 

VS_INPUT StreamOutVS(VS_INPUT vIn)
{
	return vIn;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(11)]
void StreamOutGS(point VS_INPUT gIn[1], inout PointStream<VS_INPUT> ptStream)
{	
	gIn[0].age += gTimeStep;
	
	if( gIn[0].type == PT_EMITTER )
	{	
		// time to emit a new particle?
		if( gIn[0].age > 0.00005f )
		{
			float3 vRandom = RandUnitVec3(0.0f);
			VS_INPUT p;
			[unroll]
			for(float i=0.0f;i<1.0f;++i)
			{
				
				vRandom.x *= 0.5f;
				vRandom.z *= 0.5f;

				//vRandom.x = 0.1f;
				//vRandom.z = 0.1f;

				
				p.initialPosW = float3(gEmitPosW.x,gEmitPosW.y-(9.0f-i)/9.0f,gEmitPosW.z);
				p.initialVelW = 4.0f*vRandom;
				p.sizeW       = float2(3.0f, 3.0f);
				p.age         = 0;
				p.type        = PT_FLARE;
				p.tex         = float3(0.0f,0.0f,0.0f);

				ptStream.Append(p);
			}
			// reset the time to emit
			gIn[0].age = 0.0f;
		}
		
		// always keep emitters
		ptStream.Append(gIn[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if( gIn[0].age <= maxAge )
			ptStream.Append(gIn[0]);
	}		
}
	
technique10 StreamOutTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, StreamOutVS() ) );
		SetGeometryShader(ConstructGSWithSO( CompileShader( gs_4_0, StreamOutGS() ), "POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; SLICE.xyz; TYPE.x;" ));
		
		// disable pixel shader for stream-out only
		SetPixelShader(NULL);
		
		// we must also disable the depth buffer for stream-out only
		SetDepthStencilState( DisableDepth, 0 );
	}
}

struct VS_OUT
{
	float3 posW  : POSITION;
	float2 sizeW : SIZE;
	float4 color : COLOR;
	float life  : AGE;
	uint   type  : TYPE;
};

SamplerState g_samLinearClamp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

VS_OUT DrawVS(VS_INPUT vIn)
{
	VS_OUT vOut;
	
	float t = vIn.age;
	
	// constant acceleration equation
	vOut.posW = 0.5f*t*t*gAccelW + t*vIn.initialVelW + vIn.initialPosW;
	
	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, maxAge, t);
	vOut.color = g_txColorGradient.SampleLevel( g_samLinearClamp, float2(smoothstep(0.0f, maxAge, t),0), 0 );
	float4 Light = g_directional1 + g_ambient;
	vOut.color = float4((Light.xyz*vOut.color.rgb), opacity);
	
	vOut.sizeW = vIn.sizeW;
	vOut.type  = vIn.type;
	vOut.life = vIn.age;
	
	vOut.posW = mul(vOut.posW,World);
	vOut.posW = mul(vOut.posW,View);
	vOut.posW = mul(vOut.posW,Projection);

	return vOut;
}

struct GS_OUT
{
	float4 posH  : SV_Position;
	float4 color : COLOR;
	float3 texC  : TEXCOORD;
};

// The draw GS just expands points into camera facing quads.
[maxvertexcount(25)]
void DrawGS(point VS_OUT gIn[1], 
			inout TriangleStream<GS_OUT> triStream)
{	
// do not draw emitter particles.
	//
	// Compute world matrix so that billboard faces the camera.
	//
	float3 look  = normalize(CameraPosition - gIn[0].posW);
	float3 right = normalize(cross(float3(0,1,0), look));
	float3 up    = cross(look, right);
		
	matrix W;
	W[0] = float4(right,       0.0f);
	W[1] = float4(up,          0.0f);
	W[2] = float4(look,        0.0f);
	W[3] = float4(gIn[0].posW, 1.0f);
		
	//
	// Compute 4 triangle strip vertices (quad) in local space.
	// The quad faces down the +z axis in local space.
	//
	float halfWidth  = 0.5f*gIn[0].sizeW.x;
	float halfHeight = 0.5f*gIn[0].sizeW.y;
	float r = halfWidth;
	float4 v[4];
	v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
	v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
	v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
	v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);
		
	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
	GS_OUT gOut;
	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		gOut.posH = mul(v[i],W);
		gOut.posH= mul(gOut.posH,View);
		gOut.posH = mul(gOut.posH,Projection);
		gOut.texC  = float3(gQuadTexC[i],smoothstep(0.0f,maxAge,gIn[0].life));
		gOut.color = gIn[0].color;
		triStream.Append(gOut);
	}
}

SamplerState g_samVolume
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

float4 DrawPS(GS_OUT pIn) : SV_TARGET
{
	return pIn.color*g_txVolumeDiff.Sample( g_samVolume, pIn.texC);
}

technique11 DrawTech
{
	pass P0
	{
		SetVertexShader(   CompileShader( vs_4_0, DrawVS() ) );
		SetGeometryShader( CompileShader( gs_4_0, DrawGS() ) );
		SetPixelShader(    CompileShader( ps_4_0, DrawPS() ) );
		
		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState( NoDepthWrites, 0 );
	}
}