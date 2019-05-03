// textured.frag
#version 330

// Some drivers require the following
precision highp float;

uniform samplerCube texture_diffuse1;

in vec3 ex_TexCoord;
layout(location = 0) out vec4 out_Color;
 
void main(void) {
    
	// Fragment colour
	out_Color = texture(texture_diffuse1, ex_TexCoord);
}