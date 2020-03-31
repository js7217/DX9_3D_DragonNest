#ifndef Terrain_h__
#define Terrain_h__

#include "Defines.h"
#include "GameObject.h"
#include "Cell.h"

namespace Engine
{
	class CTerrainTex;
	class CTexture;
	class CTransform;
	class CRenderer;
	class COptimization;
	class CShader;
	class CNaviMesh;
}

class CTerrain : public Engine::CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrain(void);

public:
	void Set_TexNum(_int iTexNum) { m_iTexNum = iTexNum; }
	void Get_NaviMesh(Engine::CNaviMesh** ppNaviMesh);
public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

public:
	void		Create_Cell(const _vec3& vPos, const Engine::CCell::CELL_OPTION& eOption);
	void		Link_Cell();
private:
	HRESULT		AddComponent(void);
	HRESULT		Set_ConStantTable(LPD3DXEFFECT pEffect);

private:
	Engine::CTerrainTex*		m_pBufferCom;

	Engine::CTexture*			m_pTextureCom;
	Engine::CTexture*			m_pFilterCom;
	Engine::CTexture*			m_pBrushCom;

	Engine::CTransform*			m_pTransCom;
	Engine::CRenderer*			m_pRenderCom;
	Engine::COptimization*		m_pOptimizationCom;
	Engine::CShader*			m_pShaderCom;
	Engine::CNaviMesh*			m_pNaviCom;

private:
	Engine::INDEX32*			m_pIndex = nullptr;
	_ulong						m_dwTriCnt = 0;
	_int						m_iTexNum = 0;

public:
	static CTerrain*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};


#endif // Terrain_h__
