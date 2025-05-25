#pragma once

#include "Base.h"
#include "Channel.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	ANIM_DESC Get_Desc()
	{
		ANIM_DESC eDesc = {};
		eDesc.iNumChannels = m_iNumChannels;
		for (int i = 0; i < m_iNumChannels; ++i)
		{
			CHANNEL_DESC cDesc = m_Channels[i]->Get_Desc();
			eDesc.Channels.push_back(cDesc);
		}
		eDesc.fDuration = m_fDuration;
		eDesc.fTickPerSecond = m_fTickPerSecond;

		return eDesc;
	}

public:
	HRESULT Initialize(const ANIM_DESC& eDesc, const vector<class CBone*>& Bones);
	_bool Update_Bones(_float fTimeDelta, const vector<CBone*>& Bones, _bool isLoop);
	void  Reset();

private:
	/* 전체 재생 거리. */
	_float					m_fDuration = {};
	_float					m_fTickPerSecond = {};
	_float					m_fCurrentTrackPosition = {};
	vector<_uint>			m_CurrentKeyFrameIndices;

	/* 이 애니메이션을 표현하기위해서 사용하는 뼈의 갯수 */
	_uint					m_iNumChannels;
	vector<CChannel*>		m_Channels;

public:
	static CAnimation* Create(const ANIM_DESC& eDesc, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END