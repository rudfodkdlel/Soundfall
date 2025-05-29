#pragma once

#include "Base.h"
#include "Observer.h"

/* 계속 확인하고 싶은 값들을 얻기 위해... */

NS_BEGIN(Engine)

class CObserver_Manager final : public CBase
{
private:
	CObserver_Manager();
	virtual ~CObserver_Manager() = default;


public:
	HRESULT Add_Observer(const _wstring strTag, CObserver* pObserver);
	HRESULT Remove_Observer(const _wstring strTag);
	void Notify(const _wstring& strTag, const _wstring& eventType, void* pData);
	CObserver* Find_Observer(const _wstring& strTag)
	{
		auto iter = m_pObservers.find(strTag);
		if (iter != m_pObservers.end())
			return iter->second;
		return nullptr;
	}

private:
	
	map<_wstring, CObserver*> m_pObservers = { };
	class CGameInstance* m_pGameInstance = { nullptr };



private:

public:
	static CObserver_Manager* Create();
	virtual void Free() override;
};

NS_END