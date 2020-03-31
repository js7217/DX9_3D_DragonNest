
// For.Light

vector		g_vLightPos;
float		g_fRange;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

// For.Global Material
vector		g_vMtrlDiffuse = (vector)1.f;
vector		g_vMtrlAmbient = (vector)1.f;
vector		g_vMtrlSpecular = (vector)1.f;

vector		g_vCamPosition;
float		g_fFar = 500.f;		// Far ���� �ʿ��ϴ�. �ȼ����� Z���� ���� ���̸� �Ǵ��ؾ��Ҷ� ����Ѵ�.
matrix		g_matProjInv;		// ���� -> �佺���̽� ���� �̵��� ����ϴ� ��������� �����
matrix		g_matViewInv;		// �佺���̽� -> ���� ���� �̵��� ����ϴ� �佺���̽� ����� �����

texture		g_NormalTexture;
texture		g_DepthTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

struct PS_IN
{
	vector	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vShade		: COLOR0;
	vector	vSpecular	: COLOR1;		// ��� + ����ŧ�� ����Ÿ�� �����.
};


PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float	fViewZ = vDepthInfo.x * g_fFar;		// ViewZ : Z��������� ��ģ ������ ���� Z�� ���ؼ� ������������ ����������.

	// 0 => -1 
	// 1 => 1
	vector	vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	Out.vShade = (g_vLightDiffuse * g_vMtrlDiffuse) * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	vector vSpecular;
	vector vReflect;
	vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));

	vector vLook;

	// ȭ����� �ȼ� ��ġ
	vector vPosition;			// ����Ʈ���� ��ġ��.

	// ������ı��� ���س��� ��ġ -> Z�����Ⱑ �Ͼ �ȼ� ��ġ��.
	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthInfo.y;
	vPosition.w = 1.f;

	// Z������ ���� �ȼ� ��ġ��. ->  Z�� ���ϴ� ������ �Ұ�.
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.y * fViewZ;
	vPosition.w = fViewZ;

	// ����ı����� ��ġ -> ������� ������� �����ش�.
	vPosition = mul(vPosition, g_matProjInv);

	// ��������� ��ġ -> �佺���̽� ����� ������� ���Ѵ�.
	vPosition = mul(vPosition, g_matViewInv);

	vLook = vPosition - g_vCamPosition;

	vSpecular = pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);		// 30 == power

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * vSpecular;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float	fViewZ = vDepthInfo.x * g_fFar;

	// �� �ȼ��� ��ġ.
	vector	vPosition;

	// ������ı������س��� ��ġ. / z == ���������̽����� ��ġ.
	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthInfo.y;
	vPosition.w = 1.f;

	// ������ı������س��� ��ġ.
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.y * fViewZ;
	vPosition.w = fViewZ;

	// ����ı��� ���س��� ��ġ.
	vPosition = mul(vPosition, g_matProjInv);

	// ������ı��� ���س��� ��ġ.
	vPosition = mul(vPosition, g_matViewInv);

	vector	vLightDir = vPosition - g_vLightPos;
	float	fDistance = length(vLightDir);

	float	fAtt = max((g_fRange - fDistance), 0.f) / g_fRange;

	// 0 => -1 
	// 1 => 1
	vector	vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	Out.vShade = ((g_vLightDiffuse * g_vMtrlDiffuse) * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;

	Out.vShade.a = 1.f;

	vector	vSpecular;

	vector	vReflect;

	vReflect = reflect(normalize(vLightDir), normalize(vNormal));

	vector	vLook;





	vLook = vPosition - g_vCamPosition;

	vSpecular = fAtt * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 60.f);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * vSpecular;
	Out.vSpecular.a = 0.f;

	return Out;
}

technique Default_Technique
{
	pass Directional
	{
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_DIRECTIONAL();
	}

	pass Point
	{
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_POINT();
	}
}