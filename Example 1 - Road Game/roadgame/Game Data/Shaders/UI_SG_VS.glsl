#version 330
layout(location=0)in vec2 in_Position;
layout(location=1)in vec2 in_UV;

uniform mat3 ModelMatrix;

out vec2 UV;

void main(void)
{
	vec3 newpos = ModelMatrix * vec3(in_Position, 1);
	gl_Position = vec4(newpos.x, newpos.y, 0, 1);
	UV = in_UV;
}
