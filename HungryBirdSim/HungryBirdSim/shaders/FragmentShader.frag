#version 450

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
	vec3 sphericalColor;
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

layout(binding = 2) uniform sampler2D diffuseSampler;

layout(binding = 4) uniform UniformBufferObjectLight {
	int selectorDirectional;
	int selectorPoint;
	int selectorSpot;
} uboLight;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 fragNorm;
layout(location = 4) in vec3 fragViewDir;

layout(location = 0) out vec4 outColor;

/****************************************
 *										*
 *										*
 *				DIFFUSE COLOR			*
 *										*
 *										*
 ****************************************/
vec3 Lambert_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C) {
	// Lambert Diffuse BRDF model
	// in all BRDF parameters are:
	// vec3 L : light direction
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 C : main color (diffuse color, or specular color)
	float alphaCosine = dot(L, N);
	
	return C * max(alphaCosine, 0);
}

vec3 Oren_Nayar_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float sigma) {
	// Oren Nayar directional light
	// vec3 L : light direction
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 C : main color (diffuse color, or specular color)
	// float sigma : roughness of the material
	float thetai = acos(dot(L, N));
	float thetar = acos(dot(V, N));
	float alpha = max(thetai, thetar);
	float beta = min(thetai, thetar);
	float A = 1 - 0.5 * ((pow(sigma, 2)) / (pow(sigma, 2) + 0.33));
	float B = 0.45 * ((pow(sigma, 2)) / (pow(sigma, 2) + 0.09));
	vec3 vi = normalize(L - dot(L, N) * N);
	vec3 vr = normalize(V - dot(V, N) * N);
	float G = max(0, dot(vi, vr));
	vec3 LON = C * clamp(dot(L, N), 0, 1);

	return LON * (A + B * G * sin(alpha) * tan(beta));
}

vec3 Toon_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, vec3 Cd, float thr) {
	// Toon Diffuse Brdf
	// vec3 L : light direction
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 C : main color (diffuse color, or specular color)
	// vec3 Cd : color to be used in dark areas
	// float thr : color threshold
	vec3 color;
	float alphaCosine = dot(L, N);
	if (alphaCosine < 0) {
		color = vec3(0,0,0);
	} else if (alphaCosine >= 0 && alphaCosine < thr) {
		color = Cd;
	} else {
		color = C;
	}
	
	return color;
}

/****************************************
 *										*
 *										*
 *				SPECULAR COLOR			*
 *										*
 *										*
 ****************************************/
vec3 Phong_Specular_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float gamma)  {
	// Phong Specular BRDF model
	// vec3 L : light direction
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 C : main color (diffuse color, or specular color)
	// float gamma : exponent of the cosine term
	vec3 nFirst = N * dot(L, N);
	vec3 dFirst = nFirst - L;
	vec3 r = L + 2 * dFirst;
	
	return C * pow(clamp(dot(V, r), 0, 1), gamma);
}

vec3 Blinn_Specular_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float gamma) {
	// Blinn directional light
	// vec3 L : light direction
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 C : main color (diffuse color, or specular color)
	vec3 h = normalize(L + V);

	return C * pow(clamp(dot(N, h), 0, 1), gamma);
}

vec3 Toon_Specular_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float thr)  {
	// Directional light direction
	// vec3 L : light direction
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 C : main color (diffuse color, or specular color)
	// float thr : color threshold
	vec3 nFirst = N * dot(L, N);
	vec3 dFirst = nFirst - L;
	vec3 r = L + 2 * dFirst;
	vec3 color;

	if (dot(V, r) < thr) {
		color = vec3(0, 0, 0);
	} else {
		color = C;
	}
	
	return color * clamp(dot(V, r), 0 , 1);
}

/****************************************
 *										*
 *										*
 *				LIGHTING				*
 *										*
 *										*
 ****************************************/
 vec3 direct_light_dir(vec3 pos) {
	// Directional light direction
	return guboLight.directionalDir;
}

vec3 direct_light_color(vec3 pos) {
	// Directional light color
	return guboLight.directionalColor;
}

vec3 point_light_dir(vec3 pos) {
	// Point light direction
	return normalize(guboLight.pointPos - pos);
}

