#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;
	}SPHERE_DESC;
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;
public:
	const BoundingSphere* Get_Desc() const {
		return m_pDesc;
	}
	virtual _float3 Get_Center() { return m_pDesc->Center; }
	virtual _float3 Get_Extents() { return _float3(m_pDesc->Radius, m_pDesc->Radius, m_pDesc->Radius); }
public:
	HRESULT Initialize(const CBounding::BOUNDING_DESC* pDesc);
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CBounding* pTarget) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	/* 채워지는 초기정보는 로컬상에 상태대로 셋팅한다. */
	BoundingSphere* m_pOriginalDesc = { nullptr };
	BoundingSphere* m_pDesc = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CBounding::BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

NS_END