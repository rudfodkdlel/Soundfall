#pragma once

#include "Component.h"
#include "Bounding.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	_bool Get_IsColl() const { return m_isColl; }
	void  Set_IsColl(_bool isColl) { m_isColl = isColl; }
	_bool Get_Active() const { return m_IsActive; }
	void  Set_Active(_bool isActive) { m_IsActive = isActive; }
	_float3 Get_Center() { return m_pBounding->Get_Center(); }
	_float3 Get_Extends() { return m_pBounding->Get_Extents(); }
	COLLIDER Get_Type() { return m_pBounding->Get_Type(); }
	CBounding* Get_Bounding() { return m_pBounding; }
	_int	Get_Group() { return m_eGroup; }
	void	Set_Group(_int eGroup) { m_eGroup = eGroup; }
public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;
	void Update(_fmatrix WorldMatrix);
	_bool Intersect(CCollider* pTargetCollider);
	// 내가 밀릴 방향을 계산한다, 밀리지 않는 물체(벽 등)이면 사용 안할 예정
	_float4 Calc_PushVector(CCollider* pTargetCollider);

#ifdef _DEBUG
	HRESULT Render();
#endif

private:
	COLLIDER			m_eType = { COLLIDER::END };
	CBounding* m_pBounding = { nullptr };

	_int				m_eGroup = {};
	_bool				m_isColl = { false };
	_bool				m_IsActive = { true };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END