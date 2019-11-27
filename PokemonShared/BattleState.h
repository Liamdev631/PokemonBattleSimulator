#pragma once
#include "Pokemon.h"
#include <array>

using namespace std;

template <int TeamSize>
using Team = array<Pokemon, TeamSize>;


template <int TeamSize, int NumTeams>
struct BattleState
{
	typedef array<Team<TeamSize>, NumTeams> TeamSet;

	union
	{
		array<Pokemon, TeamSize * NumTeams> pokemons;
		TeamSet teams;
	};

	// Default Constructor
	// Defaults to all empty teams
	BattleState();

	// Constructor
	// Initializes the battle state from a given set of teams
	BattleState(const TeamSet& newTeams);

	// Initializes the battle state from the currently set teams
	void initialize();
};

template <int TeamSize, int NumTeams>
BattleState<TeamSize, NumTeams>::BattleState<TeamSize, NumTeams>()
{
	for (int team = 0; team < NumTeams; team++)
		for (int mon = 0; mon < TeamSize; mon++)
			teams[team][mon] = Pokemon();
}

template <int TeamSize, int NumTeams>
BattleState<TeamSize, NumTeams>::BattleState<TeamSize, NumTeams>(const BattleState<TeamSize, NumTeams>::TeamSet& newTeams)
{
	for (int team = 0; team < NumTeams; team++)
		for (int mon = 0; mon < TeamSize; mon++)
			teams[team][mon] = newTeams[team][mon];
	initialize();
}

template <int TeamSize, int NumTeams>
void BattleState<TeamSize, NumTeams>::initialize()
{
	for (int mon = 0; mon < TeamSize * NumTeams; mon++)
		pokemons[mon].initialize();
}

