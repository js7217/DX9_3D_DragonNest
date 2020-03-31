#ifndef Component_h__
#define Component_h__

#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent(void);
	virtual ~CComponent(void);
public:
	virtual _int Update_Component(const _float& fTimeDelta);
	virtual	CComponent*		Clone(void) { return nullptr; }
protected:
	virtual void Free(void) PURE;
};

END
#endif // Component_h__
