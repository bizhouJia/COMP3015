#include "scenebasic_uniform.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include "helper/glutils.h"
#include <glm/gtc/matrix_transform.hpp>
#include "helper/noisetex.h"

using std::string;
using std::cerr;
using std::endl;

// Using glm::vec3 for 3D vectors
using glm::vec3;
// Using glm::mat3 for 3x3 matrices
using glm::mat3;
// Using glm::mat4 for 4x4 matrices
using glm::mat4;
// Using glm::vec4 for 4D vectors
using glm::vec4;

// Constructor for SceneBasic_Uniform class
SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), shadowMapWidth(512), shadowMapHeight(512), plane(13.0f, 10.0f, 200, 2), sphere(2.0f, 50, 50), cup(0.3f, 0.1f, 1, 10)
{
    // Load the mesh for the building model
    mesh = ObjMesh::load("media/models/building.obj");
}

// Initialize the scene
void SceneBasic_Uniform::initScene(int index)
{
    // Compile shaders
      compile();
      intializeScene();
}

void SceneBasic_Uniform::compile()
{
	try
    {
        // Compile and link the main shader program
		prog.compileShader("shader/pcf.vert");
		prog.compileShader("shader/pcf.frag");
		prog.link();
        prog.use();

        // Compile and link the solid shader program
        solidProg.compileShader("shader/frustum.vert");
        solidProg.compileShader("shader/frustrum.frag");
        solidProg.link();

        // Compile and link the water shader program
        waterProg.compileShader("shader/water.vert");
        waterProg.compileShader("shader/water.frag");
        waterProg.link();

	} catch (GLSLProgramException &e)
    {
        // Output error and exit if shader linking fails
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

// Update the scene based on elapsed time
void SceneBasic_Uniform::update( float t )
{
    time = t;

    // Calculate the delta time
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;

    // Update the angle based on delta time
    angle += 0.2f * deltaT;
    if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

// Render the model
void SceneBasic_Uniform::renderModel() 
{
    // Set material properties for the shader
    prog.setUniform("Material.Ka", 0.7f, 1.0f, 1.0f);
    prog.setUniform("Material.Ks", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 1.0f);
    // Set the model matrix for the cup
    model = glm::translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.1f));
    // Set the shadow matrices
    setShadowMatrices();
    // Render the cup
    cup.render();
}

// Generate the shadow map
void SceneBasic_Uniform::GenerateShadowMap() 
{
    // Set view and projection matrices for the light frustum
    view = lightFrustum.getViewMatrix();
    projection = lightFrustum.getProjectionMatrix();
    // Bind the shadow framebuffer object (FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    // Clear the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    // Set the viewport size for the shadow map
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    // Set the subroutine for recording depth
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passOneIndex);
    // Enable face culling
    glEnable(GL_CULL_FACE);
    // Cull front faces
    glCullFace(GL_FRONT);
    // Enable polygon offset to avoid shadow artifacts
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    // Draw the building scene for shadow map
    drawBuildingScene();
    // Restore the default culling mode and disable polygon offset
    glCullFace(GL_BACK);
    glDisable(GL_POLYGON_OFFSET_FILL);
    // Ensure all commands are executed
    glFlush();
}

// Render the entire scene
void SceneBasic_Uniform::render(int index)
{
    // Use the main shader program
    prog.use();
    GenerateShadowMap();
    // Set the camera position based on angle
    vec3 cameraPos(1.8f * cos(angle), 1.1f, 1.2f * sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f, -0.175f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    // Set the light position in the shader
    prog.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0));
    // Set the projection matrix for the camera
    projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);
    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the viewport size
    glViewport(0, 0, width, height);
    // Set the subroutine for shading with shadow
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passTwoIndex);
    drawBuildingScene();
    // Ensure all commands are executed
    glFinish();

    renderModel();

    // Use the solid shader program for the frustum
    solidProg.use();
    solidProg.setUniform("frustumColour", vec4(1.0f, 0.0f, 0.0f, 1.0f));
    // Set the model-view-projection matrix for the frustum
    mat4 mv = view * lightFrustum.getInverseViewMatrix();
    solidProg.setUniform("frustumMVP", projection * mv);
    // Render the frustum
    lightFrustum.render();

    // Use the water shader program
    waterProg.use();
    drawWater();
    renderWater();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    // Set the viewport size
    glViewport(0,0,w,h);
}

// Initialize the scene elements
void SceneBasic_Uniform::intializeScene() 
{
    // Set the clear color
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    // Initialize the angle
    angle = glm::two_pi<float>() * 0.85f;
    // Set up the framebuffer object
    setFBO();

    GLuint programHandle = prog.getHandle();
    // Get the subroutine indices for depth recording and shading with shadow
    passOneIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth"); 
    passTwoIndex = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");
    // Set the shadow scale matrix
    shadowScale = 
        mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.5f, 0.5f, 0.5f, 1.0f)
    );
    // Set the light position
    lightPos = vec3(-2.5f, 2.0f, -2.5f); 
    // Set up the light frustum
    lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

    lightFrustum.setPerspective(40.0f, 1.0f, 0.1f, 100.0f);

    lightPV = shadowScale * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();
    // Set uniform values for light intensity and shadow map
    prog.setUniform("Light.Intensity", vec3(0.85f));
    prog.setUniform("ShadowMap", 0);
}

// Set up the framebuffer object (FBO) for the shadow map
void SceneBasic_Uniform::setFBO() 
{
    GLfloat border[] = { 1.0f,0.0f,0.0f,0.0f };

    GLuint depthTex;
    // Generate and configure the depth texture
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    // Activate texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    // Generate and bind the shadow framebuffer
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthTex, 0);
    // Set draw buffers
    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);
    // Check framebuffer status
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    }
    else {
        printf("Framebuffer is not complete.\n");
    }
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Draw the building scene
void SceneBasic_Uniform::drawBuildingScene()
{
    // Set the color for the building
    vec3 color = vec3(1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ka", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Kd", color);
    prog.setUniform("Material.Ks", vec3(0.0f));
    prog.setUniform("Material.Shininess", 1.0f);
    // Reset the model matrix
    model = mat4(1.0f);
    setShadowMatrices();
    // Render the building mesh
    mesh->render();
}

// Set the shadow matrices
void SceneBasic_Uniform::setShadowMatrices() 
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
        glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("ShadowMatrix", lightPV * model);
}

// Set the water matrices
void SceneBasic_Uniform::setWaterMatrices() 
{
    mat4 mv = view * model;
    waterProg.setUniform("waterModelViewMatrix", mv);
    waterProg.setUniform("waterNormalMatrix",
        glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    waterProg.setUniform("waterMVP", projection * mv);
}

// Draw water elements
void SceneBasic_Uniform::drawWater() 
{
    waterProg.use();
    // Set light intensity
    waterProg.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
}

// Render the water
void SceneBasic_Uniform::renderWater()
{
    waterProg.use();
    // Set the time uniform
    waterProg.setUniform("waterTime", time);
    // Set material properties for water
    waterProg.setUniform("Material.Kd", 0.0f, 0.0f, 1.0f);
    waterProg.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    waterProg.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    waterProg.setUniform("Material.Shininess", 100.0f);
    // Set the model matrix for the water plane
    model = mat4(1.0f);
    model = glm::translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-5.0f), vec3(0.0f, 0.0f, 5.3f));
    setWaterMatrices();
    plane.render();
}



