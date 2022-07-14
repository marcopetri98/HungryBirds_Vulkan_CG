#version 450

layout(binding = 1) uniform GlobalUniformBufferObjectLight {
	vec3 eyePos;
	//~~~~~~~~~~Directional Lighting~~~~~~~~~~~~~~~~~~~~//
    vec3 directionalDir;
	vec3 directionalColor;
	//~~~~~~~~~~~Point Lighting~~~~~~~~~~~~~~~~~~~~~~~//
	vec3 pointDir;
	vec3 pointColor;
	vec3 pointPos;
	float pointDecay;
	float pointDistanceReduction;
	//~~~~~~~~~~~Spotlighting~~~~~~~~~~~~~~~~~~~~//
	vec3 spotDir;
	vec3 spotColor;
	vec3 spotPos;
	float spotDecay;
	float spotDistanceReduction;
	float spotCosineOuterAngle;
	float spotCosineInnerAngle;
	//~~~~~~~~~~~0 Lambert~~~~~~~~~~~~~~~~~~~//
	//~~~~~~~~~~~1 Toon~~~~~~~~~~~~~~~~~~~~~//
	//~~~~~~~~~~~2 Oren_Nayar~~~~~~~~~~~~~~//
    int selectorDiffuse;
	//~~~~~~~~~~~0 Phong_Specular~~~~~~~~//
	//~~~~~~~~~~~1 Toon_Specular~~~~~~~~//
    int selectorSpecular;
} guboLight;

layout(binding = 2) uniform sampler2D texSampler;

layout(binding = 4) uniform UniformBufferObjectLight {
	int selectorDirectional;
	int selectorPoint;
	int selectorSpot;
} uboLight;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 fragNorm;

layout(location = 0) out vec4 outColor;

/////           BRDF                       ////
vec3 Lambert_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C) {
	return C * max(dot(L,N),0);
}

vec3 Oren_Nayar_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float sigma) {
	
	float theta_i = acos(dot(V,N)); 
	float theta_r = acos(dot(L,N));

	float alpha = max(theta_i,theta_r);
	float beta = min(theta_i,theta_r);

	float sigmaSQ = sigma*sigma;
	float A = 1 - 0.5 * (sigmaSQ)/(sigmaSQ + 0.33);
	float B = 0.45*sigmaSQ/(sigmaSQ + 0.09);
	
	// (p-x) / length(p-x) normalize (p-x)
	vec3 vi =  normalize(L-dot(L,N)*N);
	//vi = vi / length(vi);
	vec3 vr = normalize(V-dot(V,N)*N);
	//vr = vr / length(vr);

	float G = max(0,dot(vi,vr));

	float clamped = (dot(L,N));
	if (clamped < 0){
		clamped = 0;
	}
	if (clamped > 1){
		clamped = 1;
	}
	vec3 L_new = C * clamped;
	return L_new * (A + B*G*sin(alpha)*tan(beta));
}

vec3 Phong_Specular_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float gamma)  {
	return C*max(0, pow(dot(V,-reflect(L,N)),gamma) );
}

vec3 Toon_Diffuse_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, vec3 Cd, float thr) {
	vec3 res = Cd;
	if(dot(L,N)>= thr){
		res = C;
	}
	return Lambert_Diffuse_BRDF(L,N,V,res);
}

vec3 Toon_Specular_BRDF(vec3 L, vec3 N, vec3 V, vec3 C, float thr)  {
	vec3 res = C;
	if(dot(V,-reflect(L,N)) < thr){
		res = vec3(0,0,0);
	}
	return Phong_Specular_BRDF(L,N,V,res,8.0f);
}

////                 Light Model                  ////

vec3 direct_light_dir(vec3 pos) {
	return guboLight.directionalDir;
}

vec3 direct_light_color(vec3 pos) {
	return guboLight.directionalColor;
}
vec3 point_light_dir(vec3 pos) {
	float decay = guboLight.pointDistanceReduction/length(pos-guboLight.pointPos);
	decay = pow(decay,guboLight.pointDecay);
	return -normalize(pos-guboLight.pointPos)*decay;
}

vec3 point_light_color(vec3 pos) {
	return guboLight.pointColor;
}



vec3 spot_light_dir(vec3 pos) {
	float cosA = dot(-normalize(pos-guboLight.spotPos),guboLight.spotDir);
	float toClamp = ( cosA - guboLight.spotCosineOuterAngle) / (guboLight.spotCosineInnerAngle-guboLight.spotCosineOuterAngle);
	if (toClamp > 1){
		toClamp = 1;
	}
	if (toClamp < 0){
		toClamp = 0;
	}
	return guboLight.spotDir * toClamp;
}

vec3 spot_light_color(vec3 pos) {
	return guboLight.spotColor;
}
void main() { 
	vec3 Norm = normalize(fragNorm);
	
	vec3 EyeDir = normalize(guboLight.eyePos - fragPos);
	
	// light direction from the light model
	vec3 lD = vec3(0,0,0);
	lD += direct_light_dir(fragPos);
	lD += point_light_dir(fragPos);
	lD += spot_light_dir(fragPos);
	
	// light color and intensity from the light model
	// Set this to vec3(0,0,0); when light is passed to the model
	vec3 lC = vec3(1,1,1);
	lC += direct_light_color(fragPos);
	lC += point_light_color(fragPos);
	lC += spot_light_color(fragPos);
	
	
	vec3 DiffColor = texture(texSampler, fragTexCoord).rgb;
	vec3 Diffuse = vec3(0,0,0);	
	
	if (guboLight.selectorDiffuse == 0){
		Diffuse += Lambert_Diffuse_BRDF(lD, Norm, EyeDir, DiffColor ) ;
	} else if (guboLight.selectorDiffuse == 1){ 
		vec3 darkColor = 0.2f * DiffColor;
		Diffuse += Toon_Diffuse_BRDF(lD, Norm, EyeDir, DiffColor, darkColor, 0.5); 
	} else if (guboLight.selectorDiffuse == 2){ 
		Diffuse += Oren_Nayar_Diffuse_BRDF(lD, Norm, EyeDir, DiffColor, 0.8);
	}

	vec3 Specular = vec3(0,0,0);
	if (guboLight.selectorSpecular == 0){
		Specular += Phong_Specular_BRDF(lD, Norm, EyeDir, DiffColor, 12);
	} else if (guboLight.selectorSpecular == 1){ // toon
		Specular += Toon_Specular_BRDF(lD, Norm, EyeDir, DiffColor, 0.4);
	}
	
	float AmbFact = 0.025;
	vec3 Ambient = AmbFact * DiffColor;
	
	outColor = vec4( (Diffuse + Specular + Ambient) * lC, 1.0f);	
}