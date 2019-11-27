#include "Pokedex.h"
#include "CSVReader.h"
#include <assert.h>
#include <string>

Pokedex::Pokedex()
{
	_species = make_unique<Pokemon[]>(NumberOfPokemon + 1);

	CSVReader file;
	file.open("pokemon_stats.csv");
	file.readNextRow(); // Skip the header
	for (int i = 1; i < NumberOfPokemon + 1; i++)
		for (int j = 0; j < 6; j++)
		{
			file.readNextRow();
			_species[i].baseStats[j] = stoi(file[2]);
		}

	// Read pokemon names
	file.open("pokemon.csv");
	file.readNextRow(); // Skip the header
	while (file.size() > 1)
	{
		file.readNextRow();
		const int id = stoi(file[2]);
		if (id > NumberOfPokemon)
			break;
		auto& mon = _species[id];
		strcpy_s<PokemonNameLength>(mon.name, file[1].c_str());
		mon.weight = stoi(file[4]);
	}

	// Read type names
	for (int i = 0; i < NumTypes; i++)
		strcpy_s<TypeNameLength>(_typeNames[0][i], "NONE");
	for (int i = 0; i < NumLanguages; i++)
		strcpy_s<TypeNameLength>(_typeNames[i][0], "NONE");
	file.open("type_names.csv");
	file.readNextRow(); // Skip the header
	while (true)
	{
		file.readNextRow();
		if (stoi(file[0]) < NumTypes)
			break;
		const int type = stoi(file[0]);
		const int lang = stoi(file[1]);
		strcpy_s<TypeNameLength>(_typeNames[lang][type], file[2].c_str());
	}

	// Read the primary and secondary types of each pokemon
	for (int i = 1; i < NumberOfPokemon + 1; i++)
		_species[i].type2 = Type::NONE; // Not all mons have a second type
	file.open("pokemon_types.csv");
	file.readNextRow(); // Skip the header
	while (true)
	{
		file.readNextRow();
		if (stoi(file[0]) > NumberOfPokemon + 1)
			break;
		if (file[2][0] == '1')
			_species[stoi(file[0]) - 1].type1 = (Type)(stoi(file[1]));
		else
			_species[stoi(file[0]) - 1].type2 = (Type)(stoi(file[1]));
	}

	// Load the type advantages table
	file.open("type_advantage.csv");
	file.readNextRow();
	file.readNextRow();
	while (file.size() > 1)
	{
		_typeAdvantage[stoi(file[0])][stoi(file[1])] = stoi(file[2]) / 25;
		file.readNextRow();
	}

	// Load the move data
	file.open("moves.csv");
	file.readNextRow();
	for (int i = 1; i < NumberOfMoves + 1; i++)
	{
		file.readNextRow();
		auto& move = _moves[i];
		
		// Load name
		strcpy_s<MoveNameLength>(move.name, file[1].c_str());

		// Load type
		move.type = static_cast<Type>(stoi(file[3]));

		// Load power
		if (file[4].empty())
			move.power = 0;
		else
			move.power = stoi(file[4]);

		// Load pp
		move.pp = stoi(file[5]);

		// Load accuracy
		if (file[6].empty())
			move.accuracy = 0;
		else
			move.accuracy = stoi(file[6]);

		// Load priority
		// 8 is 10 for single target, 11 for double target, 9 for all other pokemon,
		// 7 for no target, 12 for all pokemon. Might be a bitfield
		move.priority = stoi(file[7]);

		// Load damage class
		move.damageClass = static_cast<DamageClass>(stoi(file[9]));

		// Load effect ID
		move.effectID = static_cast<MoveEffect>(stoi(file[10]));

		// Load effect chance
		if (file[11].empty())
			move.effectChance = 0;
		else
			move.effectChance = stoi(file[11]);
	}
}

Pokedex::~Pokedex()
{

}

Pokedex& Pokedex::get()
{
	static Pokedex p;
	return p;
}

Pokemon& Pokedex::getSpecies(Pokemon::SpeciesID species) const
{
	return _species[species];
}

const Move& Pokedex::getMove(MoveID moveID) const
{
	assert(static_cast<int>(moveID) < NumberOfMoves);
	return _moves[static_cast<int>(moveID)];
}

const char* Pokedex::getTypeName(Type type, Language language) const
{
	return _typeNames[(int)language][(int)type];
}