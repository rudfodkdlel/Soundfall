#pragma once

#include "Base.h"

/* 특정 애니메이션을 구동하기위해서 필요한, 뼈의 시간당 상태정보*/

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	CHANNEL_DESC Get_Desc() const
	{
		CHANNEL_DESC eDesc = {};
		strcpy_s(eDesc.szName, MAX_PATH, m_szName);
		eDesc.iNumKeyFrames = m_iNumKeyFrames;
		eDesc.KeyFrames = m_KeyFrames;
		eDesc.iBoneIndex = m_iBoneIndex;

		return eDesc;
	}

	


public:
	HRESULT Initialize(const CHANNEL_DESC& eDesc, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_uint* pCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<class CBone*>& Bones);

private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iNumKeyFrames;
	vector<KEYFRAME>	m_KeyFrames;
	_uint				m_iBoneIndex = {};
	//_uint				m_iCurrentKeyFrameIndex = {};


public:
	static CChannel* Create(const CHANNEL_DESC& eDesc, const vector<class CBone*>& Bones);
	virtual void Free() override;

};

NS_END