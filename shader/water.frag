#version 460


uniform struct LightInfo {
    vec4 Position;
    vec3 Intensity;
} Light;

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
} Material;

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform float Time;

layout ( location = 0 ) out vec4 FragColor;



vec3 phongModel(vec3 kd) {
    vec3 n = Normal;

    vec3 lightVector = Light.Position.xyz - Position.xyz;

    vec3 s = normalize(lightVector);
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );

    float sDotN = max( dot(s,n), 0.0 );

    vec3 diffuseCalculation = Light.Intensity * kd * sDotN;

    vec3 diffuse = diffuseCalculation;

    vec3 spec = vec3(0.0);

    vec3 specCalculation = Light.Intensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    if( sDotN > 0.0 )
        spec = specCalculation;

    return Material.Ka * Light.Intensity + diffuse + spec;
}

void main()
{
    FragColor = vec4( phongModel(Material.Kd) , 1.0 );
}