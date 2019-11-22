#pragma once
#include <stdint.h>

typedef uint16_t uint16_t;

enum struct Stat
{
	Hp = 0,
	Atk = 1,
	Def = 2,
	SpA = 3,
	SpD = 4,
	Spe = 5,
};

constexpr int NumStats = 6;
