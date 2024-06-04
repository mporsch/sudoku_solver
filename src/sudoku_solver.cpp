#include "args_parse.h"
#include "grid.h"
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

int PrintTemplate(unsigned w, unsigned h)
{
  std::cout << Grid(h * 3, w * 3) << std::endl;
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
    std::cerr << "\nParsed grid:\n\n";
    std::cout << grid << "\n";
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
      return PrintTemplate(args.templ->width, args.templ->height);
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
