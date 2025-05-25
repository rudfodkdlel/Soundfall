#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    return S_OK;
}

void CPicking::Update()
{
    POINT ptMouse = {};
    GetCursorPos(&ptMouse);  // 운영체제 기준 커서 위치
    ScreenToClient(m_hWnd, &ptMouse); // 윈도우 기준으로 변환 (클라이언트 좌표계)

    /*2. 투영스페이스로 옮기자. 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w */
    _float4			vPosition = { };

    vPosition.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
    vPosition.y = ptMouse.y / (m_iWinSizeY * -0.5f) + 1.f;
    vPosition.z = 0.0f;
    vPosition.w = 1.f;

    // 투영 역변환
    XMStoreFloat4(&m_vMouseRay, XMVector4Transform(XMLoadFloat4(&vPosition), m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ)));
    m_vCameraPos = _float4(0.f, 0.f, 0.f, 1.f);

    // 카메라 위치
    XMStoreFloat4(&m_vCameraPos, (XMVector3TransformCoord(XMLoadFloat4(&m_vCameraPos), m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW))));

    // 방향 구하고 정규화
    XMStoreFloat4(&m_vMouseRay, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat4(&m_vMouseRay), m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW))));
}

_bool CPicking::Picking_InWorld(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC)
{

    XMVECTOR vA = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&vPointA));
    XMVECTOR vB = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&vPointB));
    XMVECTOR vC = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&vPointC));

    XMVECTOR rayStartPos = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_vCameraPos));
    XMVECTOR rayDir = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_vMouseRay));

    float fDist = 0.f;

    bool hit = DirectX::TriangleTests::Intersects(rayStartPos, rayDir, vA, vB, vC, fDist);

    if (hit)
    {
        XMVECTOR pickPos = XMVectorAdd(rayStartPos, XMVectorScale(rayDir, fDist));
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&vPickedPos), pickPos);
    }

    return hit ? true : false;
}

_bool CPicking::Picking_InLocal(_float4& vPickedPos, const _float4& vPointA, const _float4& vPointB, const _float4& vPointC)
{
    XMVECTOR vA = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&vPointA));
    XMVECTOR vB = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&vPointB));
    XMVECTOR vC = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&vPointC));

    XMVECTOR rayStartPos = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_vLocalMousePos));
    XMVECTOR rayDir = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&m_vLocalMouseRay));

    float fDist = 0.f;

    bool hit = DirectX::TriangleTests::Intersects(rayStartPos, rayDir, vA, vB, vC, fDist);

    if (hit)
    {
        XMVECTOR pickPos = XMVectorAdd(rayStartPos, XMVectorScale(rayDir, fDist));
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&vPickedPos), pickPos);
    }

    return hit ? true : false;
}

_float4 CPicking::Get_Mouse_Projection(_vector vPlanePoint, _vector vPlaneNormal)
{
    _vector vRayOrigin = XMLoadFloat4(&m_vCameraPos);     // 광선 시작점 (카메라)
    _vector vRayDir = XMVector3Normalize(XMLoadFloat4(&m_vMouseRay));    // 광선 방향

    // 법선 • 방향벡터가 0이면 평행
    float denom = XMVectorGetX(XMVector3Dot(vRayDir, vPlaneNormal));
    if (fabsf(denom) < 1e-6f)
        return _float4(0.f, 0.f, 0.f, 0.f); // 교차 없음

    // t = ((평면점 - 광선시작점) • 법선) / (광선방향 • 법선)
    float t = XMVectorGetX(
        XMVector3Dot(vPlanePoint - vRayOrigin, vPlaneNormal)
    ) / denom;

    if (t < 0.f)
        return _float4(0.f, 0.f, 0.f, 0.f); // 평면 뒤에 교점

    _vector vIntersection = vRayOrigin + vRayDir * t;

    _float4 result;
    XMStoreFloat4(&result, vIntersection);
    result.w = 1.f;
    return result;
}


void CPicking::Transform_ToLocalSpace(const _float4x4& WorldMatrixInverse)
{
   
    XMStoreFloat4(&m_vLocalMousePos, (XMVector3TransformCoord(XMLoadFloat4(&m_vCameraPos), XMLoadFloat4x4(&WorldMatrixInverse))));
    XMStoreFloat4(&m_vLocalMouseRay, (XMVector3TransformNormal(XMLoadFloat4(&m_vMouseRay), XMLoadFloat4x4(&WorldMatrixInverse))));
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX("Failed to Created : CPicking");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
