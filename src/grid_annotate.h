#pragma once

#include "grid.h"

using Candidates = Digits;

struct GridCandidates : public GridBase<Candidates>
{
  GridCandidates();
  GridCandidates(const Grid&);
};

GridCandidates Annotated(const Grid&);
