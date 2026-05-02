#pragma once

#include "grid.h"

#include <ranges>
#include <type_traits>
#include <unordered_map>
#include <vector>

// the possible solutions for a yet-unsolved field constrained by the 3 groups the field belongs to (row, column, block)
// if only one candidate exists for a field, it is a "Single" (or singleton, or lone number) and can be solved immediately
using Candidates = Digits;

// a grid (corresponding to to the grid of fields) with entries
// containing the unsolved field's solution candidates (zero candidates for already solved fields)
struct GridCandidates : public GridOf<Candidates>
{
  GridCandidates();
  GridCandidates(const Grid&);
};

// within a group (either row, column or block) a list of cells that share a common candidate
// if only one cell allows this candidate, it is a "Hidden single"--a candidate that appears with others,
// but only once in a given row, column or block--and can be solved immediately
template<typename Contender> // abstract data referring to other grid cells' candidates
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

// sort the candidates by uniqueness
void OrderCandidates(GridCandidates&);
[[nodiscard]] GridCandidates OrderedCandidates(GridCandidates);
