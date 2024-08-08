#include "noisetex.h"

#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

int NoiseTex::storeTex( GLubyte * data, int w, int h ) {
  GLuint texID;
  glGenTextures(1, &texID);

  // Bind the texture to the 2D texture target
  glBindTexture(GL_TEXTURE_2D, texID);
  // Allocate storage for the texture
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
  // Specify the texture data
  glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,data);
  // Set the magnification filter to linear
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Set the minification filter to linear
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // Set the wrap mode for the S (U) coordinate to repeat
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // Set the wrap mode for the T (V) coordinate to repeat
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  return texID;
}

// Function to generate a 2D noise texture
int NoiseTex::generate2DTex(float baseFreq, float persistence, int w, int h, bool periodic) {

  int width = w;
  int height = h;

  printf("Generating noise texture...");

  // Allocate memory for the texture data
  GLubyte *data = new GLubyte[ width * height * 4 ];

  float xFactor = 1.0f / (width - 1);
  float yFactor = 1.0f / (height - 1);

  // Loop through each row of the texture
  for( int row = 0; row < height; row++ ) {
     // Loop through each column of the texture
    for( int col = 0 ; col < width; col++ ) {
      float x = xFactor * col;
      float y = yFactor * row;
      float sum = 0.0f;
      float freq = baseFreq;
      float persist = persistence;
      // Loop through 4 octaves of noise
      for( int oct = 0; oct < 4; oct++ ) {
        // 2D vector for the noise function
        glm::vec2 p(x * freq, y * freq);

        float val = 0.0f;
        if (periodic) {
          val = glm::perlin(p, glm::vec2(freq)) * persist;
        } else {
          val = glm::perlin(p) * persist;
        }

        sum += val;

        float result = (sum + 1.0f) / 2.0f;

        result = result > 1.0f ? 1.0f : result;
        result = result < 0.0f ? 0.0f : result;

        data[((row * width + col) * 4) + oct] = (GLubyte) ( result * 255.0f );
        freq *= 2.0f;
        // Adjust the persistence for the next octave
        persist *= persistence;
      }
    }
  }

  // Store the texture data in OpenGL
  int texID = NoiseTex::storeTex(data, width, height);
  // Free the allocated memory
  delete [] data;

  printf("done.\n");
  return texID;
}

// Function to generate a periodic 2D noise texture
int NoiseTex::generatePeriodic2DTex(float baseFreq, float persist, int w, int h) {
    // Call generate2DTex with periodic set to true
    return NoiseTex::generate2DTex(baseFreq, persist, w, h, true);
}
