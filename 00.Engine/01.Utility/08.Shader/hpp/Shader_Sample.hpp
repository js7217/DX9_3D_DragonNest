matrix		g_matWorld;
matrix		g_matView;
matrix		g_matProj;
// 상수 테이블  : 값은 클라가 가지고 있고 해당 값을 대표하는 이름일뿐.

texture		g_BaseTexture; // 텍스쳐는 보통 팔레트 같음. 
// 샘플러 : 색상정보만 가지고 있는 팔레트 형태의 텍스쳐를 쉐이더에서 일컫는 형태

float		g_fAlpha;			//Alpha 값으로 사용.

sampler BaseSampler = sampler_state // 팔레트 상태의 텍스쳐를 hlsl에서 담아두는 자료형의 형태, 구조체\
									// sampler_state : 샘플러를 정의하며 샘플러의 상태도 정의했다고 말해주는 코드
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

};

struct VS_IN
{
	vector		vPosition : POSITION;	// Symantic : 과거 fvf와 동일하다고 생각. hlsl에서 자료형의 정체성을 명시하는 키워드, 반드시 대문자.
	float2		vTexUV	  : TEXCOORD0;	// 텍스쳐 시멘틱의 경우 최대 7번까지 정의가 가능하며 총 8개의 텍스쳐를 표현함.
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV	  : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;
	
	matrix matWV, matWVP;

	matWV  = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV; // UV는 동일하게 사용.

	return Out;
}

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT // 픽셀 쉐이딩에서는 정점의 Out값이 포지션 시멘틱을 사용할 수 없다.
{
	vector		vColor : COLOR;
};

PS_OUT		PS_MAIN(PS_IN In) // 컬러값 정해주기.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vColor.a *= g_fAlpha;
	// 2차원 형태의 텍스쳐로부터 uv좌표에 해당하는 색을 추출하는 함수, 반환 타입이 vector

	//if (In.vTexUV.y > 0.5f)
	//	Out.vColor.a = 1.f;
	//else
	//	Out.vColor.a = 0.5f;

	return Out;
}

PS_OUT		PS_MAIN2(PS_IN In) // 컬러값 정해주기.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	// 2차원 형태의 텍스쳐로부터 uv좌표에 해당하는 색을 추출하는 함수, 반환 타입이 vector

	//if (In.vTexUV.x > 0.5f)
	//	Out.vColor.r = 1.f;
	//else
	//	Out.vColor.r = 0.5f;

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

		vertexshader	= compile vs_3_0 VS_MAIN();
		pixelshader		= compile ps_3_0 PS_MAIN();
	}

	pass
	{
		vertexshader	= compile vs_3_0 VS_MAIN();
		pixelshader		= compile ps_3_0 PS_MAIN2();
	}
};