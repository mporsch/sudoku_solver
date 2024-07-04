#pragma once

#include "grid.h"

enum class IsSolved
{
  NotYet,
  Never,
  Yes
};

bool operator!(IsSolved);

IsSolved Check(const Grid&);
