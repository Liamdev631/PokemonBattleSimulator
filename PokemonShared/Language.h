#pragma once
#include <cstdint>

enum struct Language : uint8_t
{
	None0 = 0,
	Japanese = 1,
	None2 = 2,
	Korean = 3,
	ChineseTraditional = 4,
	French = 5,
	German = 6,
	Spanish = 7,
	Italian = 8,
	English = 9,
	None10 = 10,
	Japanese2 = 11,
	ChineseSimplified = 12,
	Count = 13,
};

constexpr size_t NumLanguages = 13;

/*static const string LanguageNames[12] = 
{
	"NO LANG",//0
	"Japanese",//1
	"NO LANG",//2
	"Korean",//3
	"Traditional Chinese",//4
	"French",//5
	"German",//6
	"Spanish",//7
	"Italian",//8
	"English",//9
	"NO LANG",//10
	"Japanese"//11
	"Simplified Chinese",//12
}/**/
