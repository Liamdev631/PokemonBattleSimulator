#pragma once
#include "Move_Base.h"

class Move_RegularAttack : public Move_Base
{
public:
	void apply(Pokemon* attacker, Pokemon* defender, PlayerAction* attackerAction, PlayerAction* defenderAction) const override;
};
