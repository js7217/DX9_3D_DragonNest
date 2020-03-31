#include "Shader_Base.hpp"




struct VS_IN
{
	float3	vPosition : POSITION;
	float3	vTexUV : TEXCOORD;
};

struct VS_OUT
{
	float4	vPosition : POSITION;
	float3	vTexUV : TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4 vPosition : POSITION;
	float3 vTexUV : TEXCOORD;
};

vector PS_MAIN(PS_IN In) : COLOR
{
	vector vColor = (vector)0;
	
	vColor = texCUBE(BaseSampler, In.vTexUV);
	return vColor;
}

technique Default_Device
{
	pass SkyBox
	{
		ZEnable = false;
		ZWriteEnable = false;

		CullMode = cw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}

