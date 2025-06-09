#pragma once

#include "Base.h"

/* 계속 확인하고 싶은 값들을 얻기 위해... */

NS_BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	void Initialize();

public:
	_float Get_BeatInterval() { return m_fBeatInterval; }
	void SetBPM(const wstring strName);
	_float Get_Timing();

public:
	void PlaySound(const wstring, CHANNELID eID, float fVolume);
	void PlayBGM(const  wstring, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

	

	void Update(_float fTimeDelta);
	

private:
	void LoadSoundFile();

public:
	static CSound_Manager* Create();

private:

	map < _wstring, _int> m_mapBPM;
	_int m_iBPM = {120};
	_float m_fBeatInterval = { };
	_float m_fElapsedTime = { 0.f };
	
	// 사운드 리소스 정보를 갖는 객체 
	map<_wstring, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[SOUND_END];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

	class CGameInstance* m_pGameInstance = { nullptr };

	virtual void Free() override;
};

NS_END