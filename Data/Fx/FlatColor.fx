
#include "Global.fxh"

struct VS_OUTPUT
{
    float4	Position	: POSITION;
};

VS_OUTPUT FlatColorVS( float4 vPosition : POSITION )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	
	Out.Position = mul(vPosition, matWorldViewProjection);
	
	return Out;
}

float4 FlatColorPS( VS_OUTPUT In ) : COLOR
{
	return float4(0, 1, 0, 1);
}

technique Default
{
	pass p0
	{
		VertexShader = compile vs_2_0 FlatColorVS();
		PixelShader  = compile ps_2_0 FlatColorPS();
	}
}

