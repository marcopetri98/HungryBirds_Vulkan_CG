#version 450

layout(binding = 0) uniform GlobalUniformBufferObject
{
	mat4 view;
	mat4 proj;
} gubo;

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

void main() {
    // OK
	gl_Position = gubo.proj * gubo.view * ubo.modelVertices * vec4(inPosition, 1.0);
    // Non so a cosa serve
	fragColor = inColor;
	// OK
    fragTexCoord = inTexCoord;
	// OK, Normal Vec3 Missing
	fragNorm = (ubo.modelVertices * vec4(inNormal,0.0) ).xyz;
	fragPos  = (gubo.view[3]).xyz - (ubo.modelVertices * vec4(inPosition,  1.0)).xyz;
	
	
}