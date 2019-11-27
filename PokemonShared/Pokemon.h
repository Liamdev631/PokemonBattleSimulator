#pragma once
#include "Stat.h"
#include "Type.h"
#include "Move.h"
#include "Status.h"
#include "Ability.h"

constexpr int NumMoves = 4;
constexpr int PokemonNameLength = 16;

class Pokemon
{
public:

	typedef uint16_t SpeciesID;
	typedef uint16_t Weight;
	typedef uint8_t Level;

public:
	// Species base
	SpeciesID speciesID;
	char name[PokemonNameLength];
	uint16_t baseStats[NumStats]; // Access with Stat enum
	Weight weight;
	Type type1;
	Type type2;

	// Instance based
	MoveID moves[NumMoves];
	uint16_t maximumHealth;
	uint16_t currentHealth;
	Level level;
	Status status;
	Ability ability;
	uint8_t sleepTimer;

	// Default constructor
	Pokemon();

	// Copy constructor
	Pokemon(const Pokemon& other);

	// Pokemon generators
	static Pokemon generate(Pokemon::SpeciesID species, uint8_t level);

	// Acessors
	uint16_t getStatWithMods(const Stat stat) const;

	uint16_t getBaseStat(const Stat stat) const;

	// Battle functionality

	// Fully restores the pokemons status
	void initialize();

	// Causes the pokemon to take damage
	// Returns the amount of damage that was actually done
	uint16_t takeDamage(uint16_t damage);

	uint16_t heal(uint16_t amount);

	bool isFainted() const;

	uint8_t getPercentOfHealth(uint16_t health) const;
};