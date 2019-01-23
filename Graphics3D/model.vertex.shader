#version 330 core
layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NR_SPOT_LIGHTS 6
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float texture_diffuse1_shininess;
uniform float texture_specular1_shininess;
uniform vec3 viewPos;

//fog
uniform bool enableFog;
uniform vec4 fogColor;
uniform float fogDensity;

//day/night
uniform bool enableNight;

//gouraud
uniform bool gouraud;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec4 GouraudColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalcFogFactor(vec3 fragPos, vec3 viewPos);

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0); 
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = normalMatrix * aNormal;
	TexCoords = aTexCoords;
	
	vec3 localNormal = normalMatrix * aNormal;
	vec3 localFragPos = vec3(model * vec4(aPos, 1.0));
	vec2 localTexCoords = aTexCoords;

	if (gouraud)
	{
		vec3 norm = normalize(localNormal);
		vec3 viewDir = normalize(viewPos - localFragPos);

		vec3 result = vec3(0.0, 0.0, 0.0);
		// phase 1: Directional lighting
		//vec3 result = CalcDirLight(dirLight, norm, viewDir);
		DirLight localDirLight = dirLight;
		if (enableNight)
		{
			localDirLight.ambient *= 0;
			localDirLight.diffuse *= 0;
			localDirLight.specular *= 0;
		}
		if (enableFog)
		{
			localDirLight.ambient /= 2;
			localDirLight.diffuse /= 2;
			localDirLight.specular /= 2;
		}
		result += CalcDirLight(localDirLight, norm, viewDir);

		//// phase 2: Point lights
		//for (int i = 0; i < NR_POINT_LIGHTS; i++)
		//	result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

		// phase 3: Spot light
		//result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
		for (int i = 0; i < NR_SPOT_LIGHTS; i++)
			result += CalcSpotLight(spotLights[i], norm, localFragPos, viewDir);

		if (!enableFog)
			GouraudColor = vec4(result, 1.0);
		else
		{
			float fogFactor = CalcFogFactor(localFragPos, viewPos);
			GouraudColor = mix(fogColor, vec4(result, 1.0), fogFactor);
		}
	}
}

float CalcFogFactor(vec3 fragPos, vec3 viewPos)
{
	float dist = distance(viewPos, fragPos);
	float fogFactor = 1.0 / exp((dist * fogDensity)* (dist * fogDensity));
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	return fogFactor;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), texture_diffuse1_shininess);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, aTexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, aTexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, aTexCoords));
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), texture_diffuse1_shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, aTexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, aTexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, aTexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), texture_diffuse1_shininess);

	//attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//spotlight
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// combine results
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, aTexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, aTexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, aTexCoords));

	ambient *= attenuation * 0;
	diffuse *= attenuation;
	specular *= attenuation;

	diffuse *= intensity;
	specular *= intensity;

	return (ambient + diffuse + specular);
}