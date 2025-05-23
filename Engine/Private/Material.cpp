#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}
HRESULT CMaterial::Initialize(const MATERIAL_DESC& eDesc, const char* pModelFilePath)
{

    for (size_t i = 1; i < 27; i++)
    {
        m_iNumSRVs[i] = eDesc.iNumSRVs[i];
        m_SRVs[i].reserve(m_iNumSRVs[i]);
        m_TexturePaths[i] = eDesc.TexturePaths[i];


        for (int j = 0; j < m_iNumSRVs[i]; ++j)
        {
            HRESULT         hr = { };
            ID3D11ShaderResourceView* pSRV = { nullptr };

            char buffer[MAX_PATH] = {};
            WideCharToMultiByte(CP_ACP, 0, m_TexturePaths[i][j].szTexturePath, -1, buffer, MAX_PATH, nullptr, nullptr);

            char szFullPath[MAX_PATH] = {};
            char szDrive[MAX_PATH] = {};
            char szDir[MAX_PATH] = {};
            char szFileName[MAX_PATH] = {};
            char szExt[MAX_PATH] = {};

            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(buffer, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDir);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            wchar_t szTextureFilePath[MAX_PATH] = {};
            MultiByteToWideChar(CP_ACP, 0, szFullPath, -1, szTextureFilePath, MAX_PATH);

            hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_SRVs[i].push_back(pSRV);
        }

    }





    return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _int eType, _uint iTextureIndex)
{
    if (iTextureIndex >= m_SRVs[eType].size())
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MATERIAL_DESC& eDesc, const char* pModelFilePath)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(eDesc, pModelFilePath)))
    {
        MSG_BOX("Failed to Created : CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMaterial::Free()
{
    __super::Free();

    for (size_t i = 0; i < 27; i++)
    {
        for (auto& pSRV : m_SRVs[i])
            Safe_Release(pSRV);
        m_SRVs[i].clear();
    }

    m_TexturePaths->clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
