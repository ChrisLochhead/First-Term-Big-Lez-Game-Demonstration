#version 330 core

in vec2 text_coords;
in vec3 frag_pos;
in vec3 normal;
in vec4 viewSpace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

#define NUMBER_OF_POINT_LIGHTS 4

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
	sampler2D texture_diffuse1;

	sampler2D texture_specular1;

	float shininess;
	float transparency;
};


struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct Light
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform Material material;
uniform Light light;
uniform Light light1;
uniform Light light2;
uniform Light light3;
uniform vec3 view_pos;
uniform DirLight dirLight;
//uniform PointLight pointLights[NUMBER_OF_POINT_LIGHTS];


//fog variables
const vec3 fogColor = vec3(0.5, 0.5,0.5);
const float FogDensity = 0.1;


out vec4 outColor;


vec3 CalcSpotLight( Light light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.texture_diffuse1, text_coords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.texture_diffuse1, text_coords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.texture_specular1, text_coords ) );
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return ( ambient + diffuse + specular );
}

vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir )
{
     vec3 lightDir = normalize( -light.direction );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
        // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.texture_diffuse1, text_coords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.texture_diffuse1, text_coords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.texture_specular1, text_coords ) );
    
    return ( ambient + diffuse + specular );
}

vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
        // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.texture_diffuse1, text_coords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.texture_diffuse1, text_coords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.texture_specular1, text_coords ) );
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ( ambient + diffuse + specular );
}

void main()
{
	vec3 view_dir = normalize(view_pos - frag_pos);

    vec3 calc_color = CalcSpotLight(light, normal, frag_pos, view_dir);
    calc_color += CalcSpotLight(light1, normal, frag_pos, view_dir);
    calc_color += CalcSpotLight(light2, normal, frag_pos, view_dir);
    calc_color += CalcSpotLight(light3, normal, frag_pos, view_dir);
    calc_color += CalcDirLight(dirLight, normal, view_dir);

	outColor = vec4(calc_color, 1.0);


///fog shading /////////////////////////////////////////////////////

vec3 RimColor = vec3(0.2, 0.2, 0.2);
vec3 tex1 = texture(material.texture_diffuse1, text_coords).rgb;
 

//get light an view directions
vec3 L = normalize( LightDirection_cameraspace);
vec3 V = normalize( EyeDirection_cameraspace);
 
//diffuse lighting
vec3 fogNorm = normalize( normal );
vec3 diffuse = light.diffuse * max(0, dot(L, fogNorm));
 
//rim lighting
float rim = 1 - max(dot(V, fogNorm), 0.0);
rim = smoothstep(0.6, 1.0, rim);
vec3 finalRim = RimColor * vec3(rim, rim, rim);
//get all lights and texture
vec3 lightColor = finalRim + diffuse + tex1;
 
vec3 finalColor = vec3(0, 0, 0);
 
//distance
float dist = 0;
float fogFactor = 0;
 


//compute distance used in fog equations

   //range based
   dist = length(viewSpace);

   fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
   fogFactor = clamp( fogFactor, 0.0, 1.0 );
   finalColor = mix(fogColor, vec3(outColor), fogFactor);

   outColor = vec4(finalColor,1);
}

