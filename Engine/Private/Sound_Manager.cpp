#include "Sound_Manager.h"
#include "GameInstance.h"

CSound_Manager::CSound_Manager()
	:m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CSound_Manager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_System_Create(&m_pSystem, 131609U);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	m_mapBPM.emplace(TEXT("Discordance.ogg"), 123);
	m_mapBPM.emplace(TEXT("TricycleRiot.ogg"), 189);
	m_mapBPM.emplace(TEXT("Soundfall_TM_Ice_Snowbound.ogg"), 157);



	m_fBeatInterval = 60.f / m_iBPM;

	LoadSoundFile();
}

void CSound_Manager::SetBPM(const wstring strName)
{
	auto iter = m_mapBPM.find(strName);

	if (iter == m_mapBPM.end())
		return;
	
	m_iBPM = iter->second;
	m_fBeatInterval = 60.f / m_iBPM;
}

_float CSound_Manager::Get_Timing()
{
	_float nearestBeat = round(m_fElapsedTime / m_fBeatInterval) * m_fBeatInterval;
	return fabs(m_fElapsedTime - nearestBeat);
}

void CSound_Manager::PlaySound(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	
	if (m_mapSound.find(pSoundKey) == m_mapSound.end())
		return;

	auto iter = m_mapSound.find(pSoundKey);

	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, 0, FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlayBGM(const wstring pSoundKey, float fVolume)
{

	if (m_mapSound.find(pSoundKey) == m_mapSound.end())
		return;

	auto iter = m_mapSound.find(pSoundKey);

	FMOD_System_PlaySound(m_pSystem, iter->second, 0, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < SOUND_END; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

}

void CSound_Manager::LoadSoundFile()
{

	_wfinddata_t fd;
	intptr_t hFind = _wfindfirst(L"../../Resource/Sounds/*.*", &fd);
	if (hFind == -1) return;

	do {
		if (fd.attrib & _A_SUBDIR) continue;

		std::wstring fileName = fd.name;
		std::wstring fullPath = L"../../Resource/Sounds/" + fileName;
		std::string fullPathA = WStringToString(fullPath);  // 네 함수 사용

		FMOD_SOUND* pSound = nullptr;
		FMOD_RESULT result = FMOD_System_CreateSound(m_pSystem, fullPathA.c_str(), FMOD_DEFAULT, nullptr, &pSound);

		if (result != FMOD_OK || !pSound) continue;

		m_mapSound[fileName] = pSound;

	} while (_wfindnext(hFind, &fd) == 0);

	_findclose(hFind);
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pSound = new CSound_Manager;
	pSound->Initialize();
	return pSound;
}

void CSound_Manager::Free()
{
	__super::Free();

	for (auto& Mypair : m_mapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);

	Safe_Release(m_pGameInstance);
}
