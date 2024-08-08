#version 460

// Input layout locations for vertex attributes
layout (location = 0 ) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

// Output variables for passing data to the fragment shader
out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;

// Uniform variables for water properties and transformation matrices
// Current time for animation
uniform float waterTime;
// Frequency of the water wave
uniform float waterFreq = 2.5;
// Velocity of the water wave
uniform float waterVelocity = 2.5;
// Amplitude of the water wave
uniform float waterAmp = 0.6;

uniform mat4 waterModelViewMatrix;
uniform mat3 waterNormalMatrix;
uniform mat4 waterMVP;

// Function to calculate the y-coordinate of the water vertex based on a sine wave
void CalculateVertices(vec4 pos, float u)
{
    // Adjust the y-coordinate based on the sine wave
    pos.y = waterAmp * sin( u );
}

// Function to calculate the transformed position, normal, and texture coordinates
void CalculateMatrices(vec4 pos, vec3 n)
{
    // Transform the position using the model-view matrix
    Position = waterModelViewMatrix * pos;
    // Transform the normal using the normal matrix
    Normal = waterNormalMatrix * n;
    // Pass through the texture coordinates
    TexCoord = VertexTexCoord;
}

// Main function to execute the vertex shader
void main()
{
    // Start with the vertex position in homogeneous coordinates
    vec4 pos = vec4(VertexPosition,1.0);
    // Calculate the sine wave parameter u based on frequency, position, velocity, and time
    float u = waterFreq * pos.x - waterVelocity * waterTime;
    // Adjust the vertex y-coordinate based on the wave
    CalculateVertices(pos, u);
    // Calculate the normal vector n for the water surface
    vec3 n = vec3(0.0);
    // Adjust the normal vector based on the wave
    n.xy = normalize(vec2(cos( u ), 1.0));

    // Calculate the transformed position, normal, and texture coordinates
    CalculateMatrices(pos, n);
    // Calculate the final position of the vertex in clip space using the MVP matrix
    gl_Position = waterMVP * pos;
}