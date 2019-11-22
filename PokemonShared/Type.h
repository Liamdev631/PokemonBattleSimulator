#pragma once
#include <cstdint>

enum struct Type : uint8_t
{
	NONE = 0,
	Normal = 1,
	Fighting = 2,
	Flying = 3,
	Poison = 4,
	Ground = 5,
	Rock = 6,
	Bug = 7,
	Ghost = 8,
	Steel = 9,
	Fire = 10,
	Water = 11,
	Grass = 12,
	Electric = 13,
	Psychic = 14,
	Ice = 15,
	Dragon = 16,
	Dark = 17,
	Fairy = 18,
	Count
};

constexpr size_t NumTypes = 19; // Includes the NONE(0) type
