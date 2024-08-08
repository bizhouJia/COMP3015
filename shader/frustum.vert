#version 460

layout (location = 0 ) in vec3 VertexPosition;

uniform mat4 frustumMVP;

void main()
{
    gl_Position = frustumMVP * vec4(VertexPosition, 1.0);
}