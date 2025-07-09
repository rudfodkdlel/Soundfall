#pragma once

#include "PartObject.h"



namespace Client
{
	typedef struct tagNormalMonsterDesc : public CPartObject::PARTOBJECT_DESC  {
		class CCombatStat* pCombatCom;


	}NORMAL_BODY_DESC;

	typedef struct tagWeaponInfoDesc {
		_bool		isEquip;
		_int		iIndex;
		WEAPON		eWeaponType;
		_wstring	strModelTag;
		_float4		vColor;
	}WEAPON_INFO_DESC;

	typedef struct tagWeaponIconInfoDesc {
		_int		iIndex;
		_int		iTextureType;

	}WEAPON_ICON_INFO_DESC;
}