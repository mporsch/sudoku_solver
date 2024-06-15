#include "grid_check.h"
#include "grid_check_impl.h"

bool IsSolved(const Grid& grid)
{
  return Checker{grid};
}
