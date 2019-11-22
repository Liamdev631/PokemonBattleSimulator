#pragma once
#include <cstdint>
#include <random>

// Multiply damage by a random value between 0.85 and 1.00.
inline void applyRandomMod(uint32_t& numer, uint32_t& denom)
{
	numer *= (85 + rand() % 15);
	denom *= 100;
}

inline void applyMovePower(uint32_t& numer, uint32_t& denom)
{

}

