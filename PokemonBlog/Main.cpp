// PokemonBlog.cpp : Defines the entry point for the console application.
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "Pokedex.h"
#include "Battle.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

constexpr auto MaxNumBattles = 128;
Battle battles[MaxNumBattles];

constexpr int MaxTicksPerSecond = 10;

Battle* getNewBattle()
{
	for (int i = 0; i < MaxNumBattles; i++)
		if (!battles[i].isActive)
			return &battles[i];
	return nullptr;
}

int main()
{
	Pokedex::get();

	// Create a sample battle
	auto b = getNewBattle();
	b->init({ Pokemon::generate(6, 40), Pokemon::generate(9, 40) });

	// Run the main server loop
	chrono::system_clock::time_point lastTickTime = chrono::system_clock::now();
	printf("Starting main server loop!\n");
	while (true)
	{
		for (int i = 0; i < MaxNumBattles; i++)
		{
			auto& battle = battles[i];
			if (!battle.isActive)
				continue;
			if (battle.needsTick)
			{
				battle.tick();

				PlayerAction action(PlayerActionType::Attack);
				action.move = 0;
				for (int i = 0; i < 2; i++)
				{
					action.user = i;
					b->submitPlayerAction(action);
				}
			}
			if (battle.isOver)
			{
				printf("Battle %i has finished!\n", i);
				battle.stop();
			}
		}

		std::this_thread::sleep_until(lastTickTime + 1s / MaxTicksPerSecond);
		lastTickTime = chrono::system_clock::now();
	}

    return 0;
}

