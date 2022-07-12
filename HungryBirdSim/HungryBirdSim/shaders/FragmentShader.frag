#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform UniformBufferObjectLight {
    vec3 directionalDir;
	vec3 directionalColor;
	vec3 pointDir;
	vec3 pointColor;
	vec3 pointPos;
	float pointDecay;
	float pointDistanceReduction;
	vec3 spotDir;
	vec3 spotColor;
	vec3 spotPos;
	float spotDecay;
	float spotDistanceReduction;
	float spotCosineOuterAngle;
	float spotCosineInnerAngle;
    int selectorDiffuse;
    int selectorSpecular;
} uboLight;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
}