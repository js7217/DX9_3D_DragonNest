float		g_fFar = 500.f;		// Far ���� �ʿ��ϴ�. �ȼ����� Z���� ���� ���̸� �Ǵ��ؾ��Ҷ� ����Ѵ�.
matrix		g_matProjInv;		// ���� -> �佺���̽� ���� �̵��� ����ϴ� ��������� �����
matrix		g_matViewInv;		// �佺���̽� -> ���� ���� �̵��� ����ϴ� �佺���̽� ����� �����

texture		g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_ShadowTexture;

sampler ShadowSampler = sampler_state
{
	texture = g_ShadowTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

struct PS_IN
{
	vector	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vColor : COLOR0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vColor[3];
	vColor[0] = tex2D(DiffuseSampler, In.vTexUV);
	vColor[1] = tex2D(ShadeSampler, In.vTexUV);
	vColor[2] = tex2D(SpecularSampler, In.vTexUV);

	Out.vColor = vColor[0] * vColor[1] + vColor[2];

	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float	fViewZ = vDepthInfo.x * g_fFar;		// ViewZ : Z��������� ��ģ ������ ���� Z�� ���ؼ� ������������ ����������.

	vector vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthInfo.y;
	vPosition.w = 1.f;

	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.y * fViewZ;
	vPosition.w = fViewZ;

	// ����ı����� ��ġ -> ������� ������� �����ش�.
	vector vViewPos = mul(vPosition, g_matProjInv);

	// ��������� ��ġ -> �佺���̽� ����� ������� ���Ѵ�.
	vPosition = mul(vPosition, g_matViewInv);

	float currentDepth = vPosition.z / vPosition.w;
	float2 uv = vPosition.xy / vPosition.w;
	uv.y *= -1.f;
	uv = uv * 0.5f + 0.5f;

	float fShadow = tex2D(ShadowSampler, uv).r;
	
//	vShadow.r = vShadow.r * 0.5f + 0.5f;
//	float fShadow = In.vTexUV.x * 2.f - 1.f;

	if (currentDepth >= fShadow + 0.00000125f)
		Out.vColor.rgb *= 0.5f;

	return Out;
}


technique Default_Technique
{
	pass Diffuse_x_Shade
	{
		AlphaTestEnable = true;
		AlphaFunc = Greater;
		AlphaRef = 0;

		ZWriteEnable = false;				// �ΰ��� ��� ������ z������ ���̰��� ������ 0�̱� ������ ����صθ� ���ĺ����� ����ϱ� ���������.

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}