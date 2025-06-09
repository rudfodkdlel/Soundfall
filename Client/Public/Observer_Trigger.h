#pragma once

#include "Client_Defines.h"
#include "Observer.h"

NS_BEGIN(Client)

class CObserver_Trigger : public CObserver
{
public:
    virtual void OnNotify(const _wstring& eventType, void* data) override
    {
        if (L"ZOOM_IN" == eventType)
        {
            m_eType = TRIGGERTYPE::ZOOM_IN;
        }
        else if (L"ZOOM_OUT" == eventType)
        {
            m_eType = TRIGGERTYPE::ZOOM_OUT;
        }
        else if (L"SHAKE" == eventType)
        {
            m_eType = TRIGGERTYPE::SHAKE;
        }

        m_IsColl = true;

    }


 
    _bool  Get_IsColl() const { return m_IsColl; }
    TRIGGERTYPE Get_Type() const { return m_eType; }

  
    virtual void Reset() { m_eType = TRIGGERTYPE::END; m_IsColl = false; }

    virtual void Free() override;

private:
    TRIGGERTYPE  m_eType = { TRIGGERTYPE::END };
    _bool        m_IsColl = { false };
};

NS_END