//--------------------------------------------------------------------------------------
// basic_lighting_effect.fx
//
// Copyright (c) Roberto Toldo 2010. Do not distribute. (Master Game Development - University of Verona)
//--------------------------------------------------------------------------------------

//Definizione variabili e strutture
matrix World;
matrix View;
matrix Projection;
float3 CameraPosition;
float4 DefaultColor;
float4 gDirToSunW;

bool  EnableTexture = true;
// Material
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

Texture2D txDiffuse;
Texture2D gLayer0;
Texture2D gLayer1;
Texture2D gLayer2;
Texture2D gLayer3;
Texture2D gLayer4;

Texture2D gLayer0N;
Texture2D gLayer1N;
Texture2D gLayer2N;
Texture2D gLayer3N;
Texture2D gLayer4N;

Texture2D gBlendMap;
// Texture diffusiva

// Stato Sampling bilineare per texture
SamplerState samLinear
{
	// TODO: Definire un filtraggio trilineare
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

// Materiale oggetto caricato
Material g_material;

DepthStencilState EnableDepth
{
	DepthEnable = true; 
	DepthWriteMask = ALL;  // Può essere ALL o ZERO
	DepthFunc = Less;  // Può essere Less Greater Less_Equal ...
	// Setup stencil states 
	StencilEnable = false; 
};

struct Light
{
	float4 Position; // Posizione luce
	float4 Color; // Colore della luce
	float4 Ambient;  // Valore del colore ambientale
	float3 Attenuation; // Coefficienti di attenuazione proporzionale alla distanza punto-luce (k0+k1*d+k2*d*d)
};

// Array contenente le luci della scena 
Light    g_lights[2];

// Strutture Input e Output per i Vertex shaders
struct VS_INPUT
{
	float4 pos    : POSITION;//posizione
	float3 norm   : NORMAL;//normale
	float3 tan    : TANGENT;//tangente
	float3 binorm : BINORMAL;//binormale
	float2 tex    : TEXCOORD0;// Coordinate texture
};

struct VS_OUTPUT
{
	float4 pos   : SV_POSITION;
	float4 color : COLOR0;
	float shade  : SHADE;
	float2 tex   : TEXCOORD0;
	float2 stretchedUV  : TEXCOORD1;
	float3 light0: TEXCOORD2;
	float3 light1: TEXCOORD3;
	float3 view  : TEXCOORD4;
	float3 normal: TEXCOORD5;
};

//--------------------------------------------------------------------------------------
// Vertex Shader - Gourad shading: illuminazione per vertice
//--------------------------------------------------------------------------------------
VS_OUTPUT VSGourad( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 worldPos = mul( input.pos, World );
	float4 cameraPos = mul( worldPos, View );
	output.pos = mul( cameraPos, Projection );
	float3 worldNormal = mul( input.norm, (float3x3)World );
	// Calcola vettore punto-telecamera
	float3 cameraDir = normalize(CameraPosition - worldPos.xyz);

	output.light0.xyz = (g_lights[0].Position.xyz - worldPos.xyz );   	
	output.light1.xyz = (g_lights[1].Position.xyz - worldPos.xyz ); 

	output.view = (CameraPosition.xyz - worldPos.xyz);

	output.color = DefaultColor;
	output.shade = saturate(max(dot(worldNormal, gDirToSunW), 0.0f) + 0.1f);
	output.tex = 20.0f*input.tex;
	output.stretchedUV = input.tex;

	float3x3 tanMatrix = { input.tan,input.binorm,input.norm };
	output.light0 = mul ( output.light0, tanMatrix );
	output.light1 = mul ( output.light1, tanMatrix );
	output.view = mul ( output.view, tanMatrix );
	output.normal = input.norm;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader - Gourad shading: illuminazione per vertice
//--------------------------------------------------------------------------------------
float4 PSGourad(VS_OUTPUT input ) : SV_Target
{
	//input.color = input.color * txDiffuse.Sample(samLinear,input.tex);
	input.light0 = normalize(input.light0);
	input.light1 = normalize(input.light1);
	input.view = normalize(input.view);
	input.normal = normalize(input.normal);

	// Se il bump mapping è abilitato assegna valori da texture, altrimenti da normali rasterizzate
	//TODO: Campiona normale dalla texture

	float4 c0 = gLayer0.Sample(samLinear,input.tex);
	float4 c1 = gLayer1.Sample(samLinear,input.tex);
	float4 c2 = gLayer2.Sample(samLinear,input.tex);
	float4 c3 = gLayer3.Sample(samLinear,input.tex);
	float4 c4 = gLayer4.Sample(samLinear,input.tex); 

	float4 cN0 = 2 * (gLayer0N.Sample(samLinear,input.tex) - 0.5);
	float4 cN1 = 2 * (gLayer1N.Sample(samLinear,input.tex) - 0.5);
	float4 cN2 = 2 * (gLayer2N.Sample(samLinear,input.tex) - 0.5);
	float4 cN3 = 2 * (gLayer3N.Sample(samLinear,input.tex) - 0.5);
	float4 cN4 = 2 * (gLayer4N.Sample(samLinear,input.tex) - 0.5);

	float4 t = gBlendMap.Sample(samLinear,input.stretchedUV);
	float4 C = c0;
	C = lerp(C, c1, t.r);
	C = lerp(C, c2, t.g);
	C = lerp(C, c3, t.b);
	C = lerp(C, c4, t.a);
	C *= input.shade;
	float4 color = C;
	float4 normal = cN0;
	normal = lerp(normal, cN1, t.r);
	normal = lerp(normal, cN2, t.g);
	normal = lerp(normal, cN3, t.b);
	normal = lerp(normal, cN4, t.a);

	//return C;
	float4 output; 
	output.rgba = 0;
	// Vettore halfangle
	float3 halfAngle0 = normalize( (input.view + input.light0) / 2 );
	float3 halfAngle1 = normalize( (input.view + input.light1) / 2 );

	// Componente diffusiva
	output += max(0,dot( input.light0, normal ) * g_lights[0].Color * color ) ;
	output += max(0,dot( input.light1, normal ) * g_lights[1].Color * color ) ;

	// Componente ambientale
	output += g_lights[0].Ambient * color;
	output += g_lights[1].Ambient * color;

	// Componente speculare
	output += max(0,pow( dot( halfAngle0, normal ), g_material.Shininess ) * g_lights[0].Color * g_material.Ks );
	output += max(0,pow( dot( halfAngle1, normal ), g_material.Shininess ) * g_lights[1].Color * g_material.Ks );

	// Componente emissiva
	output += g_material.Ke;

	return output;
}

//--------------------------------------------------------------------------------------
technique11 GouradShading
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VSGourad() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSGourad() ) );
		SetDepthStencilState(EnableDepth,0);
	}
}
