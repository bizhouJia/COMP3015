# CW

## Environment

Windows 11 Home Edition

Visual Studio Community 2022 - 17.5.1

glew - 2.1.0

glfw - 3.4

glm - 1.0.1

## Link

https://youtu.be/1V1IC8WciJc

## Codes

**cup.h**: This file declares the `cup` class, including its attributes (data members) and methods (member functions). The class includes functions to initialize the cup.

**cup.cpp**: This implementation file defines the functions declared in `cup.h`. It contains the actual code for initializing the cup, modifying its properties, and performing the specified operations.

**frustum.vert**: This vertex shader file is used to process each vertex of the 3D objects. It transforms the vertices from object space to screen space, applying view and projection transformations to define the frustum (the visible area of the scene). And handle lighting calculations and send transformed vertices to the fragment shader.

**frustrum.frag**: This fragment shader file is used to process each fragment (pixel) of the 3D objects. It determines the final color of the pixels within the frustum, handling operations like texturing, lighting, and shading to ensure proper rendering within the defined view.

**pcf.vert**: This vertex shader is used to prepare data for shadow mapping using the Percentage Closer Filtering (PCF) technique. It transforms vertices and calculate depth values to be used in the shadow map.

**pcf.frag**: This fragment shader implements the PCF algorithm to smoothen the edges of shadows. It samples multiple points within a shadow map to determine the visibility of a fragment, producing softer and more realistic shadows.

**water.vert**: This vertex shader is responsible for transforming the vertices of water surfaces, possibly including calculations for wave effects using vertex displacement. It also compute normal vertex for lighting calculations.

**water.frag**: This fragment shader handles the visual appearance of water surfaces, including reflections, refractions, and other optical effects. It combines these effects to produce realistic water rendering by simulating light interaction with the water surface.

**scenebasic_uniform.h**: This file declares a basic scene class, containing attributes and methods related to the scene setup. It defines uniform variables used for rendering, such as transformation matrices, light positions, and material properties.

**scenebasic_uniform.cpp**: This source file implements the methods declared in the header file. It sets up the scene, initializes uniform variables, and manages the rendering process, ensuring that the scene is correctly displayed with the specified parameters.
