#pragma once

#include "grid.h"

#include <ranges>
#include <type_traits>
#include <unordered_map>
#include <vector>

using Candidates = Digits;

// a grid (corresponding to to the grid of fields) with entries
// containing the unsolved field's solution candidates
struct GridCandidates : public GridOf<Candidates>
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

template<
  typename Range,
  typename MakeContender,
  typename T = std::ranges::range_reference_t<Range>,
  typename R = std::invoke_result_t<MakeContender, T>
>
CandidateContenders<R> GetCandidateContenders(
  Range range,
  MakeContender makeContender)
{
  CandidateContenders<R> candidateContenders;

  for(auto t : range) {
    auto&& candidates = std::get<const Candidates&>(t);

    for(auto candidate : candidates) {
      // can't reserve as range has unknown size
      candidateContenders[candidate].push_back(makeContender(t));
    }
  }

  return candidateContenders;
}

void OrderCandidates(GridCandidates&);
[[nodiscard]] GridCandidates OrderedCandidates(GridCandidates);
