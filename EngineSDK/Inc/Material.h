#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	MATERIAL_DESC Get_Desc() const
	{
		MATERIAL_DESC eDesc = {};
		memcpy(eDesc.iNumSRVs, m_iNumSRVs, sizeof(_uint) * 27);

		for (int i = 0; i < 27; ++i)
		{
			size_t vecSize = m_TexturePaths[i].size();
			eDesc.TexturePaths[i].resize(vecSize);

			for (size_t j = 0; j < vecSize; ++j)
			{
				eDesc.TexturePaths[i][j].iTextureType = m_TexturePaths[i][j].iTextureType;
				wcscpy_s(eDesc.TexturePaths[i][j].szTexturePath, MAX_PATH, m_TexturePaths[i][j].szTexturePath);
			}
		}



		return eDesc;
	}

public:
	
	HRESULT Initialize(const MATERIAL_DESC& eDesc, const char* pModelFilePath);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _int eType, _uint iTextureIndex);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*>			m_SRVs[27] = {};

	_uint  m_iNumSRVs[27] = {};

	vector<MATERIAL_PATH_DESC>					m_TexturePaths[27] = {};

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MATERIAL_DESC& eDesc, const char* pModelFilePath);
	virtual void Free() override;


};

NS_END