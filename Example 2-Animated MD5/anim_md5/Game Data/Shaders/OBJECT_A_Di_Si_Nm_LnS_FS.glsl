#version 330
in vec2 UV;
in vec3 eye_direction_T;
in vec3 l_direction_T;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform vec3 MaterialProperty;

uniform vec3 LightColor[1];
uniform float LightPower[1];

out vec3 out_Color;

void main(void)
{
	vec3 n = normalize(texture2D( Texture2, UV ).rgb*2.0 - 1.0);
	vec3 l = normalize(l_direction_T);
	
	float cosTheta = clamp(dot(n, l), 0, 1);
	
	vec3 E = normalize(eye_direction_T);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp(dot(E,R), 0, 1);
	
	vec3 ref_color = MaterialProperty.x*LightColor[0] + MaterialProperty.y*LightPower[0]*cosTheta*LightColor[0] + MaterialProperty.z*LightPower[0]*LightColor[0]*pow(cosAlpha, 10);
	out_Color = texture( Texture1, UV ).rgb * ref_color;
}
