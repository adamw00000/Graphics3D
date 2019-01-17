#version 330 core
in vec3 FragPos;

out vec4 FragColor;

uniform bool enableFog;
uniform vec4 fogColor;
uniform float fogDensity;
uniform vec3 viewPos;

float CalcFogFactor(vec3 fragPos, vec3 viewPos);

void main()
{

	if (enableFog)
	{
		float fogFactor = CalcFogFactor(FragPos, viewPos);
		FragColor = mix(fogColor, vec4(1.0), fogFactor);
	}
	else
		FragColor = vec4(1.0); // set alle 4 vector values to 1.0
}

float CalcFogFactor(vec3 fragPos, vec3 viewPos)
{
	float dist = distance(viewPos, fragPos);
	float fogFactor = 1.0 / exp((dist * fogDensity)* (dist * fogDensity));
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	return fogFactor;
}