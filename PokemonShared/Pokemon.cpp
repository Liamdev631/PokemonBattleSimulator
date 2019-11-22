#include "Pokemon.h"
#include "Pokedex.h"
#include <memory>
#include <assert.h>

Pokemon::Pokemon(const Pokemon& other)
{
	memcpy(this, &other, sizeof(Pokemon));
}

uint16_t Pokemon::getStatWithMods(const Stat stat) const
{
	uint16_t val = uint16_t(baseStats[(int)stat] * level) / 50;
	if (stat == Stat::Atk)
	{
		if (status == Status::Burned)
		{
			// Guts affects burn differently
			if (ability == Ability::Guts)
				val *= 3;
			val /= 2;
		}
	}
	return val;
}

uint16_t Pokemon::getBaseStat(const Stat stat) const
{
	return static_cast<uint16_t>((baseStats[static_cast<int>(stat)] * level) / 50);
}

void Pokemon::init()
{
#ifdef _DEBUG
	assert((level != 0) && "A Pokemon's level must be > 0");
#endif
	maximumHealth = getBaseStat(Stat::Hp);
	currentHealth = maximumHealth;
	status = Status::None;
	sleepTimer = 0;
}

uint16_t Pokemon::takeDamage(uint16_t damage)
{
	if (currentHealth < damage)
		damage = currentHealth;
	currentHealth -= damage;
	return damage;
}

uint16_t Pokemon::heal(uint16_t amount)
{
	if (currentHealth + amount > maximumHealth)
		amount = maximumHealth - currentHealth;
	currentHealth += amount;
	return amount;
}

bool Pokemon::isFainted() const
{
	return currentHealth == 0;
}

uint8_t Pokemon::getPercentOfHealth(uint16_t health) const
{
	return static_cast<uint8_t>(health * 100 / maximumHealth);
}

Pokemon Pokemon::generate(Pokemon::SpeciesID species, uint8_t level)
{
	assert(species < NumberOfPokemon + 1);
	Pokemon mon = Pokedex::get().getSpecies(species);
	mon.level = level;
	return mon;
}
