#pragma once

#include "grid.h"

#include <ranges>
#include <unordered_map>
#include <vector>

using Candidates = Digits;

struct GridCandidates : public GridBase<Candidates>
{
  GridCandidates();
  GridCandidates(const Grid&);
};

GridCandidates Annotated(const Grid&);

using CandidateCount = std::vector<Field*>;
using CandidateCounts = std::unordered_map<Digit, CandidateCount>;

CandidateCounts GetCandidateCounts(std::ranges::viewable_range auto range)
{
  CandidateCounts counts;

  for(std::tuple<Field&, const Candidates&> t : range) {
    auto&& [field, fieldCandidates] = t;

    if(!field.HasValue()) {
      for(auto candidate : fieldCandidates) {
        auto count = counts.find(candidate);
        if(count == end(counts)) {
          count = counts.insert(std::make_pair(candidate, CandidateCount{})).first;
//          count->second.reserve(range.size());
        }
        count->second.push_back(&field);
      }
    }
  }

  for(auto it = begin(counts); it != end(counts);) {
    auto found = std::ranges::contains(
      range | std::views::elements<0>,
      it->first,
      &Field::digit);
    if(found) {
      it = counts.erase(it); // a previous iteration already solved that one -> trim
    } else {
      ++it;
    }
  }

  return counts;
}
