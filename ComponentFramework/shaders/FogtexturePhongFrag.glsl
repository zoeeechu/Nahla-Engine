#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 eyeDir; 
layout(location = 2) in vec2 textureCoords; 
layout(location = 3) in vec3 LightsDir[4]; // takes location 3,4,5
layout(location = 7) in vec4 Lights_ks[4]; // takes location 6,7,8
layout(location = 11) in vec4 Lights_kd[4]; // takes location 9,10,11

layout(location = 15) in float FogValFromVert; 

uniform sampler2D myTexture; 

void main() {
		vec4 fogColor = vec4(0.8, 0.5, 0.2, 0.0);
		vec4 ks, kd;
		//vec4 ks = vec4(0.3, 0.3, 0.3, 0.0);
		//vec4 kd = vec4(0.6, 0.6, 0.3, 0.0);
		vec4 ka = 0.1 * kd; //outside forloop
		vec4 kt = texture(myTexture,textureCoords); 


		fragColor = ka * kt;
	for (int i = 0; i < 4; i++) {

		float diff = max(dot(vertNormal, LightsDir[i]), 0.0);

		ks = Lights_ks[i];
		kd = Lights_kd[i];
		//kd.y += i;

		/// Reflection is based incedent which means a vector from the light source
		/// not the direction to the light source so flip the sign
		vec3 reflection = normalize(reflect(-LightsDir[i], vertNormal));

		float spec = max(dot(eyeDir, reflection), 0.0);
		spec = pow(spec,14.0);
		//fragColor =   (diff * kd) + (spec * ks) * kt;	
		//fragColor +=  ((diff * kd) + (spec * ks)) * kt;	
		vec4 outputColor = ((diff * kd) + (spec * ks)) * kt + fragColor;	
	 	fragColor = mix(fogColor, outputColor, FogValFromVert);
		//fragColor = outputColor;
		
	}

}