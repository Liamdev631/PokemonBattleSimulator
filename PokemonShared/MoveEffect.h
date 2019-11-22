#pragma once
#include <stdint.h>

enum struct MoveEffect : uint16_t
{
	Regular = 1,
	SleepTarget = 2,
	PoisonTarget = 3,
	HealForHalfDamage = 4,
	BurnTarget = 5,
	FreezeTarget = 6,
	ParalyzeTarget = 7,
	UserFaints = 8,
	DreamEater = 9,
	CopyTargetsLastMove = 10,
};
