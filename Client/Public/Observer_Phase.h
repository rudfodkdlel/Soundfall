#pragma once

#include "Client_Defines.h"
#include "Observer.h"

NS_BEGIN(Client)

class CObserver_Phase : public CObserver
{
public:
    virtual void OnNotify(const _wstring& eventType, void* data) override
    {
        if (L"Phase_End")
        {
            m_IsPhaseEnd = true;
            ++m_iPhase;
        }
  

    }


    _int&  Get_Phase() { return m_iPhase; }

    _bool& IsPhaseEnd() { return m_IsPhaseEnd; }

    // phase 바뀌는 애니메이션 끝나면 다시 초기화해주기
    virtual void Reset() { m_IsPhaseEnd = false; }

    virtual void Free() override;

private:
    _bool m_IsPhaseEnd = false;
    _int m_iPhase = { 1 };
};

NS_END