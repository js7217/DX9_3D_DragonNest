#include "Shader_Base.hpp"

struct VS_IN
{
	vector		vPosition : POSITION;	// Symantic : 과거 fvf와 동일하다고 생각. hlsl에서 자료형의 정체성을 명시하는 키워드, 반드시 대문자.
	float2		vTexUV : TEXCOORD0;	// 텍스쳐 시멘틱의 경우 최대 7번까지 정의가 가능하며 총 8개의 텍스쳐를 표현함.
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV; // UV는 동일하게 사용.

	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT // 픽셀 쉐이딩에서는 정점의 Out값이 포지션 시멘틱을 사용할 수 없다.
{
	vector		vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In) // 컬러값 정해주기.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

technique Default_Device						// 거의 메인함수...
{
	// PASS : 기능의 캡슐화
	pass
	{
		AlphaBlendEnable = true;
		Srcblend = srcalpha;
		Destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
	pass
	{
		alphatestenable = true;
		alphafunc = greater;
		alpharef = 0;
	//	cullmode = none;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};