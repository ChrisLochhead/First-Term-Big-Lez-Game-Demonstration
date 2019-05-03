#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 vertexNormal_modelspace;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;  


// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 ShadowCoord;
out vec4 viewSpace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightInvDirection_worldspace;
uniform mat4 DepthBiasMVP;


void main(){

	// for fog shading
	viewSpace = V * M * vec4(vertexPosition_modelspace,1);

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	
	ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace,1);
	
	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	EyeDirection_cameraspace = vec3(0,0,0) - ( V * M * vec4(vertexPosition_modelspace,1)).xyz;

	// Vector that goes from the vertex to the light, in camera space
	LightDirection_cameraspace = (V*vec4(0.0, 0.0, 0.0 ,0)).xyz;
	
	// Normal of the the vertex, in camera space
	//Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; 
	Normal_cameraspace = normalize(vec3(transpose(inverse(M)) * vec4(vertexNormal_modelspace, 0)));
	// UV of the vertex. No special space for this one.
	UV = vertexUV;

}
