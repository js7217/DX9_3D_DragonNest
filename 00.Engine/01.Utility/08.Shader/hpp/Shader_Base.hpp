matrix		g_matWorld;
matrix		g_matView;
matrix		g_matProj;
// ��� ���̺�  : ���� Ŭ�� ������ �ְ� �ش� ���� ��ǥ�ϴ� �̸��ϻ�.

texture		g_BaseTexture; // �ؽ��Ĵ� ���� �ȷ�Ʈ ����. 
						   // ���÷� : ���������� ������ �ִ� �ȷ�Ʈ ������ �ؽ��ĸ� ���̴����� ���´� ����

sampler BaseSampler = sampler_state // �ȷ�Ʈ ������ �ؽ��ĸ� hlsl���� ��Ƶδ� �ڷ����� ����, ����ü\
																		// sampler_state : ���÷��� �����ϸ� ���÷��� ���µ� �����ߴٰ� �����ִ� �ڵ�
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};