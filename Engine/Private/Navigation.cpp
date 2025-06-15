#include "Navigation.h"

#include "Cell.h"
#include "GameInstance.h"

_float4x4		CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_Cells{ Prototype.m_Cells }
	, m_iIndex{ Prototype.m_iIndex }
#ifdef _DEBUG
	, m_pShader{ Prototype.m_pShader }
#endif
{

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

}


HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile)
{

	std::ifstream fin(pNavigationDataFile, std::ios::binary);
	if (!fin)
		return E_FAIL;

	size_t count = 0;
	fin.read(reinterpret_cast<char*>(&count), sizeof(size_t));



	for (size_t i = 0; i < count; ++i)
	{
		Triangle tri = {};
		
		
		fin.read(reinterpret_cast<char*>(&tri), sizeof(Triangle));

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, tri.points, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}
	fin.close();
	



#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("C:/Users/USER/source/repos/soundfall/Client/Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	
	
#endif

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iIndex = pDesc->iIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_bool CNavigation::isMove(_fvector vWorldPos)
{
	_vector		vLocalPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iIndex]->isIn(vLocalPos, &iNeighborIndex))
		return true;

	else
	{
		if (-1 == iNeighborIndex)
			/* 이웃이 없다면 */
			return false;

		else
		{
			while (true)
			{
				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))
					break;

				if (-1 == iNeighborIndex)
					return false;
			}

			m_iIndex = iNeighborIndex;

			/* 이웃이 있다면 */
			return true;
		}
	}
}

_vector CNavigation::SetUp_Height(_fvector vWorldPos)
{
	_vector		vLocalPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	vLocalPos = XMVectorSetY(vLocalPos, m_Cells[m_iIndex]->Compute_Height(vLocalPos));

	return XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix));
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{

	_float4x4		WorldMatrix = m_WorldMatrix;
	WorldMatrix.m[3][1] += 0.2f;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));

	m_pShader->Begin(0);



	for (auto& pCell : m_Cells)
	{
		pCell->Render();

	}

	// debug용
	
	for (auto& pCell : m_Cells)
	{
		XMMATRIX result = XMMatrixMultiply(XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)));

		_vector vPos = pCell->Get_MiddlePoint();

		vPos = XMVector3TransformCoord(vPos, result);

		// NDC → 화면 좌표로 매핑 
		_float2 screenPos;
		screenPos.x = (vPos.m128_f32[0] + 1.0f) * 0.5f * 1280.f;
		screenPos.y = (1.0f - vPos.m128_f32[1]) * 0.5f * 720.f;

		_wstring str = to_wstring(pCell->Get_index());

		m_pGameInstance->Draw_Font(TEXT("Default"), str.data(), screenPos);
	}

	// edit에서 안보여서 임시로 일단
	
	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));

	m_pShader->Begin(0);


	return S_OK;
}
#endif


HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFile)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif	
}
