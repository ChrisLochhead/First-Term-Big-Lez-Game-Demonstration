#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;  


out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

//normal mapping stuff
out mat3 TBN;

//shadowmapping stuff
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 ShadowCoord;

//out vec2 UV;   //TexCoords
//out vec3 Position_worldspace;   //FragPos
//out vec3 Normal_cameraspace;   // Normal



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//shadow mapping uniforms
uniform mat4 DepthBiasMVP;
uniform vec3 LightInvDirection_worldspace;


void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;

    // In camera space, the camera is at the origin (0,0,0).
	EyeDirection_cameraspace = vec3(0,0,0) - ( view * model * vec4(position,1)).xyz;

	ShadowCoord = DepthBiasMVP * vec4(position,1);
	
	// Vector that goes from the vertex to the light, in camera space
	LightDirection_cameraspace = (view * vec4(LightInvDirection_worldspace,0)).xyz;
	

    // assemble our TBN matrix for use in normal mapping calculations
    vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal,    0.0)));
    TBN = mat3(T, B, N); //send out for use in the fragment shader

}