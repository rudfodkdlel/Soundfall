#pragma once


namespace Client
{
	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, EDIT, END };

	// 공통
	// 플레이어는 마우스 terrain에 투영시킨 위치 바라보게
	// 몬스터는 플레이어 바라보게
	typedef enum  eDirState {
		NONE = 0, // idle 일때
		F = 0b0001, B = 0b0010, L = 0b0100, R = 0b1000,
		FL = F | L, FR = F | R, BL = B | L, BR = B | R
	}DIR_STATE;

	// 지속 시간이 있거나 차징 기능이 있으면 in loop out
	enum class ATTACK { ATTACK_IN, ATTACK_LOOP, ATTACK_OUT, END };

	// 플레이어 용
	enum class PLAYER { IDLE, RUN, DASH, ATTACK, HIT, SPWAN };
	enum class WEAPON { ASSAULT, BAZOOKA, GUITAR, KEYBOARD };
	enum class TIMING { BAD, GOOD, END };

	// 몬스터
	enum class MONSTER { IDLE, RUN, ATTACK, HIT, DEAD };

}


