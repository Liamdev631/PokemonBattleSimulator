#pragma once
#include "Pokemon.h"
#include <memory>
#include <array>
#include <map>
#include "PlayerAction.h"

using namespace std;

constexpr auto PartySize = 1;
constexpr auto PlayerCount = 2;

typedef array<Pokemon, PartySize> Team;
typedef array<PlayerAction, PlayerCount> ActionBuffer;

class Move_Base;

class Battle
{
public:
	bool needsTick;
	bool isOver;
	bool isActive;

private:
	array<Team, PlayerCount> activePokemon;
	bool _isActionSubmitted[PlayerCount];
	int turnCount;
	ActionBuffer _actionBuffer;
	
	array<Move_Base*, NumberOfMoves> _moveTable;

	void setupMoveTable();

public:
	Battle();
	~Battle();

	void init(const array<Team, PlayerCount>& teams);
	void stop();

	// Updates the game state. Most the the battle processing runs in here.
	// This method may send packets to users.
	void tick();

	void submitPlayerAction(PlayerAction action);

private:
	void simulateAttack(Pokemon& attacker, Pokemon& defender, PlayerAction& attackerAction, PlayerAction& defenderAction);

	// Wins the game for the given player, ending the battle
	void win(int player);

	void addBattleText(const char* text);

	void addBattleText(const string& text);

	// Switches the player's pokemon based on the action in _actionBuffer
	void switchPokemon(int player);
};

