#pragma once

#include "grid.h"

using Candidates = Digits;

// a grid with entries corresponding to to the grid of fields which
// contain the unsolved field's solution candidates
struct GridCandidates : public GridBase<Candidates>
{
  GridCandidates();
  GridCandidates(const Grid&);
};

void OrderCandidates(GridCandidates&);
