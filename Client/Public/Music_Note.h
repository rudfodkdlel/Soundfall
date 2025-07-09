#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
NS_END

NS_BEGIN(Client)

class CMusic_Note final : public CGameObject
{
public:
	typedef struct tagMusicNoteDesc : public CGameObject::GAMEOBJECT_DESC{
		_bool isSpread;
	}MUSICNOTE_DESC;
private:
	CMusic_Note(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMusic_Note(const CMusic_Note& Prototype);
	virtual ~CMusic_Note() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:
	HRESULT Ready_Components();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	_bool	m_isSpread = {};
	_float  m_fTotalTime = {0.f};

public:
	static CMusic_Note* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END