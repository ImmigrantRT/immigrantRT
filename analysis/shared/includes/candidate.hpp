#ifndef CANDIDATE_HPP
#define CANDIDATE_HPP

#include <vector>
#include <functional>
#include <algorithm>

#include "node.hpp"
#include "container.hpp"

/// @param HasEnoughResources Pointer to the function to perform resource availability checks.
///                           Default is `nullptr`
std::vector<Node> GetCandidates(
    std::vector<Node>, Container, 
    std::function<bool(Node, Container)> HasEnoughResources = nullptr
);

void RankCandidates(std::vector<Node>&);

#endif
