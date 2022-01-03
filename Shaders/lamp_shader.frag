#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D texture1;
uniform float brightnessLevel;
uniform float fogLevel;
uniform vec3 viewPos;

float CalcFogFactor();

void main()
{
    vec4 result = texture(texture1, TexCoord) * brightnessLevel;
    float fogFactor = CalcFogFactor();

    FragColor = mix(vec4(0.05f, 0.05f, 0.05f, 1.0), result, fogFactor);}

float CalcFogFactor() {
    if (fogLevel == 0) return 1;
    float gradient = (fogLevel * fogLevel - 7 * fogLevel + 28) / 2;
    float distance = length(viewPos - FragPos);

    float fogFactor = exp(-pow((distance / gradient), 5)) ;

    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}