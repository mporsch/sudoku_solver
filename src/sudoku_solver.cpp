#include "args_parse.h"
#include "custom_step_iterator.h"
#include "grid.h"
#include "grid_parse.h"
#include "grid_print.h"

#include <algorithm>
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

bool IsSolved(Grid::container sorted)
{
  std::sort(begin(sorted), end(sorted));

  if(sorted.front().num == Field::undef) {
    return false;
  }
  if(std::unique(begin(sorted), end(sorted)) != end(sorted)) {
    return false;
  }
  return true;
}

bool IsSolved(const Grid& grid)
{
  // check rows
  for(size_t h = 0; h < grid.height(); ++h) {
    auto first = &grid.at(h, 0);
    auto last = &grid.at(h, grid.width());

    if(!IsSolved(Grid::container(first, last))) {
      return false;
    }
  }

  // check columns
  for(size_t w = 0; w < grid.width(); ++w) {
    auto first = make_custom_step_iterator(
      std::next(grid.begin(), w),
      grid.offsetOf<0>());
    auto last = std::next(first, 1);

    if(!IsSolved(Grid::container(first, last))) {
      return false;
    }
  }

  // TODO Block

  return true;
}

int ProcessGrid(Grid grid)
{
  if(grid.size()) {
    std::cerr << "\nParsed grid:\n\n";
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
