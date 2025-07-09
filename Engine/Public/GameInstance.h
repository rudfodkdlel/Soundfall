#pragma once

/* 엔진과 클라이언트 간의 링크의 역활을 수행한다. */
/* 엔진 내에 존재하는 유일한 싱글톤 클래스이다. */
/* 엔진 개발자가 클라개밫자에게 보여주고싶은 함수를 ... */
#include "Prototype_Manager.h"


NS_BEGIN(Engine)


class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void Update_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	HRESULT Begin_Draw();
	HRESULT Draw();
	HRESULT End_Draw();

	_float Compute_Random_Normal();
	_float Compute_Random(_float fMin, _float fMax);

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint	Get_Current_Level() const;
	class CLevel* Get_Current_Level_Pointer();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	// 에디터용으로만 사용하기
	const map<const _wstring, class CBase*>* Get_Prototypes();
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* GetLastObjectFromLayer(_uint iLevelIndex, const _wstring& strLayerTag);
	list<class CGameObject*>* GetLayerList(_uint iLevelIndex, const _wstring& strLayerTag);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

#pragma region TIMER_MANAGER
	_float Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT Add_Timer(const _wstring& strTimerTag);
	void Update_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region 	PIPELINE
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);
	const _float4x4* Get_Transform_Float4x4(D3DTS eState) const;
	_matrix Get_Transform_Matrix(D3DTS eState) const;
	const _float4* Get_CamPosition() const;
	_matrix Get_Transform_Matrix_Inverse(D3DTS eState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(D3DTS eState) const;
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(DIM eMouse);
	_long	Get_DIMouseMove(DIMM eMouseState);
	_bool Is_NoKeyPressed();
	_bool Key_Pressing(_ubyte byKeyID);
	_bool Key_Pressing(DIM eMouse);
	_bool Key_Down(_ubyte byKeyID);
	_bool Key_Down(DIM eMouse);
	_bool Key_Up(_ubyte byKeyID);
	_bool Key_Up(DIM eMouse);
	
#pragma endregion
//
#pragma region PICKING
	void Transform_Picking_ToLocalSpace(const _float4x4& WorldMatrixInverse);
	_bool Picking_InWorld(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC);
	_bool Picking_InLocal(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC);

	_float4		Get_Mouse_LocalPos();
	_float4		Get_Camera_WorldPos();
	_float4		Get_Mouse_Projection(_vector vPlanePoint, _vector vPlaneNormal);
#pragma endregion

#pragma region OBSERVER_MANAGER
	HRESULT Add_Observer(const _wstring strTag, class CObserver* pObserver);
	HRESULT Remove_Observer(const _wstring strTag);
	void Notify(const _wstring& strTag, const _wstring& eventType, void* pData = nullptr);
	class CObserver* Find_Observer(const _wstring& strTag);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_Light(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void	Light_Clear();
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);
#pragma endregion

#pragma region SOUND_MANAGER
	void PlaySound(const wstring pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const wstring pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

	_float Get_BeatInterval();
	void SetBPM(const wstring strName);
	_float Get_Timing();
#pragma endregion

#pragma region COLLIDER_MANAGER
	// gameinstance에 등록해서 오브젝트에서 추가. 
	HRESULT Add_Collider(_uint iIndex, CCollider* Collider, CGameObject* pOwner);
	// 충돌 검사를 할 타입들을 미리 넣어둔다
	HRESULT Add_Collider_Group(pair<_uint, _uint> typePair);
	
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pContantName);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion


private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CPicking*				m_pPicking = { nullptr };
	class CObserver_Manager*	m_pObserver_Manager = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };
	class CCollider_Manager*	m_pCollider_Manager = { nullptr };
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END