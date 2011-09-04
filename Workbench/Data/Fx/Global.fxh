#ifndef Global_fxh__
#define Global_fxh__

shared float4x4		matView;
shared float4x4		matProjection;
shared float4x4		matViewProjection;

shared float4x4		matWorld;
shared float4x4		matWorldViewProjection;
shared float4		vLightDir;

shared float3		vEyePosWorld;

#endif // Global_fxh__