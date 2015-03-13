#version 330
in vec2 UV;
in vec3 eye_direction_T;
in vec3 l_direction_T;
varying vec3 normal_T;

uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;

uniform vec3 MaterialProperty;

uniform vec3 LightColor[1];
uniform float LightPower[1];

out vec3 out_Color;

void main(void)
{
	vec3 n = normal_T;
	vec3 l = l_direction_T;
	
	float cosTheta = clamp(dot(n, l), 0, 1);
	
	vec3 E = eye_direction_T;
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp(dot(E,R), 0, 1);
	
	vec3 ref_color = MaterialProperty.x*LightColor[0] + MaterialProperty.y*LightPower[0]*cosTheta*LightColor[0] + MaterialProperty.z*LightPower[0]*LightColor[0]*pow(cosAlpha, 10);
	
	vec4 text1 = texture( Texture1, UV );
	vec4 text2 = texture( Texture2, UV );
	vec4 text3 = texture( Texture3, UV );
	
	vec3 temp1 = text1.rgb * (1.0 - text2.a);
	
	vec3 temp2 = (text2.rgb * text2.a) * (1.0 - text3.a);
	
	vec3 temp3 = text3.rgb * text2.a * text3.a;
	
	out_Color =  (temp1 + temp2 + temp3) * ref_color;
}
