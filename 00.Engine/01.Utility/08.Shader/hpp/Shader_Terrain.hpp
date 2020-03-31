#include "Shader_Base.hpp"

float		g_fDetail; //  Terrain의 디테일값.
float		g_fFar = 500.f;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	//vector		vPosition : POSITION;	// Symantic : 과거 fvf와 동일하다고 생각. hlsl에서 자료형의 정체성을 명시하는 키워드, 반드시 대문자.
	//float3		vNormal : NORMAL;
	//float2		vTexUV : TEXCOORD0;	// 텍스쳐 시멘틱의 경우 최대 7번까지 정의가 가능하며 총 8개의 텍스쳐를 표현함.
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
	Out.vTexUV = In.vTexUV; // UV는 동일하게 사용.

	vector		vWorldNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));

	Out.vNormal = vWorldNormal;
	Out.vProjPos = Out.vPosition;		// 이때 포지션 값은 z나누기 하기 전의 투영행렬 상태.
	return Out;
}

struct PS_IN
{
	vector vPosition : POSITION;
	vector vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	vector vProjPos : TEXCOORD1;
};

struct PS_OUT // 픽셀 쉐이딩에서는 정점의 Out값이 포지션 시멘틱을 사용할 수 없다.
{
	vector		vDiffuse : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
};

PS_OUT		PS_MAIN(PS_IN In) // 컬러값 정해주기.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = tex2D(BaseSampler, In.vTexUV);
	Out.vDiffuse.a = 1.f;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w / g_fFar, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);
					// Z나누기를 위한 w값을 far로 나눠서 투영상의 값으로 바꿔준다.
					// w값을 z에 나눠서 z나누기 이후의 값을 가진다??
	return Out;
}

technique Default_Device						// 거의 메인함수...
{
	// PASS : 기능의 캡슐화
	pass
	{
		
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};