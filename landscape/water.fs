#version 330 core

in vec4 clipspace;
in vec2 TexCoord;
in vec3 FragPos;
in float visibility;

out vec4 FragColor;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform vec3 skyColor;
uniform vec3 lightColor;
uniform float moveOffset;

const float distStrength = 0.004;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)//Only need to implement the specular part
{
    vec3 lightDir = normalize(-light.direction);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    return specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)//Only need to implement the specular part
{
    vec3 lightDir = normalize(light.position - fragPos);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 specular = light.specular * spec * material.specular;

    specular *= attenuation;
    return specular;
}

void main()
{
    vec2 ndc = (clipspace.xy/clipspace.w)/2.0 + 0.5;
    vec2 refractTexCoord = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoord = vec2(ndc.x, -ndc.y);

    float near = 0.1;
    float far = 1000.0;
    float depth = texture(depthMap, refractTexCoord).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

    float waterDepth = floorDistance - waterDistance;

    vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoord.x + moveOffset, TexCoord.y)).rg*0.1;
	distortedTexCoords = TexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveOffset);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * distStrength * clamp(waterDepth * 0.5, 0.0, 1.0);

    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    refractTexCoord += totalDistortion;
    refractTexCoord = clamp(refractTexCoord, 0.001, 0.999);
    reflectTexCoord += totalDistortion;
    reflectTexCoord.x = clamp(reflectTexCoord.x, 0.001, 0.999);
    reflectTexCoord.y = clamp(reflectTexCoord.y, -0.999, -0.001);
    float refractiveFactor = dot(normalize(viewPos - FragPos), normalize(vec3(normal.x, normal.y + 3.0, normal.z)));
    refractiveFactor = clamp(pow(refractiveFactor, 2.0), 0.0, 1.0);//The greater the expon the more the water will reflect

    
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, normal, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir); 
    
    result *= clamp(waterDepth * 5, 0.0, 1.0);
    vec4 reflectColor = texture(reflection, reflectTexCoord);
    vec4 refractColor = texture(refraction, refractTexCoord);
    FragColor = mix(mix(reflectColor, refractColor, refractiveFactor), vec4(0.0, 0.3, 0.5, 1.0), 0.05) + vec4(result, 0.0) * 1.0;
    //FragColor = mix(vec4(skyColor, 1.0), FragColor, visibility);
    FragColor.a = clamp(waterDepth * 1.5, 0.0, 1.0);
    
}