vec3 point_light_color(vec3 pos) {
	// Point light color
	float magnitude = length(guboLight.pointPos - pos);
	float decay = pow(guboLight.pointDistanceReduction / magnitude, guboLight.pointDecay);
	return guboLight.pointColor * decay;
}

vec3 spot_light_dir(vec3 pos) {
	// Spot light direction
	return normalize(guboLight.spotPos - pos);
}

vec3 spot_light_color(vec3 pos) {
	// Spot light color
	float magnitude = length(guboLight.spotPos - pos);
	float decay = pow(guboLight.spotDistanceReduction / magnitude, guboLight.spotDecay);
	float cosine = dot(normalize(guboLight.spotPos - pos), guboLight.spotDir);
	float clampInput = (cosine - guboLight.spotCosineOuterAngle) / (guboLight.spotCosineInnerAngle - guboLight.spotCosineOuterAngle);
	return guboLight.spotColor * decay * clamp(clampInput, 0, 1);
}

/****************************************
 *										*
 *										*
 *			AMBIENT LIGHTING			*
 *										*
 *										*
 ****************************************/
 vec3 Ambient_Light(vec3 C, vec3 mA) {
	// Ambient light model
	// vec3 C: ambient light color
	// vec3 mA: ambient light reflection color
	return C * mA;
}

vec3 Hemispheric_Light(vec3 topColor, vec3 bottomColor, vec3 topDir, vec3 N, vec3 mA) {
	// Hemispheric light model
	// vec3 topColor: the top color for the hemisphere
	// vec3 bottomColor: the bottom color for the hemisphere
	// vec3 topDir: the direction of the top color
	// vec3 N: the normal to the point
	// vec3 mA: ambient light reflection color
	float topCoeff = (dot(N, topDir) + 1) / 2;
	float bottomCoeff = (1 - dot(N, topDir)) / 2;
	vec3 ambient_color = topCoeff * topColor + bottomCoeff * bottomColor;

	return ambient_color * mA;
}

vec3 Spherical_Harmonics_Light(vec3 baseColor, vec3 DxColor, vec3 DyColor, vec3 DzColor, vec3 N, vec3 mA) {
	// Spherical Harmonics diffuse model
	// vec3 baseColor: the base color of the ambient
	// vec3 DxColor: the deviation color in x
	// vec3 DyColor: the deviation color in y
	// vec3 DzColor: the deviation color in z
	// vec3 N: the normal of the point in which we need to compute spherical harmonics
	// vec3 mA: ambient light reflection color
	vec3 sphericalColor = baseColor + N.x * DxColor + N.y * DyColor + N.z * DzColor;
	
	return sphericalColor * mA;
}

