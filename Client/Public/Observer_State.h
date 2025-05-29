#include "Client_Defines.h"
#include "Observer.h"

NS_BEGIN(Client)

class CObserver_State : public CObserver
{
public:
    virtual void OnNotify(const _wstring& eventType, void* data) override
    {
        if (L"STATE_IDLE" == eventType)
        {
            m_eMainState = STATE_MAIN::IDLE;
            m_eSubState = STATE_SUB::NONE;
        }
        else if (L"STATE_MOVE" == eventType)
        {
            m_eMainState = STATE_MAIN::MOVE;
            m_eSubState = STATE_SUB::NONE;
        }
        else if(L"STATE_DASH" == eventType)
        {
            m_eMainState = STATE_MAIN::DASH;
            m_eSubState = STATE_SUB::NONE;
        }
        else if (L"STATE_HIT" == eventType)
        {
            m_eMainState = STATE_MAIN::HIT;
            m_eSubState = STATE_SUB::NONE;
        }
        else if (L"STATE_SPWAN" == eventType)
        {
            m_eMainState = STATE_MAIN::SPWAN;
            m_eSubState = STATE_SUB::NONE;
        }
        else if (L"STATE_ATTACK" == eventType)
        {
            m_eMainState = STATE_MAIN::ATTACK;
            m_eSubState = STATE_SUB::NONE;
        }
        else if (L"STATE_ATTACK_MOVE" == eventType)
        {
            m_eMainState = STATE_MAIN::ATTACK;
            m_eSubState = STATE_SUB::MOVEABLE;
        }
        else if (L"STATE_ATTACK_CHARGE" == eventType)
        {
            m_eMainState = STATE_MAIN::ATTACK;
            m_eSubState = STATE_SUB::CHARGE;
        }
        else if (L"STATE_ATTACK_COMBO" == eventType)
        {
            m_eMainState = STATE_MAIN::ATTACK;
            m_eSubState = STATE_SUB::COMBO;
        }
        else if (L"STATE_ATTACK_ULT" == eventType)
        {
            m_eMainState = STATE_MAIN::ATTACK;
            m_eSubState = STATE_SUB::ULT;
        }
    }

    pair<STATE_MAIN, STATE_SUB> Get_State()
    {
        return { m_eMainState, m_eSubState };
    }
   

   

   virtual void Reset() {
        m_eMainState = STATE_MAIN::IDLE;
        m_eSubState = STATE_SUB::NONE; 

    } 

    virtual void Free() override;

private:
    STATE_MAIN m_eMainState = { STATE_MAIN::IDLE };
    STATE_SUB  m_eSubState = { STATE_SUB::NONE };
    WEAPON     m_eWeaponState = { WEAPON::END };
};

NS_END