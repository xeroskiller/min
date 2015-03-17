#version 330
layout(location=0)in vec4 in_Position;
layout(location=1)in vec2 in_UV;
layout(location=2)in vec3 in_Tangent;
layout(location=3)in vec3 in_BiTangent;
layout(location=4)in vec3 in_Norm;
layout(location=5)in vec4 in_BoneWeight;
layout(location=6)in vec4 in_BoneIndex;

out vec2 UV;
out vec3 eye_direction_T;
out vec3 l_direction_T;

uniform mat4 BoneMatrix[58];
uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;

uniform vec3 Lights[1];

uniform vec3 MaterialProperty;

void main(void)
{
	mat4 matTransform = BoneMatrix[int(in_BoneIndex.x)] * in_BoneWeight.x;
	matTransform += BoneMatrix[int(in_BoneIndex.y)] * in_BoneWeight.y;
	matTransform += BoneMatrix[int(in_BoneIndex.z)] * in_BoneWeight.z;
	float fw = 1.0 - (in_BoneWeight.x + in_BoneWeight.y + in_BoneWeight.z);
	matTransform += BoneMatrix[int(in_BoneIndex.w)] * fw;
	
	mat4 MVT = ViewMatrix*ModelMatrix*matTransform;
	mat3 MV = mat3(MVT);
	vec3 Normal_C = MV * in_Norm;
	vec3 Tangent_C = MV * in_Tangent;
	vec3 BiTangent_C = MV * in_BiTangent;
	mat3 TBN = transpose(mat3(Tangent_C, BiTangent_C, Normal_C));
	
	gl_Position = (ProjectionMatrix*MVT)*in_Position;
	vec3 v_position_C = (MVT * in_Position).xyz;
	vec3 eye_direction_C = vec3(0,0,0) - v_position_C;
	vec3 l_position_C = (ViewMatrix * vec4(Lights[0], 1)).xyz;
	vec3 l_direction_C = l_position_C - v_position_C;
	
	eye_direction_T = normalize(TBN * eye_direction_C);
	l_direction_T = normalize(TBN * l_direction_C);
	
	UV = in_UV;
}
