#ifndef Resources_h__
#define Resources_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CResources : public CComponent // 모든 리소스들을 상속받게 하는 부모클래스, 통일된 자료형.
{
protected:
	explicit CResources(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CResources(const CResources& rhs);
	virtual ~CResources(void);

protected:
	LPDIRECT3DDEVICE9 m_pGraphicDev;

public:
	virtual CResources* Clone(void) PURE;
	virtual void Free(void);
};
END
#endif // Resources_h__
