#include "grid_check.h"
#include "grid_check_impl.h"

bool operator!(IsSolved e)
{
  switch(e) {
    case IsSolved::NotYet:
    case IsSolved::Never:
      return true;
    default:
      return false;
  }
}

IsSolved Check(const Grid& grid)
{
  auto checker = Checker{grid};
  (void)!checker; // this is a concession for using all_of internally
  return checker.check.result;
}
