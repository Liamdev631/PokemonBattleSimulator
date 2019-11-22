#include "Battle.h"
#include "Pokedex.h"
#include <queue>
#include <assert.h>
#include "MoveEffect.h"
#include "TestUtil.h"
#include <string>
#include "PlayerAction.h"

#include "Move_Base.h"
#include "Move_RegularAttack.h"

int otherPlayer(int player)
{
	int id = 1 - player;
	DebugAssert(id == 1 || id == 0, "otherPlayer bug");
	DebugAssert(id != player, "otherPlayer bug");
	return id;
}

Battle::Battle()
	: isActive(false), needsTick(false), isOver(false)
{
	setupMoveTable();
}

Battle::~Battle()
{

}

void Battle::tick()
{
	DebugAssert(isActive, "Battle::tick(), tried to tick while isActive is false");

	// Continue if all the inputs needed have been received
	for (int i = 0; i < PlayerCount; i++)
		if (!_isActionSubmitted[i])
			return;
	for (int i = 0; i < PlayerCount; i++)
		_isActionSubmitted[i] = false;

	// Fastest pokemon moves first
	array<int, 2> moveOrders;
	if (activePokemon[0][0].getStatWithMods(Stat::Spe) > activePokemon[1][0].getStatWithMods(Stat::Spe))
		moveOrders = { 0, 1 };
	else
		moveOrders = { 1, 0 };

	// Takes priority
	for (int i = 0; i < 2; i++)
		if (_actionBuffer[moveOrders[i]].type == PlayerActionType::Switch)
			switchPokemon(moveOrders[0]);

	for (int i = 0; i < 2; i++)
	{
		// Check if the battle has stopped since the last attack
		if (isOver)
			break; 

		auto& attacker = activePokemon[moveOrders[i]][0];
		auto& defender = activePokemon[moveOrders[otherPlayer(i)]][0];
		auto& attackerAction = _actionBuffer[moveOrders[i]];
		auto& defenderAction = _actionBuffer[moveOrders[otherPlayer(i)]];

		// Update sleep mechanics
		if (attacker.status == Status::Sleep)
		{
			DebugAssert(attacker.sleepTimer > 0, "Pokemon is asleep but sleepTimer is 0");
			if (attacker.sleepTimer-- == 0)
			{
				// Wake up
				attacker.status = Status::None;
				addBattleText(string(attacker.name) + " woke up!");
			}
			else
				// Skip the rest of the turn
				continue;
		}

		simulateAttack(attacker, defender, attackerAction, defenderAction);
	}
}

void Battle::init(const array<Team, PlayerCount>& teams)
{
#ifdef _DEBUG
	if (isActive)
	{
		printf("Warning! Battle::init() called on a battle that has already been initialized!\n");
		return;
	}
#endif

	for (int player = 0; player < PlayerCount; player++)
		for (int mon = 0; mon < PartySize; mon++)
		{
			auto& slot = activePokemon[player][mon];
			slot = teams[player][mon];
			slot.init();
		}

	turnCount = 1;
	needsTick = true;
	isOver = false;
	isActive = true;
}

void Battle::stop()
{
	isActive = false;
}

void Battle::submitPlayerAction(PlayerAction action)
{
	DebugAssert(action.user < PlayerCount, "Battle::submitPlayerAction, invalid user");
	DebugAssert(isActive, "Battle::submitPlayerAction, battle is inactive");

	// If the action is a forfeit command, end the battle immediatly
	if (action.type == PlayerActionType::Forfeit)
	{
		addBattleText("Player " + to_string(action.user) + " forfeited!");
		win(otherPlayer(action.user));
	}
		
	_actionBuffer[action.user] = action;
	_isActionSubmitted[action.user] = true;

	// Determine if all the inputs have been received or not
	needsTick = true;
	for (int i = 0; i < PlayerCount; i++)
		if (!_isActionSubmitted[i])
			needsTick = false;
}

void Battle::simulateAttack(Pokemon& attacker, Pokemon& defender, PlayerAction& attackerAction, PlayerAction& defenderAction)
{
	assert(attackerAction.type == PlayerActionType::Attack);
	auto& move = Pokedex::get().getMove(attackerAction.move);
	printf("%s used %s.\n", attacker.name, move.name);

	// Call the attacks function from the table
	_moveTable[(int)move.effectID]->apply(&attacker, &defender, &attackerAction, &defenderAction);

	// Check if the defending pokemon was knocked out
	if (defender.isFainted())
	{
		printf("%s fainted!\n", defender.name);
		win(attackerAction.user);
		return;
	}
}

void Battle::setupMoveTable()
{
	_moveTable[static_cast<int>(MoveEffect::Regular)] = new Move_RegularAttack();
}

void Battle::win(int player)
{
	addBattleText("Player " + to_string(player) + " has won the battle!");
	needsTick = false;
	isOver = true;
}

void Battle::addBattleText(const char* text)
{
	printf("%s\n", text);
}

void Battle::addBattleText(const string& text)
{
	addBattleText(text.c_str());
}

void Battle::switchPokemon(int player)
{

}
