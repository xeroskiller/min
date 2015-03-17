#version 330
layout(location=0)in vec4 in_Position;
layout(location=1)in vec4 in_Color;

out vec4 color;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;

void main(void)
{
	gl_Position = (ProjectionMatrix*ViewMatrix*ModelMatrix)*in_Position;
	color = in_Color;
}