#pragma once

#include "Mesh.h"

struct Skeleton
{
    int numSubsets;
    int numJoints;

    std::vector<Joint> joints;
    std::vector<Mesh> subsets;
};
