#ifndef CellTriCol_h__
#define CellTriCol_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCellTriCol : public CVIBuffer
{
private:
	explicit CCellTriCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCellTriCol(const CCellTriCol& rhs);
	virtual ~CCellTriCol(void);

public:
	HRESULT Ready_Buffer(const _vec3& vPosA,
						 const _vec3& vPosB,
						 const _vec3& vPosC,
						 const D3DXCOLOR& dwColor);

	void Change_Color(const D3DXCOLOR& dwColor);
	void Set_Pos(const _vec3& vPosA,
				 const _vec3& vPosB,
				 const _vec3& vPosC);
public:
	static CCellTriCol*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									const _vec3& vPosA, 
									const _vec3& vPosB,
									const _vec3& vPosC, 
									const D3DXCOLOR& dwColor);
	virtual CResources* Clone(void);
	virtual void		Free(void);
};
END
#endif // CellTriCol_h__
