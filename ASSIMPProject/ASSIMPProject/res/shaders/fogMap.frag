#version 330 core


struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{

	float cutOff;
	float outerCutOff;  // values to determine the spotlight size

	float constant;
	float linear;
	float quadratic;  // attenuation values

	vec3 ambient;
	vec3 diffuse;
	vec3 specular; // light values
	vec3 position;
	vec3 direction;

};

struct PointLight
{
    float constant;
  float linear;
  float quadratic;  // attenuation values

  vec3 ambient;
  vec3 diffuse;
  vec3 specular; // light values
  vec3 position;
};

struct Material
{

    sampler2D diffuse;
    sampler2D specular;
    float 	  shininess;
};

uniform sampler2D texture_diffuse1; // the diffuse texture
uniform sampler2D texture_normals1; // the normal map texture

uniform DirLight dirLight;
//uniform PointLight pointLight;
uniform SpotLight light;
uniform SpotLight light1;
uniform SpotLight light2;
uniform SpotLight light3;
uniform Material material;


//fog uniforms
uniform int fogSelector;
//0 plane based; 1 range based
uniform int depthFog;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 ShadowCoord;

//fog shading
in vec4 viewSpace;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform vec3 viewPos;
uniform bool normalMapped;
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform sampler2DShadow shadowMap;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

//fog variables
const vec3 fogColor = vec3(0.5, 0.5,0.5);
const float FogDensity = 0.1;

//function prototypes
vec3 CalcDirLight( DirLight light, vec3 normal, float cosA, float visibility , float cosT);
vec3 CalcSpotLight( SpotLight light, vec3 normal, float cosA, float visibility , float cosT );
vec3 CalcPointLight( PointLight light, vec3 normal, float cosA, float visibility , float cosT );

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main(){

/////////////////////////////////////////////////// normal mapping calculations/////////////////////
    // Properties
    vec3 norm = normalize( Normal_cameraspace );
 

////////////////////////////////////////////////light calculations with materials ///////////////////
	// Light emission properties
	vec3 LightColor = vec3(0.5,1,1);
	float LightPower = 1.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture( material.diffuse, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3( texture( material.specular, UV) );

////////////////////////////////////////////////light calculations //////////////////////////////////

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );

	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );

	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);

	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);

	float cosAlpha = clamp( dot( E,R ), 0,0.1 );

	////////////////////////////////////shadow mapping calculations for directional light ////////////////////////////
	float visibility=1.0;

	// Fixed bias
	float bias = 0.005;

	// Sample the shadow map 4 times
	for (int i=0;i<4;i++){
		int index = i;
		visibility -= 0.1*(1.0-texture( shadowMap, vec3(ShadowCoord.xy + poissonDisk[index]/700.0,  (ShadowCoord.z-bias)/ShadowCoord.w) ));
	}


	///////////////////////////////////////adding the light together ////////////////////////////////////

	color = CalcDirLight(dirLight, norm, cosAlpha, visibility, cosTheta);
	color += CalcSpotLight(light, norm,  cosAlpha, visibility , cosTheta );
  color += CalcSpotLight(light1, norm,  cosAlpha, visibility , cosTheta );
  color += CalcSpotLight(light2, norm,  cosAlpha, visibility , cosTheta );
  color += CalcSpotLight(light3, norm,  cosAlpha, visibility , cosTheta );
  //color += CalcPointLight(pointLight, norm,  cosAlpha, visibility , cosTheta );
	
/////////////////////////////////fog shading/////////////////////////////////

vec3 RimColor = vec3(0.2, 0.2, 0.2);
vec3 tex1 = texture(material.diffuse, UV).rgb;
 
//get light an view directions
vec3 L = normalize( LightDirection_cameraspace);
vec3 V = normalize( EyeDirection_cameraspace);
 
//diffuse lighting
vec3 fogNorm = normalize( Normal_cameraspace );
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
if(depthFog == 0)//select plane based vs range based
{
  //plane based
  dist = abs(viewSpace.z);
  //dist = (gl_FragCoord.z / gl_FragCoord.w);
}
else
{
   //range based
   dist = length(viewSpace);
}
 
if(fogSelector == 0)//linear fog
{
   // 20 - fog starts; 80 - fog ends
   fogFactor = (80 - dist)/(80 - 20);
   fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
   //if you inverse color in glsl mix function you have to
   //put 1.0 - fogFactor
   finalColor = mix(fogColor, color, fogFactor);
}
else if( fogSelector == 1)// exponential fog
{
    fogFactor = 1.0 /exp(dist * FogDensity);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
    // mix function fogColor⋅(1−fogFactor) + lightColor⋅fogFactor
    finalColor = mix(fogColor, color, fogFactor);
}
else if( fogSelector == 2)
{
   fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
   fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
   finalColor = mix(fogColor, color, fogFactor);
}

 ////////////////////////////////////////////ground fog /////////////////////////////////////////////
/*
 //my camera y is 10.0. you can change it or pass it as a uniform
float be = (1.0 - viewSpace.y) * 0.004;//0.004 is just a factor; change it if you want
float bi = (1.0 - viewSpace.y) * 0.001;//0.001 is just a factor; change it if you want
 
//OpenGL SuperBible 6th edition uses a smoothstep function to get
//a nice cutoff here
//You have to tweak this values
// float be = 0.025 * smoothstep(0.0, 6.0, 32.0 - viewSpace.y);
// float bi = 0.075* smoothstep(0.0, 80, 10.0 - viewSpace.y);
 
float ext = exp(-dist * be);
float insc = exp(-dist * bi);
 
finalColor = color * ext + fogColor * (1 - insc);
*/
////////////////////////////////////////////

//show fogFactor depth(gray levels)
//fogFactor = 1 - fogFactor;
//out_color = vec4( fogFactor, fogFactor, fogFactor,1.0 );
color = finalColor;



}

vec3 CalcDirLight( DirLight light, vec3 normal, float cosA, float visibility, float cosT )
{
	 vec3 lightDir = normalize( -light.direction );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    //float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Combine results
    vec3 diffuse = light.diffuse  * diff * vec3( texture( material.diffuse,  UV ) );
    vec3 ambient =visibility * light.ambient * vec3( texture( material.diffuse, UV ) );
    vec3 specular = visibility * light.specular * cosA* vec3( texture( material.specular, UV ) );

    //vec3 MaterialDiffuseColor = texture( material.diffuse, UV ).rgb;
   // vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	//vec3 MaterialSpecularColor = vec3( texture( material.specular, UV) );
    
    return ( ambient + diffuse + specular );
}

vec3 CalcSpotLight( SpotLight light, vec3 normal, float cosA, float visibility , float cosT  )
{

    vec3 lightDir = normalize( light.position - Position_worldspace );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
   // float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Attenuation
    float distance = length( light.position - Position_worldspace);
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, UV ) );
    vec3 diffuse =  light.diffuse * diff * vec3( texture( material.diffuse, UV ) );
    vec3 specular = light.specular * cosA * vec3( texture( material.specular, UV ) );
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
  
    return ( ambient + diffuse + specular );
}

// Calculates the color when using a point light.
vec3 CalcPointLight( PointLight light, vec3 normal, float cosA, float visibility , float cosT )
{
    vec3 lightDir = normalize( light.position - Position_worldspace  );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
   // float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Attenuation
    float distance = length( light.position - Position_worldspace );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, UV ) );
    vec3 diffuse = visibility * light.diffuse * diff * vec3( texture( material.diffuse, UV ) );
    vec3 specular = visibility * light.specular * cosT * vec3( texture( material.specular, UV ) );
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ( ambient + diffuse + specular );
}