matrix		g_matWorld;
matrix		g_matView;
matrix		g_matProj;
// ��� ���̺�  : ���� Ŭ�� ������ �ְ� �ش� ���� ��ǥ�ϴ� �̸��ϻ�.

texture		g_BaseTexture; // �ؽ��Ĵ� ���� �ȷ�Ʈ ����. 
// ���÷� : ���������� ������ �ִ� �ȷ�Ʈ ������ �ؽ��ĸ� ���̴����� ���´� ����

float		g_fAlpha;			//Alpha ������ ���.

sampler BaseSampler = sampler_state // �ȷ�Ʈ ������ �ؽ��ĸ� hlsl���� ��Ƶδ� �ڷ����� ����, ����ü\
									// sampler_state : ���÷��� �����ϸ� ���÷��� ���µ� �����ߴٰ� �����ִ� �ڵ�
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

};

struct VS_IN
{
	vector		vPosition : POSITION;	// Symantic : ���� fvf�� �����ϴٰ� ����. hlsl���� �ڷ����� ��ü���� ����ϴ� Ű����, �ݵ�� �빮��.
	float2		vTexUV	  : TEXCOORD0;	// �ؽ��� �ø�ƽ�� ��� �ִ� 7������ ���ǰ� �����ϸ� �� 8���� �ؽ��ĸ� ǥ����.
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
	Out.vTexUV = In.vTexUV; // UV�� �����ϰ� ���.

	return Out;
}

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT // �ȼ� ���̵������� ������ Out���� ������ �ø�ƽ�� ����� �� ����.
{
	vector		vColor : COLOR;
};

PS_OUT		PS_MAIN(PS_IN In) // �÷��� �����ֱ�.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vColor.a *= g_fAlpha;
	// 2���� ������ �ؽ��ķκ��� uv��ǥ�� �ش��ϴ� ���� �����ϴ� �Լ�, ��ȯ Ÿ���� vector

	//if (In.vTexUV.y > 0.5f)
	//	Out.vColor.a = 1.f;
	//else
	//	Out.vColor.a = 0.5f;

	return Out;
}

PS_OUT		PS_MAIN2(PS_IN In) // �÷��� �����ֱ�.
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	// 2���� ������ �ؽ��ķκ��� uv��ǥ�� �ش��ϴ� ���� �����ϴ� �Լ�, ��ȯ Ÿ���� vector

	//if (In.vTexUV.x > 0.5f)
	//	Out.vColor.r = 1.f;
	//else
	//	Out.vColor.r = 0.5f;

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

		vertexshader	= compile vs_3_0 VS_MAIN();
		pixelshader		= compile ps_3_0 PS_MAIN();
	}

	pass
	{
		vertexshader	= compile vs_3_0 VS_MAIN();
		pixelshader		= compile ps_3_0 PS_MAIN2();
	}
};