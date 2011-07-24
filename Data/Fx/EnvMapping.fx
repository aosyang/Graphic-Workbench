
#include "Global.fxh"

texture	texEnv;

sampler samplerEnv = 
sampler_state
{
    Texture = <texEnv>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

struct VS_OUTPUT
{
    float4	Position	: POSITION;
    float4	Diffuse		: COLOR0;
    float3	Normal		: TEXCOORD0;
    float3	Reflect		: TEXCOORD1;
    //float4	WorldPos	: TEXCOORD2;
};

VS_OUTPUT EnvReflectiveVS( float4 vPosition : POSITION,
							float3 vNormal : NORMAL )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float3 worldPos = mul(vPosition, matWorld);

	Out.Position = mul(vPosition, matWorldViewProjection);
	Out.Diffuse = float4(0, 1, 1, 1);
	Out.Normal = mul(vNormal, (float3x3)matWorld);
	Out.Reflect = reflect(worldPos - vEyePosWorld, Out.Normal);
	//Out.WorldPos = worldPos;
	
	return Out;
}

float4 EnvReflectivePS( VS_OUTPUT In ) : COLOR
{
	// Reflective cube map
	return texCUBE(samplerEnv, In.Reflect);
	
	// Environmental map
	//return texCUBE(samplerEnv, In.Normal);
	
	// Per-pixel env map
	//float3 Reflect = reflect(In.WorldPos - vEyePosWorld, In.Normal);
	//return texCUBE(samplerEnv, Reflect);

}


technique Default
{
	pass p0
	{
		VertexShader = compile vs_2_0 EnvReflectiveVS();
		PixelShader  = compile ps_2_0 EnvReflectivePS();
	}
}