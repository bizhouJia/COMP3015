#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/objmesh.h"
#include "helper/plane.h"
#include "helper/frustum.h"
#include "helper/sphere.h";
#include "helper/cup.h";

#include <glad/glad.h>
#include "helper/glslprogram.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, solidProg, waterProg;

    GLuint shadowFBO, passOneIndex, passTwoIndex;
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    Frustum lightFrustum;
    int shadowMapWidth, shadowMapHeight;
    float tPrev, angle, time;

    glm::mat4 lightPV, shadowScale;
    glm::vec3 lightPos;

    Sphere sphere;
    Cup cup;
    Frustum frustum;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene(int);
    void update( float t );
    void render(int);
    void resize(int, int);

    void intializeScene();
    void setShadowMatrices();
    void setFBO();
    void drawBuildingScene();
    void GenerateShadowMap();

    void drawWater();
    void setWaterMatrices();
    void renderWater();

    void renderModel();
};

#endif
