#version 330
in vec2 UV;
in vec3 ref_color;
uniform sampler2D Texture;

out vec3 out_Color;

void main(void)
{
	out_Color = texture( Texture, UV ).rgb * ref_color;
}
