#ifndef ANIMMODEL
#define ANIMMODEL

//anim mesh for itself and glew
#include "AnimMesh.h"

//std includes
#include <vector>
#include <string>
#include <map>

#include "glm/glm/gtc/quaternion.hpp"

//assimp
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

using namespace std;

class AnimModel
{
public:
	AnimModel();
	~AnimModel();
	static const uint MAX_BONES = 100;

	//animation testing function
	void setAnim(int n);

	void initShaders(GLuint shader_program);
	void loadModel(const string& path);
	void update();
	void draw(GLuint shaders_program, bool isAnimated);
	void playSound();
	void showNodeName(aiNode* node);

	//get texture using soil
	GLuint TextureFromFile(const char * path);

	//utility functions
	glm::mat4 aiToGlm(aiMatrix4x4 ai_matr);
	aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend); 

	//for rotating a selected body part
	glm::quat rotate_head_xz = glm::quat(cos(glm::radians(0.0f)), sin(glm::radians(0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f)); 

private:
	//assimp variables
	Assimp::Importer import;
	const aiScene* scene;
	vector<AnimMesh> meshes;
	string directory;

	//bone variables
	map<string, uint> m_bone_mapping;
	uint m_num_bones = 0;
	vector<BoneMatrix> m_bone_matrices;
	aiMatrix4x4 m_global_inverse_transform;

	GLuint m_bone_location[MAX_BONES];
	float ticks_per_second = 0.0f;

	//animation testing variable
	int lefthand = 2;

	//loading functionality
	void processNode(aiNode* node, const aiScene* scene);
	AnimMesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<AnimTexture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, string type_name);

	void searchNode(aiNode* node);
	aiNode* armNode;

	//find initial positions
	uint findPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	uint findRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	uint findScaling(float p_animation_time, const aiNodeAnim* p_node_anim);
	const aiNodeAnim* findNodeAnim(const aiAnimation* p_animation, const string p_node_name);

	// calculate position of bones between frames
	aiVector3D calcInterpolatedPosition(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiQuaternion calcInterpolatedRotation(float p_animation_time, const aiNodeAnim* p_node_anim);
	aiVector3D calcInterpolatedScaling(float p_animation_time, const aiNodeAnim* p_node_anim);

	//initialise bone reading for getting positions
	void readNodeHierarchy(float p_animation_time, const aiNode* p_node, const aiMatrix4x4 parent_transform);
	void boneTransform(double time_in_sec, vector<aiMatrix4x4>& transforms);

	
};

#endif