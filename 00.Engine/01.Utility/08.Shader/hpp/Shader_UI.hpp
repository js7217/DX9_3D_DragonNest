#include "Shader_Base.hpp"

struct VS_IN
{
	vector		vPosition : POSITION;	// Symantic : ���� fvf�� �����ϴٰ� ����. hlsl���� �ڷ����� ��ü���� ����ϴ� Ű����, �ݵ�� �빮��.
	float2		vTexUV : TEXCOORD0;	// �ؽ��� �ø�ƽ�� ��� �ִ� 7������ ���ǰ� �����ϸ� �� 8���� �ؽ��ĸ� ǥ����.
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
	Out.vTexUV = In.vTexUV; // UV�� �����ϰ� ���.

	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT // �ȼ� ���̵������� ������ Out���� ������ �ø�ƽ�� ����� �� ����.
{
	vector		vColor : COLOR0;
};

PS_OUT		PS_MAIN(PS_IN In) // �÷��� �����ֱ�.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

technique Default_Device						// ���� �����Լ�...
{
	// PASS : ����� ĸ��ȭ
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