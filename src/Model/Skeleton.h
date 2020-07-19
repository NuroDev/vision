#pragma once

#include "Mesh.h"

struct Bone
{
    // Position of the bone in final array
    int id = 0;

	// Name of the bone loaded from file
    std::string name = "Unnamed bone";

	// Each bone has it's one 4x4 transformation matrix
    XMMATRIX offset;

	// All the bones child bones (EG: 
    std::vector<Bone> children = {};
};
