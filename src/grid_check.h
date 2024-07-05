#pragma once

#include "grid.h"

enum class IsSolved
{
  Yes,
  NotYet,
  Never
};

IsSolved Check(const Grid&);
