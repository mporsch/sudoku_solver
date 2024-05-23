#include "hypervector.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

struct Field
{
	unsigned char num;

	constexpr Field(unsigned char num)
		: num(num)
	{
		if(num >= 10) {
			throw std::invalid_argument("out of range");
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Field&);
};

std::ostream& operator<<(std::ostream& os, const Field& f)
{
	os << ' ' << (f.num ? static_cast<char>(f.num + 48) : ' ') << ' ';
	return os;
}

constexpr auto undef = Field(0);


// Block is 3x3 Fields
using Grid = hypervector<Field, 2>; // width, height always in multiples of 3


struct SepField
{
	const char* str;

	constexpr SepField(const char* str)
		: str(str)
	{
	}

	friend std::ostream& operator<<(std::ostream& os, const SepField&);
};

std::ostream& operator<<(std::ostream& os, const SepField& sep)
{
	os << sep.str;
	return os;
}

constexpr auto sepBlockTop = SepField("+~~~");
constexpr auto sepFieldTop = SepField("+---");
constexpr auto sepBlockLeft = SepField("|");
constexpr auto sepFieldLeft = SepField(":");

std::ostream& operator<<(std::ostream& os, const Grid& grid)
{
	auto sepH = sepBlockTop;
	auto sepW = sepBlockLeft;

	for(size_t h = 0; h < grid.size(1); ++h) {
		for(size_t w = 0; w < grid.size(0); ++w) {
			os << sepH;
		}
		os << "+\n";

		for(size_t w = 0; w < grid.size(0); ++w) {
			os << sepW << grid.at(w, h);
			sepW = ((w + 1) % 3 ? sepFieldLeft : sepBlockLeft);
		}
		os << "|\n";

		sepH = ((h + 1) % 3 ? sepFieldTop : sepBlockTop);
	}
	for(size_t w = 0; w < grid.size(0); ++w) {
		os << sepH;
	}
	os << "+\n";

	return os;
}


void PrintTemplate(unsigned w, unsigned h)
{
	std::cout << Grid(w * 3, h * 3, undef) << std::endl;
}

int Usage(const char* name)
{
	std::cout << "Usage: " << name
		<< "[--help] show this help\n"
		<< "[--template=W[xH]] print a template of dimensions width x height blocks to use for input\n"
		<< std::endl;
	PrintTemplate(3, 3);
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	return Usage(argv[0]);
}
