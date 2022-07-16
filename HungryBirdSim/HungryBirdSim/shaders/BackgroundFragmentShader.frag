#version 450

layout(binding = 1) uniform GlobalUniformBufferObjectLight {
	vec3 eyePos;
    vec3 directionalDir;
	vec3 directionalColor;
	vec3 pointColor;
	vec3 pointPos;
	vec3 spotDir;
	vec3 spotColor;
	vec3 spotPos;
	vec3 ambientColor;
	vec3 ambientReflection;
	vec3 hemisphericalTopColor;
	vec3 hemisphericalBottomColor;
	vec3 hemisphericalTopDir;
	vec3 hemisphericalReflection;
	vec3 sphericalColor;
	vec3 sphericalColorDeviationX;
	vec3 sphericalColorDeviationY;
	vec3 sphericalColorDeviationZ;
	vec3 sphericalReflection;
    vec3 selectorDiffuse;
    vec3 selectorSpecular;
	float pointDecay;
	float pointDistanceReduction;
	float spotDecay;
	float spotDistanceReduction;
	float spotCosineOuterAngle;
	float spotCosineInnerAngle;
	int selectorDirectional;
	int selectorAmbient;
	int selectorHemispherical;
	int selectorSpherical;
	int selectorPoint;
	int selectorSpot;
} guboLight;

layout(binding = 2) uniform sampler2D diffuseSampler;

layout(binding = 4) uniform UniformBufferObjectLight {
	int selectorBackground;
	int selectorObject;
} uboLight;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 fragNorm;
layout(location = 4) in vec3 fragViewDir;

layout(location = 0) out vec4 outColor;

void main() { 
	const vec3  diffColor = texture(diffuseSampler, fragTexCoord).rgb;

	vec3 backgroundColor = uboLight.selectorBackground * diffColor;

	outColor = vec4(backgroundColor, 1.0f);
}