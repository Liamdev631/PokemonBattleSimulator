#pragma once
#include <memory>
#include <map>
#include "PlayerAction.h"
#include "BattleState.h"

using namespace std;

constexpr auto PartySize = 1;
constexpr auto PlayerCount = 2;

typedef array<PlayerAction, PlayerCount> ActionBuffer;
typedef BattleState<PartySize, PlayerCount> State;

class Move_Base;

class Battle
{
public:
	bool needsTick;
	bool isOver;
	bool isActive;

private:
	// Holds the current state of the battle
	State _state;

	bool _isActionSubmitted[PlayerCount];
	
	// The turn number. The first turn has a value of 1
	int turnCount;

	// Holds the battles current user input
	ActionBuffer _actionBuffer;
	
	// Holds references to the functions that hold the code for moves
	array<Move_Base*, NumberOfMoves> _moveTable;

	// Initializes the move table for this type of battle
	void setupMoveTable();

public:
	Battle();
	~Battle();

	void init(const State::TeamSet& teams);
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

