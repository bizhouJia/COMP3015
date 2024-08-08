#pragma once

#include "drawable.h"
#include "trianglemesh.h"

class Cup : public TriangleMesh
{
public:
    Cup(float topRadius, float bottomRadius, float height, unsigned int segments);
};
