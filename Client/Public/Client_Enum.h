#pragma once


namespace Client
{
	enum class LEVEL { STATIC, LOADING, LOGO, LOBBY, GAMEPLAY, BOSS, EDIT, END };

	enum class PART_DEFAULT { BODY, WEAPON, UI, EFFECT, END };
	
	enum class ANIMSTATE {ANIM_IN, ANIM_LOOP, ANIM_OUT};

	enum class OBJTYPE { PLAYER, MOSNTER, BOSS, END };

	typedef enum  eDirState {
		NONE = 0, // idle 일때
		F = 0b0001, B = 0b0010, L = 0b0100, R = 0b1000,
		FL = F | L, FR = F | R, BL = B | L, BR = B | R
	}DIR_STATE;

	// 지속 시간이 있거나 차징 기능이 있으면 in loop out
	enum class ATTACK { ATTACK_IN, ATTACK_LOOP, ATTACK_OUT, END };

	// 플레이어 용
	enum class STATE_MAIN { IDLE, MOVE, DASH, ATTACK, HIT, STUN, CHANGE , DEAD, SPWAN };
	enum class STATE_SUB {NONE, MOVEABLE, CHARGE, COMBO, ULT};
	enum class WEAPON { ASSAULT, BAZOOKA, GUITAR, KEYBOARD, AXE, SICKLE, END };
	enum class TIMING { BAD, GOOD, END };

	// 몬스터
	enum class MONSTER { IDLE, MOVE, ATTACK, HIT, DEAD };

	enum BOSS_PATTERN { BP_MELEE, BP_ECHOSLAM, BP_SPAWN, BP_WALL, BP_MISSILE, BP_END};

}


