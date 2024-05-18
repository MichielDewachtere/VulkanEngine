#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
//    vec4 texColor = texture(texSampler, fragTexCoord);
//    outColor = texColor; // Ensure this has an alpha component
//
//    const vec3 lightDirection = normalize(vec3(0.736, 0.626, -0.261));
//    const vec4 lightColor = vec4(1,1,1,1);
//
//    // Calculate the diffuse factor using Lambert's Law
//    float diffuseFactor = max(dot(fragNormal, lightDirection), 0.25);
//
//    // Calculate the final color using the diffuse factor and the light and object colors
//    vec4 diffuseColor = lightColor * vec4(texture(texSampler, fragTexCoord)) * diffuseFactor;
//
//    // Output the final color
//    outColor = diffuseColor;

    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = texColor; // Use the alpha channel from the texture
}