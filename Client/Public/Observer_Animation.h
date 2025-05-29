#pragma once

#include "Client_Defines.h"
#include "Observer.h"

NS_BEGIN(Client)

class CObserver_Animation : public CObserver
{
public:
    virtual void OnNotify(const _wstring& eventType, void* data) override
    {
        if (eventType == L"AnimEnd") 
            m_bAnimFinished = true;
    }
    

    bool IsAnimationFinished() const { return m_bAnimFinished; }

    virtual void Reset() { m_bAnimFinished = false; } // 애니메이션 다시 시작할 때 호출

    virtual void Free() override;

private:
    bool m_bAnimFinished = false;
};

NS_END