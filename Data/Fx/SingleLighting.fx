
#include "Global.fxh"
#include "DepthMap.fxh"

shared float4x4 matLightSpace;
shared float4x4 matLightViewProjBias;

struct VS_OUTPUT
{
    float4	Position	: POSITION;
    float4	Diffuse		: COLOR0;
};

VS_OUTPUT SingleLightingVS( float4 vPosition : POSITION,
							float3 vNormal : NORMAL )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	
	Out.Position = mul(vPosition, matWorldViewProjection);
	
	float3 normal = mul(vNormal, (float3x3)matWorld);
	Out.Diffuse = float4((float3)dot(-vLightDir, normal), 1);
	
	return Out;
}

float4 SingleLightingPS( VS_OUTPUT In ) : COLOR
{
	return In.Diffuse;
}

// Shadowed shaders
texture	texLightSpaceDepth;

sampler samplerLightSpaceDepth = 
sampler_state
{
    Texture = <texLightSpaceDepth>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VS_SHADOW_OUTPUT
{
	float4	Position		: POSITION;
	float2	Texcoord		: TEXCOORD0;
	float4	LightSpacePos	: TEXCOORD1;
	float3	Normal			: TEXCOORD2;
	float4	ProjTexcoord	: TEXCOORD3;
};

VS_SHADOW_OUTPUT SingleShadowedVS( float4 vPosition : POSITION,
								   float2 vTexcoord : TEXCOORD0,
								   float3 vNormal : NORMAL )
{
	VS_SHADOW_OUTPUT Out = (VS_SHADOW_OUTPUT)0;
	
	Out.Position = mul(vPosition, matWorldViewProjection);
	Out.Texcoord = vTexcoord;
	Out.LightSpacePos = mul(vPosition, matLightSpace);
	
	Out.Normal = mul(vNormal, (float3x3)matWorld);
	
	Out.ProjTexcoord = mul(vPosition, matLightViewProjBias);
	
	return Out;
}

float4 SingleShadowedPS( VS_SHADOW_OUTPUT In ) : COLOR
{
	//return tex2Dproj(samplerLightSpaceDepth, In.ProjTexcoord);
		
	float depth = tex2Dproj(samplerLightSpaceDepth, In.ProjTexcoord).r;
	
	if (depth < In.LightSpacePos.z - 0.01)
		return float4((float3)abs(dot(vLightDir, normalize(In.Normal)) * 0.2), 1);
	
	return float4((float3)dot(-vLightDir, normalize(In.Normal)), 1);
}

technique Default
{
	pass p0
	{
		VertexShader = compile vs_2_0 SingleLightingVS();
		PixelShader  = compile ps_2_0 SingleLightingPS();
	}
}

technique DepthMap
{
	pass p0
	{
		VertexShader = compile vs_2_0 DepthMapVS();
		PixelShader  = compile ps_2_0 DepthMapPS();
	}
}

technique Shadowed
{
	pass p0
	{
		VertexShader = compile vs_2_0 SingleShadowedVS();
		PixelShader  = compile ps_2_0 SingleShadowedPS();
	}
}
