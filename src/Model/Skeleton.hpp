#pragma once

#include "Mesh.hpp"

struct Skeleton
{
    int numSubsets;
    int numJoints;

    std::vector<Joint> joints;
    std::vector<Mesh> subsets;
};
