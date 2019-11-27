#pragma once
#include "Type.h"
#include "DamageClass.h"
#include "MoveEffect.h"

constexpr int MoveNameLength = 16;
constexpr size_t NumberOfMoves = 355;

class Move
{
public:
	typedef uint8_t EffectChance;
	typedef uint8_t PPValue;

	char name[MoveNameLength];
	Type type;
	uint8_t power;
	PPValue pp;
	uint8_t accuracy;
	DamageClass damageClass;
	MoveEffect effectID;
	EffectChance effectChance;
	int8_t priority;

	Move()
		: name("NO NAME"), type(Type::NONE), power(0), pp(0), accuracy(0),
		damageClass(DamageClass::Physical), effectID(MoveEffect::Regular), effectChance(0)
	{

	}
};

enum struct MoveID : uint16_t
{
	NONE = 0,
	Pound = 1,
	KarateChop = 2,
	DoubleSlap = 3,
	CometPunch = 4,
	MegaPunch = 5,
	PayDay = 6,
	FirePunch = 7,
	IcePunch = 8,
	ThunderPunch = 9,
	Scratch = 10,
	ViceGrip = 11,
	Guillotine = 12,
	RazorWind = 13,
	SwordsDance = 14,
	Cut = 15,
	Gust = 16,
	WingAttack = 17,
	Whirlwind = 18,
	Fly = 19,
	Bind = 20,
	Slam = 21,
	VineWhip = 22,
	Stomp,
	DoubleKick,
	MegaKick,
	JumpRick,
	RollingKick,
	SandAttack,
	Headbutt,
	HornAttack = 30,
	FuryAttack,
	HornDrill,
	Tackle,
	BodySlam,
	Wrap,
	TakeDown,
	Thrash,
	DoubleEdge,
	TailWhip,
	PoisonSting = 40,
	Count = NumberOfMoves + 1, // +1 to compensate for the first move entry being NONE
};
