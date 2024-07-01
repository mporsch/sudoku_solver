#include "args_parse.h"
#include "grid.h"
#include "grid_check.h"
#include "grid_parse.h"
#include "grid_print.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

int PrintUsage(std::ostream& os, const char* name, int code = EXIT_SUCCESS)
{
  os << Usage{name} << std::endl;
  return code;
}

int PrintTemplate(const Args::Template& templ)
{
  auto grid = Grid(templ.height, templ.width, templ.blockHeight, templ.blockWidth);
  std::cerr
    << "Grid:"
    << ' ' << grid.width() << 'x' << grid.height()
    << ',' << grid.blockWidth << 'x' << grid.blockHeight
    << "\n\n";
  std::cout << grid << std::endl;
  return EXIT_SUCCESS;
}

Grid ParseGrid(std::istream& is)
{
  Grid grid;
  is >> grid;
  return grid;
}

int ProcessGrid(Grid grid)
{
  if(grid.size()) {
    std::cerr
      << "\nParsed grid:"
      << ' ' << grid.width() << 'x' << grid.height()
      << ',' << grid.blockWidth << 'x' << grid.blockHeight
      << "\n\n";
    std::cout << grid << "\n";

    std::cerr << "Is " << (IsSolved(grid) ? "" : "not ") << "solved\n\n";

    return EXIT_SUCCESS;
  }
  std::cerr << "\nFailed to parse grid\n\n";
  return EXIT_FAILURE;
}

int main(int argc, char** argv)
{
  if(argc >= 2) {
    auto args = ParseArgs(argc, argv);
    if(args.help) {
      return PrintUsage(std::cout, argv[0]);
    }
    if(args.templ) {
      return PrintTemplate(*args.templ);
    }
    if(args.stdinput) {
      std::cerr << "\nPaste grid and end input with Ctrl-D / Ctrl-Z+Enter:\n\n";
      return ProcessGrid(ParseGrid(std::cin));
    }
    if(!args.filepath.empty()) {
      std::ifstream ifs(args.filepath);
      return ProcessGrid(ParseGrid(ifs));
    }
  }

  return PrintUsage(std::cerr, argv[0], EXIT_FAILURE);
}
