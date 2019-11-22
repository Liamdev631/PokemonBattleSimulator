#pragma once
#include "Move.h"
#include "PlayerAction.h"

class Pokemon;

class Move_Base
{
public:
	virtual void apply(Pokemon* attacker, Pokemon* defender, PlayerAction* attackerAction, PlayerAction* defenderAction) const = 0;
};

