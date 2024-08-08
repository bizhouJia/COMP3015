#version 460

// Struct for light properties
uniform struct LightInfo
{
    // Light position in homogeneous coordinates
    vec4 Position;
    // Light intensity
    vec3 Intensity;
} Light;

// Struct for material properties
uniform struct MaterialInfo
{
    // Ambient reflectivity
    vec3 Ka;
    // Diffuse reflectivity
    vec3 Kd;
    // Specular reflectivity
    vec3 Ks;
    // Specular shininess factor
    float Shininess;
} Material;

// Sampler for shadow map
uniform sampler2DShadow ShadowMap;

// Inputs from the vertex shader
in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

// Output color of the fragment
layout (location = 0) out vec4 FragColor;

// Function to calculate the light vector
vec3 CalculateLightVector()
{
  return vec3(Light.Position) - Position;
}

// Function to calculate the dot product between the light vector and the normal
float CalculateDotProduct(vec3 s, vec3 n)
{
   return max( dot(s,n), 0.0 );
}

// Function to calculate the diffuse component of the Phong model
vec3 CalculateDiffuse(float sDotN)
{
   return Light.Intensity * Material.Kd * sDotN;
}

// Function to calculate the combined diffuse and specular components using the Phong model
vec3 phongModelDiffAndSpec()
{
    vec3 n = Normal; //get the Normal

    vec3 s = normalize(CalculateLightVector());
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );

    // Calculate the dot product of the light vector and the normal
    float sDotN = CalculateDotProduct(s,n);
    // Calculate the diffuse component
    vec3 diffuse = CalculateDiffuse(sDotN);
    // Initialize the specular component
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        // Calculate the specular component
        spec = Light.Intensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );
    // Return the combined diffuse and specular components
    return diffuse + spec;
}

// Subroutine for different render pass types
subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

// Function to calculate the ambient component
vec3 CalculateAmbient()
{
  return Light.Intensity * Material.Ka;
}

// Function to sum texel values for shadow calculation
void SumTexels(float sum, float shadow)
{
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,-1));
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,1));
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,1));
     sum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,-1));
     shadow = sum * 0.25;
}

// Subroutine for shading with shadow
subroutine (RenderPassType)
void shadeWithShadow()
{
    vec3 ambient = CalculateAmbient();
    vec3 diffAndSpec = phongModelDiffAndSpec();

    float sum = 0;
    float shadow = 1.0;

    if(ShadowCoord.z >= 0 ) {
        // Sum texels for shadow calculation if the coordinate is valid
        SumTexels(sum, shadow);
    }

    // Calculate the final color with shadow effect and apply gamma correction
    FragColor = vec4( ambient + diffAndSpec * shadow, 1.0 ); //this will generate shadows from the shadow map
    FragColor = pow( FragColor, vec4(1.0 / 2.2) );
}

subroutine (RenderPassType)
void recordDepth()
{
    
}

void main() {
    // Call the appropriate subroutine based on the current render pass
    RenderPass();
}