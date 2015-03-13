#version 330
layout(location=0)in vec4 in_Position;
layout(location=1)in vec2 in_UV;
layout(location=2)in vec3 in_Norm;

out vec2 UV;
out vec3 ref_color;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;

uniform vec3 Lights[1];
uniform vec3 LightColor[1];
uniform float LightPower[1];

uniform vec3 MaterialProperty;

void main(void)
{
	gl_Position = (ProjectionMatrix*ViewMatrix*ModelMatrix)*in_Position;
	vec3 v_position_C = (ViewMatrix * ModelMatrix * in_Position).xyz;
	vec3 eye_direction_C = vec3(0,0,0) - v_position_C;
	vec3 l_position_C = (ViewMatrix * vec4(Lights[0], 1)).xyz;
	vec3 l_direction_C = l_position_C - v_position_C;
	vec3 norm_C = (ViewMatrix*ModelMatrix*vec4(in_Norm, 0)).xyz;
	vec3 n = normalize(norm_C);
	vec3 l = normalize(l_direction_C);
	float cosTheta = clamp(dot(n, l), 0, 1);
	
	vec3 E = normalize(eye_direction_C);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp(dot(E,R), 0, 1);
	
	ref_color = MaterialProperty.x*LightColor[0] + MaterialProperty.y*LightPower[0]*cosTheta*LightColor[0] + MaterialProperty.z*LightPower[0]*LightColor[0]*pow(cosAlpha, 10);
	UV = in_UV;
}
