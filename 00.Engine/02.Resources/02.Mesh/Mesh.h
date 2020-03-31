#ifndef Mesh_h__
#define Mesh_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CMesh : public CResources
{
protected:
	explicit	CMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CMesh(const CMesh& rhs);
	virtual		~CMesh(void);

protected:
	_bool		m_bClone;
public:
	virtual CResources*	Clone(void)PURE;
public:
	virtual void	Free(void);

};

END
#endif // Mesh_h__
