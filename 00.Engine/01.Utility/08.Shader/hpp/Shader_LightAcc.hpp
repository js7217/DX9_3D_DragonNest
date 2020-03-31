
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
float		g_fFar = 500.f;		// Far 값이 필요하다. 픽셀들의 Z값을 위해 깊이를 판단해야할때 사용한다.
matrix		g_matProjInv;		// 투영 -> 뷰스페이스 로의 이동에 사용하는 투영행렬의 역행렬
matrix		g_matViewInv;		// 뷰스페이스 -> 월드 로의 이동에 사용하는 뷰스페이스 행렬의 역행렬

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
	vector	vSpecular	: COLOR1;		// 명암 + 스펙큘러 렌더타겟 사용함.
};


PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float	fViewZ = vDepthInfo.x * g_fFar;		// ViewZ : Z나누기까지 거친 뎁스의 값을 Z를 곱해서 투영영역으로 내리려고함.

	// 0 => -1 
	// 1 => 1
	vector	vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	Out.vShade = (g_vLightDiffuse * g_vMtrlDiffuse) * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	vector vSpecular;
	vector vReflect;
	vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));

	vector vLook;

	// 화면상의 픽셀 위치
	vector vPosition;			// 뷰포트상의 위치값.

	// 투영행렬까지 곱해놓은 위치 -> Z나누기가 일어난 픽셀 위치값.
	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthInfo.y;
	vPosition.w = 1.f;

	// Z나누기 전의 픽셀 위치값. ->  Z를 곱하는 행위를 할것.
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.y * fViewZ;
	vPosition.w = fViewZ;

	// 뷰행렬까지의 위치 -> 투영행렬 역행렬을 곱해준다.
	vPosition = mul(vPosition, g_matProjInv);

	// 월드까지의 위치 -> 뷰스페이스 행렬의 역행렬을 곱한다.
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

	// 내 픽셀의 위치.
	vector	vPosition;

	// 투영행렬까지곱해놓은 위치. / z == 투영스페이스상의 위치.
	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthInfo.y;
	vPosition.w = 1.f;

	// 투영행렬까지곱해놓은 위치.
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepthInfo.y * fViewZ;
	vPosition.w = fViewZ;

	// 뷰행렬까지 곱해놓은 위치.
	vPosition = mul(vPosition, g_matProjInv);

	// 월드행렬까지 곱해놓은 위치.
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