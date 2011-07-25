#ifndef SHLighting_fxh__
#define SHLighting_fxh__

shared float3 L00;
shared float3 L1_1;
shared float3 L10;
shared float3 L11;
shared float3 L2_2;
shared float3 L2_1;
shared float3 L20;
shared float3 L21;
shared float3 L22;

const float c[5] =
{
	0.429043,
	0.511664,
	0.743125,
	0.886227,
	0.247708
};

float4 CalculateSHLighting(float4 normal)
{
	float4 color;
	for (int i=0; i<3; i++)
	{
		float4x4 M = float4x4(	c[0] * L22[i], c[0] * L2_2[i], c[0] * L21[i], c[1] * L11[i],
								c[0] * L2_2[i], -c[0] * L22[i], c[0] * L2_1[i], c[1] * L1_1[i],
								c[0] * L21[i], c[0] * L2_1[i], c[2] * L20[i], c[1] * L11[i],
								c[1] * L11[i], c[1] * L1_1[i], c[1] * L10[i], c[3] * L00[i] - c[4] * L20[i] );
								
		color[i] = mul( mul(normal, M), normal );
	}
	
	color.a = 1;
	return color;
}

#endif // SHLighting_fxh__
