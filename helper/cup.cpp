#include "cup.h"
#include <glad/glad.h>
#include <cstdio>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

using namespace std;

// Constructor for the Cup class
Cup::Cup(float topRadius, float bottomRadius, float height, unsigned int segments) {
    // Vector to store vertices of the cup
    vector<GLfloat> p;
    // Vector to store indices for elements (triangles)
    vector<GLuint> el;
    // Vector to store normals for each vertex
    vector<GLfloat> n;

    // Angle step based on the number of segments
    float step = 360.0f / segments;
    for (unsigned int i = 0; i <= segments; ++i) {
        // Convert step to radians for trigonometric functions
        float angle = glm::radians(step * i);
        // Calculate top circle points using polar coordinates
        float xTop = topRadius * cos(angle);
        float zTop = topRadius * sin(angle);
        // Calculate bottom circle points using polar coordinates
        float xBottom = bottomRadius * cos(angle);
        float zBottom = bottomRadius * sin(angle);

        // Push back top points to the vertices vector
        p.push_back(xTop); p.push_back(height / 2.0); p.push_back(zTop);
        // Normals for top points (upwards - y direction)
        n.push_back(0.0f); n.push_back(1.0f); n.push_back(0.0f);

        // Push back bottom points to the vertices vector
        p.push_back(xBottom); p.push_back(-height / 2.0); p.push_back(zBottom);
        // Normals for bottom points (downwards - negative y direction)
        n.push_back(0.0f); n.push_back(-1.0f); n.push_back(0.0f);

        // Only add indices if it's not the last segment to avoid wrapping incorrectly
        if (i != segments) {
            // Add indices for the top and bottom triangles of this segment
            el.push_back(i * 2);
            el.push_back(i * 2 + 1);
            el.push_back((i + 1) * 2);

            el.push_back(i * 2 + 1);
            el.push_back((i + 1) * 2 + 1);
            el.push_back((i + 1) * 2);
        }
    }

    // Call initBuffers with the vertices, indices, and normals to prepare for rendering
    initBuffers(&el, &p, &n, nullptr, nullptr);
}