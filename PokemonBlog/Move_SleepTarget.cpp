#include "Move_SleepTarget.h"
#include "Pokemon.h"
#include "Pokedex.h"
#include "MoveUtil.h"
#include "TestUtil.h"

void Move_SleepTarget::apply(Pokemon* attacker, Pokemon* defender, PlayerAction* attackerAction, PlayerAction* defenderAction) const
{
	auto move = Pokedex::get().getMove(attacker->moves[attackerAction->move]);

	// Make some assertions that should always pass with this move effect
	DebugAssert(move.power == 0, "A SleepTarget move must have a power of 0!");
	DebugAssert(move.damageClass == DamageClass::Status, "A SleepTarget move must be a Status move!");

	// Roll to see of the move hits
	int roll = rand() % 100;
	if (roll >= move.accuracy)
	{
		printf("%s's attack missed!\n", attacker->name);
		return;
	}

	// Check if target is already statused
	if (defender->status != Status::None)
	{
		printf("$s's attack failed! (The defending Pokemon is already statused!\n");
		return;
	}

	defender->status = Status::Sleep;
	defender->sleepTimer = 2 + rand() % 4;
}
