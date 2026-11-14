#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__
#define LGIHT_TYPE_DIRECTIONAL 0
#define LGIHT_TYPE_POINT 1
#define LGIHT_TYPE_SPOT 2
#define MAX_LIGHTS 6

struct VertexShaderInput
{
    float3 localPosition    : POSITION; 
    float2 uv               : TEXCOORD0; 
    float3 normal           : NORMAL0; 
    float3 tangent          : TANGENT0;
};

struct VertexToPixel
{
    float4 screenPosition   : SV_POSITION;
    float2 uv               : TEXCOORD0;
    float3 normal           : NORMAL0;
    float3 tangent          : TANGENT0;
    float3 worldPosition    : TEXCOORD1;
};

struct Light
{
    int     Type;
    float3  Direction;
    float   Range;
    float3  Position;
    float   Intensity;
    float3  Color;
    float   SpotInnerAngle;
    float   SpotOuterAngle;
    float2  Padding;
};

struct VertexToPixel_Sky
{
    float4 position         : SV_POSITION;
    float3 sampleDir        : DIRECTION;
};

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0 - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float Diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

float SpecularPhong(float3 normal, float3 dirFromLight, float3 dirToCamera, float shininess)
{
    float3 reflected = reflect(dirFromLight, normal);
    return pow(saturate(dot(reflected, dirToCamera)), shininess);
}

float3 DirectionalLight(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceColor)
{
    float3 dirFromLight = normalize(light.Direction);
    float3 dirToCamera = normalize(camPos - worldPos);
    float diff = Diffuse(normal, -dirFromLight);
    float spec = SpecularPhong(normal, dirFromLight, dirToCamera, 128.0f);
    return (diff * surfaceColor + spec) * light.Intensity * light.Color;
}

float3 PointLight(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceColor)
{
    float3 dirFromLight = normalize(worldPos - light.Position);
    float3 dirToCamera = normalize(camPos - worldPos);
    float diff = Diffuse(normal, -dirFromLight);
    float spec = SpecularPhong(normal, dirFromLight, dirToCamera, 128.0f);
    return (diff * surfaceColor + spec) * Attenuate(light, worldPos) * light.Intensity * light.Color;
}

float3 SpotLight(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceColor)
{
    float3 dirFromLight = normalize(worldPos - light.Position);
    float pixelCos = saturate(dot(dirFromLight, normalize(light.Direction)));
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloff = saturate((pixelCos - cosOuter) / (cosInner - cosOuter));
    return PointLight(light, normal, worldPos, camPos, surfaceColor) * falloff;
}

#endif