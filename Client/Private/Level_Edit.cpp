#include "Level_Edit.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "MyImgui.h"

#include "BackGround.h"

CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_Edit::Initialize()
{
	m_pImgui = CMyImgui::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CLevel_Edit::Update(_float fTimeDelta)
{
	m_pImgui->Update(fTimeDelta);
}

HRESULT CLevel_Edit::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	m_pImgui->Render();

	return S_OK;
}



CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Edit* pInstance = new CLevel_Edit(pDevice, pContext);


	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Edit");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Edit::Free()
{
	__super::Free();

	Safe_Release(m_pImgui);
}
