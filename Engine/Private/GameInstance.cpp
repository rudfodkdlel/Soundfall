#include "GameInstance.h"

#include "Picking.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Prototype_Manager.h"
#include "Observer_Manager.h"
#include "Font_Manager.h"
#include "Light_Manager.h"
#include "Sound_Manager.h"
#include "Collider_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDeviceOut, ppContextOut);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDeviceOut, *ppContextOut);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDeviceOut, *ppContextOut, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pObserver_Manager = CObserver_Manager::Create();
	if (nullptr == m_pObserver_Manager)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDeviceOut, *ppContextOut);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;


	m_pCollider_Manager = CCollider_Manager::Create();
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;



	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{

	m_pSound_Manager->Update(fTimeDelta);
	
	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pPicking->Update();

	m_pObject_Manager->Update(fTimeDelta);

	m_pCollider_Manager->Update();

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);

	m_pCollider_Manager->Check_Nullptr();
}

HRESULT CGameInstance::Begin_Draw()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.0f, 1.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pRenderer->Draw();

	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	m_pGraphic_Device->Present();

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨의 자원을 삭제한다. */
	
	/* 특정 레벨의 객체을 삭제한다. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 특정 레벨의 원형객을 삭제한다. */
	m_pPrototype_Manager->Clear(iLevelIndex);
}


_float CGameInstance::Compute_Random_Normal()
{
	return rand() / static_cast<_float>(RAND_MAX);	
}

_float CGameInstance::Compute_Random(_float fMin, _float fMax)
{
	return fMin + (fMax - fMin) * Compute_Random_Normal();	
}

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNewLevel);
}
_uint CGameInstance::Get_Current_Level() const
{
	return m_pLevel_Manager->Get_Current_Level();
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototypeType, iPrototypeLevelIndex, strPrototypeTag, pArg);
}
const map<const _wstring, class CBase*>* CGameInstance::Get_Prototypes()
{
	return m_pPrototype_Manager->Get_Prototypes();
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLevelIndex, strLayerTag, pArg);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
	
}

CGameObject* CGameInstance::GetLastObjectFromLayer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->GetLastObjectFromLayer(iLevelIndex, strLayerTag);
}

list<class CGameObject*>* CGameInstance::GetLayerList(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->GetLayerList(iLevelIndex, strLayerTag);
}

#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

#pragma endregion

#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Update_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Update(strTimerTag);
}


#pragma endregion

#pragma region PIPELINE

void CGameInstance::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(D3DTS eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(D3DTS eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipeLine->Get_CamPosition();
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(D3DTS eState) const
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

#pragma endregion

#pragma region INPUTDEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(DIM eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(DIMM eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Is_NoKeyPressed()
{
	return m_pInput_Device->Is_NoKeyPressed();
}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Key_Pressing(DIM eMouse)
{
	return m_pInput_Device->Key_Pressing(eMouse);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Down(DIM eMouse)
{
	return m_pInput_Device->Key_Down(eMouse);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Key_Up(DIM eMouse)
{
	return m_pInput_Device->Key_Up(eMouse);
}

#pragma endregion
#pragma region PICKING
void CGameInstance::Transform_Picking_ToLocalSpace(const _float4x4& WorldMatrixInverse)
{
	m_pPicking->Transform_ToLocalSpace(WorldMatrixInverse);
}
_bool CGameInstance::Picking_InWorld(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC)
{
	return m_pPicking->Picking_InWorld(vPickedPos, vPointA, vPointB, vPointC);
}
_bool CGameInstance::Picking_InLocal(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC)
{
	return m_pPicking->Picking_InLocal(vPickedPos, vPointA, vPointB, vPointC);
}

_float4 CGameInstance::Get_Mouse_LocalPos()
{
	return m_pPicking->Get_Mouse_LocalPos();
}

_float4 CGameInstance::Get_Camera_WorldPos()
{
	return m_pPicking->Get_Camera_WorldPos();
}

_float4 CGameInstance::Get_Mouse_Projection(_vector vPlanePoint, _vector vPlaneNormal)
{
	return m_pPicking->Get_Mouse_Projection(vPlanePoint, vPlaneNormal);
}

HRESULT CGameInstance::Add_Observer(const _wstring strTag, CObserver* pObserver)
{
	return m_pObserver_Manager->Add_Observer(strTag, pObserver);
}

HRESULT CGameInstance::Remove_Observer(const _wstring strTag)
{
	return m_pObserver_Manager->Remove_Observer(strTag);
}

void CGameInstance::Notify(const _wstring& strTag, const _wstring& eventType, void* pData)
{
	m_pObserver_Manager->Notify(strTag, eventType, pData);
}

CObserver* CGameInstance::Find_Observer(const _wstring& strTag)
{
	return m_pObserver_Manager->Find_Observer(strTag);
}

#pragma endregion

#pragma region LIGHT_MANAGER

const LIGHT_DESC* CGameInstance::Get_Light(_uint iIndex)
{
	return m_pLight_Manager->Get_Light(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
#pragma endregion


#pragma region FONT_MANAGER

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

void CGameInstance::Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	m_pFont_Manager->Draw(strFontTag, pText, vPosition, vColor, fRotation, vOrigin, fScale);
}

#pragma endregion


#pragma region SOUND_MANAGER

void CGameInstance::PlaySound(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	m_pSound_Manager->PlaySound(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(const wstring pSoundKey, float fVolume)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(CHANNELID eID)
{
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

_float CGameInstance::Get_BeatInterval()
{
	return m_pSound_Manager->Get_BeatInterval();
}

void CGameInstance::SetBPM(const wstring strName)
{
	m_pSound_Manager->SetBPM(strName);
}

_float CGameInstance::Get_Timing()
{
	return m_pSound_Manager->Get_Timing();
}
#pragma endregion

#pragma region COLLIDER_MANAGER

HRESULT CGameInstance::Add_Collider(_uint iIndex, CCollider* Collider, CGameObject* pOwner)
{
	return m_pCollider_Manager->Add_Collider(iIndex, Collider, pOwner);
}

HRESULT CGameInstance::Add_Collider_Group(pair<_uint, _uint> typePair)
{
	return m_pCollider_Manager->Add_Collider_Group(typePair);
}

#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pCollider_Manager);

	Safe_Release(m_pSound_Manager);

	Safe_Release(m_pFont_Manager);

	Safe_Release(m_pLight_Manager);

	Safe_Release(m_pPicking);

	Safe_Release(m_pPipeLine);

	Safe_Release(m_pTimer_Manager);

	Safe_Release(m_pRenderer);

	Safe_Release(m_pObject_Manager);

	Safe_Release(m_pPrototype_Manager);

	Safe_Release(m_pLevel_Manager);

	Safe_Release(m_pObserver_Manager);

	Safe_Release(m_pInput_Device);

	Safe_Release(m_pGraphic_Device);

	Destroy_Instance();

}

void CGameInstance::Free()
{
	__super::Free();
	
}
