#version 460 core
// Input from vertex shader
in vec3 FragPos;
in vec3 Normal;

// Output to framebuffer
out vec4 FragColor;

// Uniforms
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

// Function to calculate reflection
vec3 reflect(vec3 I, vec3 N) {
    return I - 2.0 * dot(I, N) * N;
}

void main() {
    // Ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    
    // Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    
    // Specular light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);
    
    // Combined lighting
    vec3 result = (ambient + diffuse + specular) * texture(texture_diffuse1, TexCoords).rgb;
    
    // Reflection calculation
    vec3 reflectedDir = reflect(viewDir, norm);
    vec3 reflectedColor = texture(skybox, reflectedDir).rgb;
    
    // Final color with reflections
    FragColor = vec4(result + reflectedColor, 1.0);
}
