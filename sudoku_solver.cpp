#include "grid.h"
#include "grid_parse.h"
#include "grid_print.h"

#include <cstdlib>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

template<typename T>
T ParseValue(std::string const &arg)
{
  long long num;
  if(!(std::istringstream(arg) >> num) ||
     (num < std::numeric_limits<T>::min()) ||
     (num > std::numeric_limits<T>::max())) {
    throw std::invalid_argument("Invalid number '" + arg + "'");
  }
  return static_cast<T>(num);
}

template<typename T>
T ParseValue(std::string_view arg)
{
  return ParseValue<T>(std::string(arg));
}

int PrintTemplate(unsigned w, unsigned h)
{
  std::cout << Grid(w * 3, h * 3) << std::endl;
  return EXIT_SUCCESS;
}

int PrintTemplate(unsigned w)
{
  return PrintTemplate(w, w);
}

constexpr auto argHelp = std::string_view("--help");
constexpr auto argTemplate = std::string_view("--template");

int Usage(const char* name)
{
  std::cout << "Usage: " << name << "\n"
    << "  [" << argHelp << "]              show this help\n"
    << "  [" << argTemplate << "[=W[xH]]]  print a template of dimensions width x height blocks to use for input\n"
    << std::endl;
  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  if(argc == 2) {
    auto arg = std::string_view(argv[1]);

    if(arg.starts_with(argHelp)) {
      return Usage(argv[0]);
    }

    if(arg.starts_with(argTemplate)) {
      arg.remove_prefix(argTemplate.size());
      if(arg.starts_with('=')) {
        arg.remove_prefix(1);
        if(auto pos = arg.find('x'); pos < arg.size()) {
          return PrintTemplate(
            ParseValue<unsigned>(arg.substr(0, pos)),
            ParseValue<unsigned>(arg.substr(pos + 1, std::string::npos)));
        }
        return PrintTemplate(ParseValue<unsigned>(arg));
      }
      return PrintTemplate(3, 3);
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

  std::cout << "\nParsed grid:\n\n" << grid << "\n";

  return Usage(argv[0]);
}