void main() { 
	const vec3  diffColor = texture(diffuseSampler, fragTexCoord).rgb;
	const vec3  specColor = vec3(0, 0, 0);
	const float specPower = 200.0f;

	///////////////////////////////////////////////////////////////////////////////////////
	//																					 //
	//																					 //
	// sum all the ambient lights that are present in the scene using selectors (1 or 0) //
	//																					 //
	//																					 //
	///////////////////////////////////////////////////////////////////////////////////////
	vec3 ambientLight = guboLight.selectorAmbient * Ambient_Light(guboLight.ambientColor, diffColor) + 
						guboLight.selectorHemispherical * Hemispheric_Light(guboLight.hemisphericalTopColor, guboLight.hemisphericalBottomColor, guboLight.hemisphericalTopDir, fragNorm, diffColor) +
						guboLight.selectorSpherical * Spherical_Harmonics_Light(guboLight.sphericalColor, guboLight.sphericalColorDeviationX, guboLight.sphericalColorDeviationY, guboLight.sphericalColorDeviationZ, fragNorm, diffColor);

	vec3 diffuseColor = vec3(0,0,0);
	vec3 specularColor = vec3(0,0,0);

	///////////////////////////////////////////////////////////////////////////////////////
	//																					 //
	//																					 //
	// sum all the scene's lights that are present in the scene to compute diffuse color //
	//																					 //
	//																					 //
	///////////////////////////////////////////////////////////////////////////////////////
	if (guboLight.selectorDiffuse == 0) {
		diffuseColor += guboLight.selectorDirectional * direct_light_color(fragPos) * Lambert_Diffuse_BRDF(direct_light_dir(fragPos), fragNorm, fragViewDir, diffColor) +
						guboLight.selectorPoint * point_light_color(fragPos) * Lambert_Diffuse_BRDF(point_light_dir(fragPos), fragNorm, fragViewDir, diffColor) +  
						guboLight.selectorSpot * spot_light_color(fragPos) * Lambert_Diffuse_BRDF(spot_light_dir(fragPos), fragNorm, fragViewDir, diffColor);
	} else if (guboLight.selectorDiffuse == 1) {
		diffuseColor += guboLight.selectorDirectional * direct_light_color(fragPos) * Toon_Diffuse_BRDF(direct_light_dir(fragPos), fragNorm, fragViewDir, diffColor, 0.2f * diffColor, 0.5f) +
						guboLight.selectorPoint * point_light_color(fragPos) * Toon_Diffuse_BRDF(point_light_dir(fragPos), fragNorm, fragViewDir, diffColor, 0.2f * diffColor, 0.5f) +  
						guboLight.selectorSpot * spot_light_color(fragPos) * Toon_Diffuse_BRDF(spot_light_dir(fragPos), fragNorm, fragViewDir, diffColor, 0.2f * diffColor, 0.5f);
	} else {
		diffuseColor += guboLight.selectorDirectional * direct_light_color(fragPos) * Oren_Nayar_Diffuse_BRDF(direct_light_dir(fragPos), fragNorm, fragViewDir, diffColor, 1.5f) +
						guboLight.selectorPoint * point_light_color(fragPos) * Oren_Nayar_Diffuse_BRDF(point_light_dir(fragPos), fragNorm, fragViewDir, diffColor, 1.5f) +  
						guboLight.selectorSpot * spot_light_color(fragPos) * Oren_Nayar_Diffuse_BRDF(spot_light_dir(fragPos), fragNorm, fragViewDir, diffColor, 1.5f);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//																					  //
	//																					  //
	// sum all the scene's lights that are present in the scene to compute specular color //
	//																					  //
	//																					  //
	////////////////////////////////////////////////////////////////////////////////////////
	if (guboLight.selectorSpecular == 0) {
		specularColor += guboLight.selectorDirectional * direct_light_color(fragPos) * Phong_Specular_BRDF(direct_light_dir(fragPos), fragNorm, fragViewDir, specColor, specPower) +
						guboLight.selectorPoint * point_light_color(fragPos) * Phong_Specular_BRDF(point_light_dir(fragPos), fragNorm, fragViewDir, specColor, specPower) +  
						guboLight.selectorSpot * spot_light_color(fragPos) * Phong_Specular_BRDF(spot_light_dir(fragPos), fragNorm, fragViewDir, specColor, specPower);
	} else if (guboLight.selectorSpecular == 1) {
		specularColor += guboLight.selectorDirectional * direct_light_color(fragPos) * Toon_Specular_BRDF(direct_light_dir(fragPos), fragNorm, fragViewDir, specColor, 0.97f) +
						guboLight.selectorPoint * point_light_color(fragPos) * Toon_Specular_BRDF(point_light_dir(fragPos), fragNorm, fragViewDir, specColor, 0.97f) +  
						guboLight.selectorSpot * spot_light_color(fragPos) * Toon_Specular_BRDF(spot_light_dir(fragPos), fragNorm, fragViewDir, specColor, 0.97f);
	} else {
		specularColor += guboLight.selectorDirectional * direct_light_color(fragPos) * Blinn_Specular_BRDF(direct_light_dir(fragPos), fragNorm, fragViewDir, specColor, specPower) +
						guboLight.selectorPoint * point_light_color(fragPos) * Blinn_Specular_BRDF(point_light_dir(fragPos), fragNorm, fragViewDir, specColor, specPower) +  
						guboLight.selectorSpot * spot_light_color(fragPos) * Blinn_Specular_BRDF(spot_light_dir(fragPos), fragNorm, fragViewDir, specColor, specPower);
	}

	outColor = vec4(diffuseColor + specularColor + ambientLight, 1.0f);
}