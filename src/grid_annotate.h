#pragma once

#include "grid.h"

#include <ranges>
#include <unordered_map>
#include <vector>

using Candidates = Digits;

// a grid (corresponding to to the grid of fields) with entries
// containing the unsolved field's solution candidates
struct GridCandidates : public GridBase<Candidates>
{
  GridCandidates();
  GridCandidates(const Grid&);
};

// abstract data referring to other grid cells candidates
template<typename Contender>
using Contenders = std::vector<Contender>;

// a map of candidate digit -> list of other grid cells' data
template<typename Contender>
using CandidateContenders = std::unordered_map<Digit, Contenders<Contender>>;

template<typename Contender, typename... Entries>
CandidateContenders<Contender>
GetCandidateContenders(std::ranges::viewable_range auto range)
{
  CandidateContenders<Contender> candidateContenders;

  for(std::tuple<Entries...> t : range) {
    auto&& candidates = std::get<const Candidates&>(t);

    for(auto candidate : candidates) {
      // can't reserve as range has unknown size
      candidateContenders[candidate].push_back(t);
    }
  }

  return candidateContenders;
}

void OrderCandidates(GridCandidates&);
