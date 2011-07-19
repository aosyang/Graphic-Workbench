#ifndef DepthMap_fxh__
#define DepthMap_fxh__

#include "Global.fxh"

struct VS_DEPTH_OUTPUT
{
	float4	Position	: POSITION;
	float2	Depth		: TEXCOORD0;
};

VS_DEPTH_OUTPUT DepthMapVS( float4 vPosition : POSITION )
{
	VS_DEPTH_OUTPUT Out = (VS_DEPTH_OUTPUT)0;

	Out.Position	= mul(vPosition, matWorldViewProjection);
	Out.Depth.xy	= Out.Position.zw;

	return Out;
}

float4 DepthMapPS( VS_DEPTH_OUTPUT In ) : COLOR
{
	float depth = In.Depth.x / In.Depth.y;
	return float4( depth, depth, depth, 1.0f );
}

#endif // DepthMap_fxh__
