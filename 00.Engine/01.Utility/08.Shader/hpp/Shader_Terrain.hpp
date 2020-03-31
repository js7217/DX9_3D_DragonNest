#include "Shader_Base.hpp"

float		g_fDetail; //  Terrain�� �����ϰ�.
float		g_fFar = 500.f;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	//vector		vPosition : POSITION;	// Symantic : ���� fvf�� �����ϴٰ� ����. hlsl���� �ڷ����� ��ü���� ����ϴ� Ű����, �ݵ�� �빮��.
	//float3		vNormal : NORMAL;
	//float2		vTexUV : TEXCOORD0;	// �ؽ��� �ø�ƽ�� ��� �ִ� 7������ ���ǰ� �����ϸ� �� 8���� �ؽ��ĸ� ǥ����.
};

struct VS_OUT
{
	vector vPosition : POSITION;
	vector vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	vector vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV; // UV�� �����ϰ� ���.

	vector		vWorldNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));

	Out.vNormal = vWorldNormal;
	Out.vProjPos = Out.vPosition;		// �̶� ������ ���� z������ �ϱ� ���� ������� ����.
	return Out;
}

struct PS_IN
{
	vector vPosition : POSITION;
	vector vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	vector vProjPos : TEXCOORD1;
};

struct PS_OUT // �ȼ� ���̵������� ������ Out���� ������ �ø�ƽ�� ����� �� ����.
{
	vector		vDiffuse : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
};

PS_OUT		PS_MAIN(PS_IN In) // �÷��� �����ֱ�.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = tex2D(BaseSampler, In.vTexUV);
	Out.vDiffuse.a = 1.f;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
					// Z�����⸦ ���� w���� far�� ������ �������� ������ �ٲ��ش�.
					// w���� z�� ������ z������ ������ ���� ������??
	return Out;
}

technique Default_Device						// ���� �����Լ�...
{
	// PASS : ����� ĸ��ȭ
	pass
	{
		
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};