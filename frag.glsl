#version 410 core

out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    const float lg2 = 1.442695;
    float z = gl_FragCoord.z / gl_FragCoord.w;
    z = z / 2.0;
    float factor = 1.0 - clamp(exp2(z * z * lg2) - 1.0, 0.0, 1.0);
    FragColor = mix(vec4(vec3(0.0), 1.0), texture(ourTexture, TexCoord), factor);
}
