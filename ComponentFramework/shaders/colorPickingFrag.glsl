#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 2) uniform uint  colorID;
layout(location = 0) out vec4 fragColor;

/// Doing it this way allows me to pick any of 16,777,216  objects on the screen
void main() {
	float redColor = float(0x000000FF & colorID) / 255.0;
	float greenColor = float( (0x0000FF00 & colorID)>>8 ) / 255.0;
	float blueColor = float( (0x00FF0000 & colorID)>>16 ) / 255.0;
	float alphaColor = 0.0;
	
	fragColor = vec4(redColor,greenColor,blueColor,alphaColor);   
	
}