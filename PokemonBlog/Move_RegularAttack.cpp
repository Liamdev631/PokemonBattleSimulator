#include "Move_RegularAttack.h"
#include "Pokemon.h"
#include "Pokedex.h"
#include "MoveUtil.h"
#include "TestUtil.h"

void Move_RegularAttack::apply(Pokemon* attacker, Pokemon* defender, PlayerAction* attackerAction, PlayerAction* defenderAction) const
{
	auto move = Pokedex::get().getMove(attacker->moves[attackerAction->move]);

	// Make some assertions that should always pass with this move effect
	DebugAssert(move.power != 0, "A RegularAttack move must have a power > 0!");
	DebugAssert(move.damageClass != DamageClass::Status, "A RegularAttack move must be Physical or Special!");

	// Roll to see of the move hits
	int roll = rand() % 100;
	if (roll >= move.accuracy)
	{
		printf("%s's attack missed!\n", attacker->name);
		return;
	}

	uint32_t numer = 1, denom = 1;

	// Calculate the effect of the moves power
	numer *= move.power;

	// Multiply damage by a random value between 0.85 and 1.00.
	applyRandomMod(numer, denom);

	// Roll for critical hit
	if (rand() % 16 == 0)
	{
		printf("A critical hit!\n");
		numer *= 2;
	}

	// Calculate the effect of the attacker and defenders stats
	if (move.damageClass == DamageClass::Physical)
	{
		numer *= attacker->getStatWithMods(Stat::Atk);
		denom *= defender->getStatWithMods(Stat::Def);
	}
	else
	{
		numer *= attacker->getStatWithMods(Stat::SpA);
		denom *= defender->getStatWithMods(Stat::SpD);
	}

	uint16_t damage = numer / denom;
	damage = defender->takeDamage(damage);
	printf("%s took %u damage! (%u%%)\n", defender->name, damage, defender->getPercentOfHealth(damage));
}
