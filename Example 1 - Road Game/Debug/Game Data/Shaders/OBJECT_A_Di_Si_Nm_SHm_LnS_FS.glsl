#version 330

varying vec2 UV;
varying vec3 eye_direction_T;
varying vec3 l_direction_T;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;
uniform vec3 MaterialProperty;

uniform vec3 LightColor[1];
uniform float LightPower[1];

out vec3 out_Color;

void main(void)
{	
	float bumpScale = 0.045;
	float height = 1.0;
	
	float numSteps = 10;
	float numShadowSteps = 10;
	
	vec2 offsetCoord = UV;
	vec2 oldCoord = offsetCoord;
	
	vec4 NB = texture2D(Texture2, offsetCoord);
	NB.a = length(texture2D( Texture3, offsetCoord ).rgb);
	
	vec3 tsE = normalize(eye_direction_T);
	
	numSteps = mix(numSteps*2, numSteps, tsE.z);
	
	float step = 1.0/ numSteps;
	vec2 delta = vec2(-tsE.x, -tsE.y) * bumpScale / (tsE.z * numSteps);
	
	while(NB.a < height)
	{
		oldCoord = offsetCoord;
		height -= step;
		offsetCoord += delta;
		NB = texture2D(Texture2, offsetCoord);
		NB.a = length(texture2D( Texture3, offsetCoord ).rgb);
	}
	
	if(height < 0.0)
	{
		height = 0.0;
		offsetCoord = oldCoord;
	}
	
	vec3 color = texture2D(Texture1, offsetCoord).rgb;
	
	vec3 tsL = normalize(l_direction_T);
	
	vec3 tsN = normalize(NB.xyz*2.0 - 1.0);
	tsN = normalize(texture2D(Texture2, offsetCoord + vec2(tsN.x, tsN.y)*10).xyz * 2 - 1 + tsN);
	
	vec3 tsH = normalize(tsL + tsE);
	
	float NdotL = max(0 , dot(tsN, tsL));
	float NdotH = max(0 , dot(tsN, tsH));
	float spec = NdotH * NdotH;
	
	float selfShadow = 1.0;
	height = 1.0;
	
	if(NdotL > 0)
	{
		numShadowSteps = mix(2*numShadowSteps,numShadowSteps,tsL.z);
		step = 1.0 / numShadowSteps;
		delta = vec2(-tsL.x, -tsL.y) * bumpScale / (numShadowSteps * tsL.z);
		
		height = NB.a + step * 0.1;
		
		while((NB.a < height) && (height < 1))
		{
			height += step;
			offsetCoord += delta;
			NB = texture2D(Texture2, offsetCoord);
			NB.a = length(texture2D( Texture3, offsetCoord ).rgb);
			selfShadow -= step;
		}
		
		if(NB.a < height)
		{
			selfShadow = 1;
		}
		else
		{
			selfShadow /= 4;
		}
		selfShadow = selfShadow * selfShadow;
	}
	
	vec3 R = reflect(-tsL,tsN);
	float cosAlpha = clamp(dot(tsE,R), 0, 1);
	vec3 ref_color = MaterialProperty.x*LightColor[0] + MaterialProperty.y*LightPower[0]*LightColor[0]*NdotL*selfShadow + MaterialProperty.z*LightPower[0]*LightColor[0]*pow(cosAlpha, 5)*selfShadow;
	
	out_Color =  (color * ref_color);
}
