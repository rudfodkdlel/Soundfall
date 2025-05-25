#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);

	/* 마우스의 윈도우상의 위치를 구하고 */
	/* 뷰스페이스 상에서 마우스 레이로 변형하여 */
	/* 월드까지 변환시켜서 보관하자. */
	void Update();

	_bool Picking_InWorld(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC);
	_bool Picking_InLocal(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC);

	_float4		Get_Mouse_LocalPos() { return m_vLocalMousePos; }

	// 법선 벡터를 넣어주자
	_float4		Get_Mouse_Projection(_vector vPlaneDir, _vector vPlanePoint);

	// 이게 카메라 위치네? 
	_float4		Get_Camera_WorldPos() { return m_vCameraPos; }

	void Transform_ToLocalSpace(const _float4x4& WorldMatrixInverse);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
private:
	HWND			m_hWnd = {};
	_uint			m_iWinSizeX{}, m_iWinSizeY{};

	_float4			m_vMouseRay = {};
	_float4			m_vCameraPos = {};

	_float4			m_vLocalMouseRay = {};
	_float4			m_vLocalMousePos = {};

	//
	_float4			m_vMousePos = {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

NS_END