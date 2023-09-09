#version 330 core
out vec4 Fragcolor;
in vec3 color;
in vec2 texCoord;
uniform sampler2D tex0;
void main()
{
	Fragcolor=texture(tex0,texCoord);
}