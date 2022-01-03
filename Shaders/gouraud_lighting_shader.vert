#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct LampLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float brightnessLevel;
};

struct SpotlightLight {
    bool ON;

    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform LampLight lampLight;
uniform SpotlightLight spotlightLight;
uniform float brightnessLevel;
uniform bool useBlinn;
uniform float fogLevel;

float CalcFogFactor(vec3 FragPos);
vec3 CalcLampLight(LampLight lampLight, vec3 fragPos, vec3 normal, vec2 TexCoords);
vec3 CalcSpotlightLight(SpotlightLight spotlightLight, vec3 fragPos, vec3 normal, vec2 TexCoords);

out vec4 fragColor;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    vec2 TexCoords = aTexCoords;    
    gl_Position = projection * view * vec4(FragPos, 1.0);

    vec3 result = CalcLampLight(lampLight, FragPos, Normal, TexCoords);
    result += CalcSpotlightLight(spotlightLight, FragPos, Normal, TexCoords);

    float fogFactor = CalcFogFactor(FragPos);   
    result = mix(vec3(0.05f), result, fogFactor);

    fragColor = vec4(result, 1.0);
}

float CalcFogFactor(vec3 FragPos) {
    if (fogLevel == 0) return 1;
    float gradient = (fogLevel * fogLevel - 7 * fogLevel + 28) / 2;
    float distance = length(viewPos - FragPos);

    float fogFactor = exp(-pow((distance / gradient), 5)) ;

    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}

vec3 CalcLampLight(LampLight lampLight, vec3 fragPos, vec3 normal, vec2 TexCoords) 
{
    float distance    = length(lampLight.position - fragPos);
    float attenuation = 1.0 / (lampLight.constant + lampLight.linear * distance + 
    		            lampLight.quadratic * (distance * distance)); 

    // ambient
    vec3 ambient = lampLight.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lampLight.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lampLight.brightnessLevel * lampLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = 0.0;
    if(useBlinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = lampLight.brightnessLevel * lampLight.specular * (spec * material.specular);

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation; 

    return (ambient + diffuse + specular);
}

vec3 CalcSpotlightLight(SpotlightLight spotlightLight, vec3 FragPos, vec3 Normal, vec2 TexCoords)
{
    if (spotlightLight.ON == false) return vec3(0);

    vec3 lightDir = normalize(spotlightLight.position - FragPos);
    
    float theta = dot(lightDir, normalize(-spotlightLight.direction)); 
      
    // ambient
    vec3 ambient = spotlightLight.ambient * texture(material.diffuse, TexCoords).rgb;
        
    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotlightLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
        
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    if(useBlinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }vec3 specular = spotlightLight.specular * lampLight.specular * (spec * material.specular);
        
    // spotlight (smooth edges)
    float epsilon = (spotlightLight.cutOff - spotlightLight.outerCutOff);
    float intensity = clamp((theta - spotlightLight.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance    = length(spotlightLight.position - FragPos);
    float attenuation = 1.0 / (spotlightLight.constant + spotlightLight.linear * distance + spotlightLight.quadratic * (distance * distance));    

    ambient  *= attenuation;
    diffuse   *= attenuation;
    specular *= attenuation;   
            
    return (ambient + diffuse + specular);
}