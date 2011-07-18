
#include "Global.fxh"
#include "DepthMap.fxh"

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