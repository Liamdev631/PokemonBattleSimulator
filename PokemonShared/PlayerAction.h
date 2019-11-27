#pragma once

enum struct PlayerActionType : uint8_t
{
	Attack,
	Switch,
	Forfeit,
};

struct PlayerAction
{
	uint8_t user;
	PlayerActionType type;
	union
	{
		uint8_t move;
		uint8_t pokemon;
	};

	PlayerAction()
		: type(PlayerActionType::Attack), user(0), move(0)
	{

	}
	PlayerAction(PlayerActionType type)
		: type(type), user(0), move(0)
	{

	}
};
