matrix		g_matWorld;
matrix		g_matView;
matrix		g_matProj;
// 상수 테이블  : 값은 클라가 가지고 있고 해당 값을 대표하는 이름일뿐.

texture		g_BaseTexture; // 텍스쳐는 보통 팔레트 같음. 
						   // 샘플러 : 색상정보만 가지고 있는 팔레트 형태의 텍스쳐를 쉐이더에서 일컫는 형태

sampler BaseSampler = sampler_state // 팔레트 상태의 텍스쳐를 hlsl에서 담아두는 자료형의 형태, 구조체\
																		// sampler_state : 샘플러를 정의하며 샘플러의 상태도 정의했다고 말해주는 코드
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};