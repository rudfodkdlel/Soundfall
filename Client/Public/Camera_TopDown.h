#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Observer_Trigger.h"

NS_BEGIN(Client)

class CCamera_TopDown final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fSensor;
	}CAMERA_FREE_DESC;
private:
	CCamera_TopDown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_TopDown(const CCamera_TopDown& Prototype);
	virtual ~CCamera_TopDown() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void Zoom_In(_float fTimeDelta);
	void Zoom_Out(_float fTimeDelta);

private:
	_float				m_fSensor{};

	_float				m_fOffset = { 5.f };

	_bool				m_IsTargeted = { false };
	CGameObject*		m_pTarget = { nullptr };

	CObserver_Trigger*	m_pObserver = { nullptr };

	_bool				m_bMoveRequested = { false };
	_float				m_fMoveTime = { 1.5f };

public:
	static CCamera_TopDown* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END