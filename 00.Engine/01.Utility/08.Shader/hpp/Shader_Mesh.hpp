#include "Shader_Base.hpp"

float		g_fFar = 500.f;

struct	VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_IN_SHADOW
{
	float3 vPosition : POSITION;
};

struct VS_OUT
{
	vector vPosition : POSITION;
	vector vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	vector vProjPos : TEXCOORD1;
};

struct VS_OUT_SHADOW
{
	vector vPosition : POSITION;
	vector vProjPos : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix	matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	vector		vWorldNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));

	Out.vNormal = vWorldNormal;

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN_SHADOW In)
{
	VS_OUT_SHADOW			Out = (VS_OUT_SHADOW)0;

	matrix	matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	vector vPosition : POSITION;
	vector vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	vector vProjPos : TEXCOORD1;
};

struct PS_IN_SHADOW
{
	vector vPosition : POSITION;
	vector vProjPos : TEXCOORD0;
};

struct PS_OUT
{
	vector	vDiffuse : COLOR0;
	vector	vNormal : COLOR1;
	vector	vDepth : COLOR2;
};

struct PS_OUT_SHADOW
{
	vector	vShadow : COLOR0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

//	Out.vDiffuse.a = 1.f;
	return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
	PS_OUT_SHADOW			Out = (PS_OUT_SHADOW)0;

	Out.vShadow = vector(In.vProjPos.w / g_fFar, 0.f, 0.f, 1.f);

	return Out;
}


technique Default_Device		// 작성한 쉐이더 코드를 장치에게 명시적으로 알려주는 구문
{
	// PASS : 기능의 캡슐화
	pass
	{	
		alphatestenable = true;
		alphafunc = greater;
		alpharef = 0;
	//	cullmode = none;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass CullMode_None
	{
		cullmode = none;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass Shadow
	{
		alphatestenable = true;
		alphafunc = greater;
		alpharef = 0;

	//	ZWriteEnable = false;
		vertexshader = compile vs_3_0 VS_MAIN_SHADOW();
		pixelshader = compile ps_3_0 PS_MAIN_SHADOW();
	}
};