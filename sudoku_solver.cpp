#include "args.h"
#include "grid.h"
#include "grid_parse.h"
#include "grid_print.h"

#include <cstdlib>
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
  }

  std::cerr << "\nPaste grid and end input with Ctrl-D:\n\n";

  Grid grid;
  std::cin >> grid;

  if(grid.size()) {
    std::cerr << "\n\nParsed grid:\n\n";
    std::cout << grid << "\n";
    return EXIT_SUCCESS;
  }

  std::cerr << "\nFailed to parse grid\n\n";
  return PrintUsage(std::cerr, argv[0], EXIT_FAILURE);
}
