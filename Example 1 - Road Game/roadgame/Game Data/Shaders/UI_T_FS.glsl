#version 330
in vec2 UV;
uniform sampler2D TextTable2D;

out vec4 out_Color;

void main(void)
{
	out_Color = texture( TextTable2D, UV ).rgba;
}