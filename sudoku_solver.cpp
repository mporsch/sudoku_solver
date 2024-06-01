#include "args.h"
#include "grid.h"
#include "grid_parse.h"
#include "grid_print.h"

#include <cstdlib>
#include <sstream>

int PrintUsage(std::ostream& os, const char* name)
{
  os << Usage{name} << std::endl;
  return EXIT_SUCCESS;
}

int PrintTemplate(unsigned w, unsigned h)
{
  std::cout << Grid(w * 3, h * 3) << std::endl;
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

  auto input = R"(
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
|   : 3 :   |   :   :   |   :   :   |
+---+---+---+---+---+---+---+---+---+
|   :   :   | 1 : 9 : 5 |   :   :   |
+---+---+---+---+---+---+---+---+---+
|   :   : 8 |   :   :   |   : 6 :   |
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
| 8 :   :   |   : 6 :   |   :   :   |
+---+---+---+---+---+---+---+---+---+
| 4 :   :   | 8 :   :   |   :   : 1 |
+---+---+---+---+---+---+---+---+---+
|   :   :   |   : 2 :   |   :   :   |
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
|   : 6 :   |   :   :   | 2 : 8 :   |
+---+---+---+---+---+---+---+---+---+
|   :   :   | 4 : 1 : 9 |   :   : 5 |
+---+---+---+---+---+---+---+---+---+
|   :   :   |   :   :   |   : 7 :   |
+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
)";
  std::istringstream iss(input);

  Grid grid;
  iss >> grid;

  std::cerr << "\nParsed grid:\n\n";
  std::cout << grid << "\n";

  return PrintUsage(std::cerr, argv[0]);
}
