#pragma once
//#include "Pokemon.h"
#include "Language.h"
#include <array>
#include <memory>
#include "Pokemon.h"

using namespace std;

constexpr size_t NumberOfPokemon = 151;
constexpr size_t TypeNameLength = 16;

// Type advantage table types
typedef array<int, NumTypes> TypeAdvantageArray;
typedef array<TypeAdvantageArray, NumTypes> TypeAdvantageTable;

// A c-string type for Type names.
typedef char TypeName[TypeNameLength];
// An array containing the c-string names of each Type.
typedef array<TypeName, NumTypes> TypeNameArray;
// A table of names for each type in each language.
typedef array<TypeNameArray, NumLanguages> TypeNameTable;

class Pokedex
{
private:
	Pokedex();
	
	unique_ptr<Pokemon[]> _species;// [NumberOfPokemon + 1] ;
	Move _moves[NumberOfMoves + 1];
	int8_t _typeAdvantage[NumTypes][NumTypes];

	// Array of type names for each language
	TypeNameTable _typeNames;

public:
	static Pokedex& get();
	~Pokedex();

	Pokemon& getSpecies(Pokemon::SpeciesID species) const;
	const Move& getMove(MoveID move) const;
	const char* getTypeName(Type type, Language language = Language::English) const;
};

