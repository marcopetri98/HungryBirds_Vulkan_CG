#version 450

layout(binding = 0) uniform GlobalUniformBufferObject
{
	mat4 view;
	mat4 proj;
} gubo;

layout(binding = 1) uniform GlobalUniformBufferObjectLight {
	vec3 eyePos;
    vec3 directionalDir;
	vec3 directionalColor;
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
	vec3 ambientColor;
	vec3 ambientReflection;
	vec3 hemisphericalTopColor;
	vec3 hemisphericalBottomColor;
	vec3 hemisphericalTopDir;
	vec3 hemisphericalReflection;
	vec3 sphericalColorDeviationX;
	vec3 sphericalColorDeviationY;
	vec3 sphericalColorDeviationZ;
	vec3 sphericalReflection;
    int selectorDiffuse;
    int selectorSpecular;
	int selectorDirectional;
	int selectorAmbient;
	int selectorHemispherical;
	int selectorSpherical;
	int selectorPoint;
	int selectorSpot;
} guboLight;

layout(binding = 3) uniform UniformBufferObject
{
	mat4 modelVertices;
	mat4 modelNormal;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec3 fragNorm;
layout(location = 4) out vec3 fragViewDir;

void main() {
	gl_Position = gubo.proj * gubo.view * ubo.modelVertices * vec4(inPosition, 1.0f);
	fragColor = inColor;
	fragViewDir = normalize(guboLight.eyePos - (ubo.modelVertices * vec4(inPosition, 1.0f)).xyz);
	fragNorm = normalize(mat3(ubo.modelNormal) * inNormal);
	fragTexCoord = inTexCoord;
	fragPos = (ubo.modelVertices * vec4(inPosition, 1.0f)).xyz;
}