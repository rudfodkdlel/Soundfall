#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)

class  CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(DIM eMouse)
	{
		return m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)];
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(DIMM eMouseState)
	{
		return *((reinterpret_cast<_int*>(&m_tMouseState)) + static_cast<_uint>(eMouseState));
	}

	_bool Is_NoKeyPressed()
	{
		for (int i = 0; i < 256; ++i)
		{
			if (m_byKeyState[i] & 0x80)
				return false; 
		}

		for (int i = 0; i < 4; i++) 
		{
			if (m_tMouseState.rgbButtons[i] & 0x80)
				return false;

		}

		return true; // 전부 안 눌림
	}

	// 현재 누르고 있는 중
	_bool Key_Pressing(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID] & 0x80;
	}
	_bool Key_Pressing(DIM eMouse)
	{
		return m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80;
	}

	// 이번 프레임에 눌림 (Down)
	_bool Key_Down(_ubyte byKeyID)
	{
		return !(m_byPrevKeyState[byKeyID] & 0x80) && (m_byKeyState[byKeyID] & 0x80);
	}
	// 이번 프레임에 눌림 (Down)
	_bool Key_Down(DIM eMouse)
	{
		return !(m_tPreMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80) && (m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80);
	}


	// 이번 프레임에 떼짐 (Up)
	_bool Key_Up(_ubyte byKeyID)
	{
		return (m_byPrevKeyState[byKeyID] & 0x80) && !(m_byKeyState[byKeyID] & 0x80);
	}

	_bool Key_Up(DIM eMouse)
	{
		return (m_tPreMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80) && !(m_tMouseState.rgbButtons[static_cast<_uint>(eMouse)] & 0x80);
	}

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };



private:
	_byte					m_byKeyState[256] = {};
	_byte					m_byPrevKeyState[256] = {};

	DIMOUSESTATE			m_tMouseState = {};
	DIMOUSESTATE			m_tPreMouseState = {};

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void Free(void);

};

NS_END
#endif // InputDev_h__


