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
bool  EnableLighting = true;
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
// Texture diffusiva

// Stato Sampling bilineare per texture
SamplerState samLinear
{
	// TODO: Definire un filtraggio trilineare
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
}
;
// Proprietà Sorgente luminosa puntiforme
struct Light
{
	float4 Position;
	// Posizione luce
	float4 Color;
	// Colore della luce
	float4 Ambient;
	// Valore del colore ambientale
	float3 Attenuation;
	// Coefficienti di attenuazione proporzionale alla distanza punto-luce (k0+k1*d+k2*d*d)
};
// Array contenente le luci della scena 
Light    g_lights[2];
// Materiale oggetto caricato
Material g_material;
// Strutture Input e Output per i Vertex shaders
struct VS_INPUT
{
	float4 pos          : POSITION;
	//posizione
	float3 norm         : NORMAL;
	//normale
	float2 tex : TEXCOORD0;
	// Coordinate texture
};
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
}
;
// Nel vertex shader phong dobbiamo anche passare al rasterizer normali e posizioni in coordinate mondo
struct VS_OUTPUTPhong
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
	float3 norm: TEXCOORD1;
	float3 worldpos: TEXCOORD2;
}
;
// Stato Z-Buffer Abilitato
DepthStencilState EnableDepth{
	DepthEnable = true;
	DepthWriteMask = ALL;
	// Può essere ALL o ZERO
	DepthFunc = Less;
	// Può essere Less Greater Less_Equal ...
	// Setup stencil states 
	StencilEnable = false;
}
;
// Funzione per calcolare illuminazione da luci puntiformi con modello di phong
float4 CalcLighting( float3 worldNormal, float3 worldPos, float3 cameraDir, Material mat )
{
	// colore di output
	float4 output = float4(0,0,0,0);
	for(int i=0; i<2; i++)
	{
		// Vettore luce-punto
		float3 toLight = g_lights[i].Position.xyz - worldPos;
		// Distanza dalla luce
		float lightDist = length( toLight );
		// Coefficiente di attenuazione
		float fAtten = 1.0/dot( g_lights[i].Attenuation, float3(1,lightDist,lightDist*lightDist) );
		// Versore luce-punto normalizzato
		float3 lightDir = normalize( toLight );
		// Vettore halfangle
		float3 halfAngle = normalize( (cameraDir + lightDir) / 2 );
		// Componente diffusiva
		output += max(0,dot( lightDir, worldNormal ) * g_lights[i].Color * mat.Kd * fAtten ) ;
		// Componente ambientale
		output += g_lights[i].Ambient * mat.Ka;
		// Componente speculare
		output += max(0,pow( abs(dot( halfAngle, worldNormal )), mat.Shininess ) * g_lights[i].Color * mat.Ks * fAtten );
		// Componente emissiva
		output += mat.Ke;
	}

		
	return output;
}



//--------------------------------------------------------------------------------------
// Vertex Shader - Gourad shading: illuminazione per vertice
//--------------------------------------------------------------------------------------
VS_OUTPUT VSGourad( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 worldPos = mul( input.pos, World );
	float4 cameraPos = mul( worldPos, View );
	output.pos = mul( cameraPos, Projection );
	float3 worldNormal = normalize( mul( input.norm, (float3x3)World ) );
	// Calcola vettore punto-telecamera
	float3 cameraDir = normalize(CameraPosition - worldPos.xyz);
	if(EnableLighting){
		output.color = CalcLighting( worldNormal, worldPos.xyz, cameraDir.xyz, g_material );
	}

	else{
		output.color = DefaultColor;
	}
		
	
	output.tex = input.tex;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader - Gourad shading: illuminazione per vertice
//--------------------------------------------------------------------------------------
float4 PSGourad(VS_OUTPUT input ) : SV_Target
{
	if(EnableTexture)
		input.color = input.color * txDiffuse.Sample(samLinear,input.tex);
	// TODO: Campionare texture e modulare con il colore di illuminazione

	return input.color;
}



//--------------------------------------------------------------------------------------
// Vertex Shader - Phong shading: illuminazione per pixel
//--------------------------------------------------------------------------------------
VS_OUTPUTPhong VSPhong( VS_INPUT input )
{
	VS_OUTPUTPhong output = (VS_OUTPUTPhong)0;
	float4 worldPos = mul( input.pos, World );
	float4 cameraPos = mul( worldPos, View );
	output.pos = mul( cameraPos, Projection );
	float3 worldNormal = normalize( mul( input.norm, (float3x3)World ) );
	output.norm = worldNormal;
	output.worldpos = worldPos.xyz;
	output.color = DefaultColor;
	// il colore sarà calcolato nel pixel shader    
	output.tex = input.tex;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader - Phong shading: illuminazione per pixel
//--------------------------------------------------------------------------------------
float4 PSPhong( VS_OUTPUTPhong input ) : SV_Target
{
	// Rinormalizza le normali
	input.norm = normalize(input.norm);
	// Calcola vettore punto-telecamera
	float3 cameraDir = normalize(CameraPosition - input.worldpos.xyz);
	float4 color;
	if(EnableLighting){
		color = CalcLighting( input.norm, input.worldpos.xyz, cameraDir, g_material );
	}

	else{
		color = input.color;
	}
		

	if(EnableTexture)
		input.color = input.color * txDiffuse.Sample(samLinear,input.tex);
	// TODO: Campionare texture e modulare con il colore di illuminazione
		
	return color;
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

//--------------------------------------------------------------------------------------
technique11 PhongShading
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VSPhong() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PSPhong() ) );
		SetDepthStencilState(EnableDepth,0);
	}
}
