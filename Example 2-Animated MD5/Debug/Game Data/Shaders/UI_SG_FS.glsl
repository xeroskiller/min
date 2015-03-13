#version 330

in vec2 UV;
uniform sampler2D Texture;

out vec4 out_Color;

void main(void)
{
	out_Color = texture( Texture, UV ).rgba;
}