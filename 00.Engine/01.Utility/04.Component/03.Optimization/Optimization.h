#ifndef Optimization_h__
#define Optimization_h__

#include "Component.h"
#include "Frustum.h"

BEGIN(Engine)

class ENGINE_DLL COptimization : public CComponent
{
private:
	explicit COptimization(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit COptimization(const COptimization& rhs);
	virtual	~COptimization(void);

public:
	HRESULT	Ready_Optimization();
	_bool	IsInFrustum_ForObject(const _vec3* pPosWorld, const _float fRadius);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	_bool					m_bClone;
	CFrustum*				m_pFrustum;

public:
	static COptimization*	Create(LPDIRECT3DDEVICE9	pGraphicDev);
	CComponent*	Clone(void);
	virtual void Free(void);
};
END
#endif // Optimization_h__
