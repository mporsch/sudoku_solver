#include "grid_check.h"
#include "grid_check_impl.h"

IsSolved Check(const Grid& grid)
{
  auto checker = Checker{grid};
  (void)!checker; // this is a concession for using all_of internally
  return checker.check.result;
}
