#version 330
layout(location=0)in vec4 in_Position;
layout(location=1)in vec2 in_UV;
layout(location=2)in vec3 in_Tangent;
layout(location=3)in vec3 in_BiTangent;
layout(location=4)in vec3 in_Norm;

varying vec2 UV;
varying vec3 eye_direction_T;
varying vec3 l_direction_T;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;

uniform vec3 Lights[1];

uniform vec3 MaterialProperty;

void main(void)
{
	mat3 MV = mat3(ViewMatrix*ModelMatrix);
	vec3 Normal_C = MV * normalize(in_Norm);
	vec3 Tangent_C = MV * normalize(in_Tangent);
	vec3 BiTangent_C = MV * normalize(in_BiTangent);
	mat3 TBN = transpose(mat3(Tangent_C, BiTangent_C, Normal_C));
	
	gl_Position = (ProjectionMatrix*ViewMatrix*ModelMatrix)*in_Position;
	vec3 v_position_C = (ViewMatrix * ModelMatrix * in_Position).xyz;
	vec3 eye_direction_C = vec3(0,0,0) - v_position_C;
	vec3 l_position_C = (ViewMatrix * vec4(Lights[0], 1)).xyz;
	vec3 l_direction_C = l_position_C - v_position_C;
	
	eye_direction_T = TBN * eye_direction_C;
	l_direction_T = TBN * l_direction_C;
	
	UV = in_UV;
}
