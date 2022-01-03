#version 330 core
out vec4 FragColor;

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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform LampLight lampLight;
uniform SpotlightLight spotlightLight;
uniform float brightnessLevel;
uniform bool useBlinn;

vec3 CalcLampLight(LampLight lampLight, vec3 fragPos, vec3 normal);
vec3 CalcSpotlightLight(SpotlightLight spotlightLight, vec3 fragPos, vec3 normal);

void main()
{
    vec3 result = CalcLampLight(lampLight, FragPos, Normal);
    result += CalcSpotlightLight(spotlightLight, FragPos, Normal);
    FragColor = vec4(result, 1.0);
}

vec3 CalcLampLight(LampLight lampLight, vec3 fragPos, vec3 normal) 
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

vec3 CalcSpotlightLight(SpotlightLight spotlightLight, vec3 fragPos, vec3 normal)
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
    }
    vec3 specular = spotlightLight.specular * lampLight.specular * (spec * material.specular);
        
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