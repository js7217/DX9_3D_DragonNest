#ifndef Line_h__
#define Line_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLine : public CBase
{
public:
	enum POINT { POINT_START, POINT_FINISH, POINT_END };
	enum COMPARE { COMPARE_OUT, COMPARE_IN };

private:
	explicit CLine(void);
	virtual ~CLine(void);

public:
	_vec3 Get_Sliding() { return _vec3(m_vSliding.x, 0.f, m_vSliding.y); }

public:
	HRESULT				Ready_Line(const _vec2 * pPointA, const _vec2* pPointB);
	COMPARE				Compare(const _vec2* pEndPos, const _vec3* pTargetDir);
	_vec2				Make_SlidingVector(const _vec2* pEndPos, const _vec3* pTargetDir);
private:
	_vec2		m_vPoint[POINT_END];
	_vec2		m_vDirection;
	_vec2		m_vNormal;
	_vec2		m_vSliding;
public:
	static CLine*	Create(const _vec2* pPointA, const _vec2* pPointB);
	virtual void	Free(void);
};
END
#endif // Line_h__
