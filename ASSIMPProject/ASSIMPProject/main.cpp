
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <stack>
#include <ctime>
#include "SDL.h"

//include objects
#include "Lights.h"
#include "Model.h"
#include "AnimModel.h"
#include "Shader.h"
#include "SkyBox.h"
#include "Camera.h"
#include "UI.h"

// Include GLFW
#include <GLFW/glfw3.h>

// Include Audio engine
#include <irrKlang.h>


#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

using namespace std;

//window 
GLFWwindow * window;

//callback variable
bool fired = false;

//camera 
Camera camera(glm::vec3(0.0f, 0.55f, 0.0f));

//time keeping
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//model stack
stack<glm::mat4> mStack;
stack<glm::mat4> msStack;

//directional light
DirLight dirLight = {
	5.0f, 1.8f, 0.1f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
};

//struct for the torch
SpotLight light{

glm::cos(glm::radians(12.5f)),
glm::cos(glm::radians(29.5f)),

1.0f, 0.09f, 0.032f,

{ 0.15f, 0.15f, 0.15f },
{ 0.0f, 0.0f, 0.0f },
{ 0.0f, 0.0f, 0.0f },
{ 2.5, - 3.4, - 0.18 },
{2.7,  1.29, 6.11 }

};

//struct for the lamps
SpotLight light1{

glm::cos(glm::radians(15.5f)),
glm::cos(glm::radians(25.5f)),

1.0f, 0.09f, 0.032f,

{ 0.7f, 0.7f, 0.7f },
{ 0.0f, 0.0f, 0.0f },
{ 0.0f, 0.0f, 0.0f },

{ 2.5, -3.4, -0.18 },
{ 2.5f, -3.8f, 6.1f },
};

PointLight pointLight{

1.0f, 0.09f, 0.032f,

{ 1.0f, 1.0f, 1.0f },
{ 0.8f, 0.8f, 0.8f },
{ 1.0f, 1.0f, 1.0f },
{6.07234f, -2.66478f, 0.179068 }

};

//normal map functions
bool normalMapped = true;


//glfw values
bool keys[1024];

GLfloat lastX = 400, lastY = 300;

bool firstMouse = true;

int windowWidth = 1024;
int windowHeight = 768;

// bias matrix
glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

//assimp models
Model *ground;
Model *roofs;
Model *props;
Model *lamps;
Model *gun;
Model *muzzleFlash;
Model *bullet;


//animated models
//AnimModel Leslie;
AnimModel Sassy;
//AnimModel Weapon;
//AnimModel Choomah;

//buffer and shader values
GLuint Framebuffer;
GLuint depthTexture;

//shaders
Shader *shadowShader;
Shader *shader;
Shader *animShader;
Shader *UIShader;

//matrices
glm::mat4 ProjectionMatrix;
glm::mat4 ViewMatrix;
glm::mat4 ModelMatrix;

//fog shader variables
int fogSelector = 2;
//0 plane based; 1 range based
int depthFog = 1;

//sound engine
irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine *AmbientEngine= irrklang::createIrrKlangDevice();

//character animation controllers
bool SassyAnim = false;
bool LeslieAnim = false;

GLfloat oY;

//gun variables
int ammo = 6;
bool bulletRemoved = false;
float reloadTimer = 3.0f;

//UI variables
glm::mat4 text_matrix_2D = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, 1.0f, -1.0f);

float alpha = 0.0f;

//menu variables
bool gameStart = false;
bool musicStarted = false;


//collision variables
std::vector<Model> collisionObjs;

//debug variable
float testervalue = 0.0f;
int test = 0;

//timer variables
float timer = 20.0f; 
float sassytimer = 15.0f;
float sassyInAnim = 0.0f;
float sassyAnimLength = 1.2;
float roundtimer = 15.0f;
bool alphaBool = false;
float muzzleTimer = 0.15f;
float secondMuzzleTimer = 1.0f;

//function prototypes
void setMaterial();
void setAnimLights();
void setModel(glm::mat4 ModelMatrix, GLuint shader);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);
void update();


void setMaterial()
{
	//glUniform3f(glGetUniformLocation(animShader->Program, "view_pos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
	glUniform1f(glGetUniformLocation(animShader->Program, "material.shininess"), 32.0f);
	glUniform1f(glGetUniformLocation(animShader->Program, "material.transparency"), 1.0f);

	glUniform3f(glGetUniformLocation(animShader->Program, "view_pos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

}
void setAnimLights()
{

	//camera light
	glUniform3f(glGetUniformLocation(animShader->Program, "light.ambient"), light.ambient[0], light.ambient[1], light.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light.diffuse"), light.diffuse[0], light.diffuse[1], light.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light.specular"), light.specular[0], light.specular[1], light.specular[2]);
	glUniform1f(glGetUniformLocation(animShader->Program, "light.constant"), light.constant);
	glUniform1f(glGetUniformLocation(animShader->Program, "light.linear"), light.linear);
	glUniform1f(glGetUniformLocation(animShader->Program, "light.quadratic"), light.quadratic);
	glUniform3f(glGetUniformLocation(animShader->Program, "light.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
	glUniform3f(glGetUniformLocation(animShader->Program, "light.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
	glUniform1f(glGetUniformLocation(animShader->Program, "light.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(animShader->Program, "light.outerCutOff"), light.outerCutOff);

	// set light and materials for the animated shader
	glUniform3f(glGetUniformLocation(animShader->Program, "light1.ambient"), light1.ambient[0], light1.ambient[1], light1.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light1.diffuse"), light1.diffuse[0], light1.diffuse[1], light1.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light1.specular"), light1.specular[0], light1.specular[1], light1.specular[2]);
	glUniform1f(glGetUniformLocation(animShader->Program, "light1.constant"), light1.constant);
	glUniform1f(glGetUniformLocation(animShader->Program, "light1.linear"), light1.linear);
	glUniform1f(glGetUniformLocation(animShader->Program, "light1.quadratic"), light1.quadratic);
	glUniform3f(glGetUniformLocation(animShader->Program, "light1.position"), light.position[0], light.position[1], light.position[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light1.direction"), light.direction[0], light.direction[1], light.direction[2]);
	glUniform1f(glGetUniformLocation(animShader->Program, "light1.cutOff"), light1.cutOff);
	glUniform1f(glGetUniformLocation(animShader->Program, "light1.outerCutOff"), light1.outerCutOff);

	//spotlight
	glUniform3f(glGetUniformLocation(animShader->Program, "light2.ambient"), light1.ambient[0], light1.ambient[1], light1.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light2.diffuse"), light1.diffuse[0], light1.diffuse[1], light1.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light2.specular"), light1.specular[0], light1.specular[1], light1.specular[2]);
	glUniform1f(glGetUniformLocation(animShader->Program, "light2.constant"), light1.constant);
	glUniform1f(glGetUniformLocation(animShader->Program, "light2.linear"), light1.linear);
	glUniform1f(glGetUniformLocation(animShader->Program, "light2.quadratic"), light1.quadratic);
	glUniform3f(glGetUniformLocation(animShader->Program, "light2.position"), light.position[0] + 3.8f, light.position[1], light.position[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light2.direction"), light.direction[0], light.direction[1], light.direction[2]);
	glUniform1f(glGetUniformLocation(animShader->Program, "light2.cutOff"), light1.cutOff);
	glUniform1f(glGetUniformLocation(animShader->Program, "light2.outerCutOff"), light1.outerCutOff);

	//spotlight
	glUniform3f(glGetUniformLocation(animShader->Program, "light3.ambient"), light1.ambient[0], light1.ambient[1], light1.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light3.diffuse"), light1.diffuse[0], light1.diffuse[1], light1.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "light3.specular"), light1.specular[0], light1.specular[1], light1.specular[2]);
	glUniform1f(glGetUniformLocation(animShader->Program, "light3.constant"), light1.constant);
	glUniform1f(glGetUniformLocation(animShader->Program, "light3.linear"), light1.linear);
	glUniform1f(glGetUniformLocation(animShader->Program, "light3.quadratic"), light1.quadratic);
	glUniform3f(glGetUniformLocation(animShader->Program, "light3.position"), light.position[0] + 1.9f, light.position[1], light.position[2] - 3.15f);
	glUniform3f(glGetUniformLocation(animShader->Program, "light3.direction"), light.direction[0], light.direction[1], light.direction[2]);
	glUniform1f(glGetUniformLocation(animShader->Program, "light3.cutOff"), light1.cutOff);
	glUniform1f(glGetUniformLocation(animShader->Program, "light3.outerCutOff"), light1.outerCutOff);
	

	glUniform3f(glGetUniformLocation(animShader->Program, "dirLight.direction"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "dirLight.ambient"), dirLight.ambient[0], dirLight.ambient[1], dirLight.ambient[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "dirLight.diffuse"), dirLight.diffuse[0], dirLight.diffuse[1], dirLight.diffuse[2]);
	glUniform3f(glGetUniformLocation(animShader->Program, "dirLight.specular"), dirLight.specular[0], dirLight.specular[1], dirLight.specular[2]);


}
void setModel(glm::mat4 ModelMatrix, GLuint shader)
{
	glm::mat4 MVP = ProjectionMatrix * camera.GetViewMatrix() * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(shader, "M"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
}

int initglfw()
{

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1200, 700, "StillDer", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);


	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallBack);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);


	// grey background
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

}

void initModels()
{
	ground = new Model("res/Props/Map/environment.obj");

	roofs = new Model("res/Props/Map/roofs.obj");

	props = new Model("res/Props/Map/props.obj");

	lamps = new Model("res/Props/Map/lamps.obj");

	gun = new Model("res/Props/Gun/gun.obj");

	muzzleFlash = new Model("res/Props/Gun/muzzleFlash.obj");

	bullet = new Model("res/Props/Gun/bullet.obj");


	//Leslie.loadModel("res/characters/lez.dae");
	//Leslie.initShaders(animShader->Program);

	Sassy.loadModel("res/characters/sassy/sassy.dae");
	Sassy.initShaders(animShader->Program);
	Sassy.setAnim(0);

	//Weapon.loadModel("res/Props/Gun/gun.dae");
	//Weapon.initShaders(animShader->Program);
	//Weapon.setAnim(0);

	//Choomah.loadModel("res/characters/choomah/Choomah.dae");
	//Choomah.initShaders(animShader->Program);


}

int initFrameBuffer()
{
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	Framebuffer = 0;
	glGenFramebuffers(1, &Framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

	// Depth texture
	depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

}

void setLights(Shader* shader)
{
	// Use our shader
	glUseProgram(shader->Program);

	//set the directional light 
	glUniform3f(glGetUniformLocation(shader->Program, "dirLight.direction"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "dirLight.ambient"), dirLight.ambient[0], dirLight.ambient[1], dirLight.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "dirLight.diffuse"), dirLight.diffuse[0], dirLight.diffuse[1], dirLight.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "dirLight.specular"), dirLight.specular[0], dirLight.specular[1], dirLight.specular[2]);

	//set camera position
	GLint viewPosLoc = glGetUniformLocation(shader->Program, "viewPos");
	glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

	//spotlight
	glUniform3f(glGetUniformLocation(shader->Program, "light.ambient"), light.ambient[0], light.ambient[1], light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light.diffuse"), light.diffuse[0], light.diffuse[1], light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light.specular"), light.specular[0], light.specular[1], light.specular[2]);
	glUniform1f(glGetUniformLocation(shader->Program, "light.constant"), light.constant);
	glUniform1f(glGetUniformLocation(shader->Program, "light.linear"), light.linear);
	glUniform1f(glGetUniformLocation(shader->Program, "light.quadratic"), light.quadratic);
	glUniform3f(glGetUniformLocation(shader->Program, "light.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
	glUniform3f(glGetUniformLocation(shader->Program, "light.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
	glUniform1f(glGetUniformLocation(shader->Program, "light.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(shader->Program, "light.outerCutOff"), light.outerCutOff);


	//spotlight
	glUniform3f(glGetUniformLocation(shader->Program, "light1.ambient"), light.ambient[0], light.ambient[1], light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light1.diffuse"), light.diffuse[0], light.diffuse[1], light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light1.specular"), light.specular[0], light.specular[1], light.specular[2]);
	glUniform1f(glGetUniformLocation(shader->Program, "light1.constant"), light.constant);
	glUniform1f(glGetUniformLocation(shader->Program, "light1.linear"), light.linear);
	glUniform1f(glGetUniformLocation(shader->Program, "light1.quadratic"), light.quadratic);
	glUniform3f(glGetUniformLocation(shader->Program, "light1.position"), light.position[0], light.position[1], light.position[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light1.direction"), light.direction[0], light.direction[1], light.direction[2]);
	glUniform1f(glGetUniformLocation(shader->Program, "light1.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(shader->Program, "light1.outerCutOff"), light.outerCutOff);

	//spotlight
	glUniform3f(glGetUniformLocation(shader->Program, "light2.ambient"), light.ambient[0], light.ambient[1], light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light2.diffuse"), light.diffuse[0], light.diffuse[1], light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light2.specular"), light.specular[0], light.specular[1], light.specular[2]);
	glUniform1f(glGetUniformLocation(shader->Program, "light2.constant"), light.constant);
	glUniform1f(glGetUniformLocation(shader->Program, "light2.linear"), light.linear);
	glUniform1f(glGetUniformLocation(shader->Program, "light2.quadratic"), light.quadratic);
	glUniform3f(glGetUniformLocation(shader->Program, "light2.position"), light.position[0] + 3.8f, light.position[1], light.position[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light2.direction"), light.direction[0], light.direction[1], light.direction[2]);
	glUniform1f(glGetUniformLocation(shader->Program, "light2.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(shader->Program, "light2.outerCutOff"), light.outerCutOff);
	
	//spotlight
	glUniform3f(glGetUniformLocation(shader->Program, "light3.ambient"), light.ambient[0], light.ambient[1], light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light3.diffuse"), light.diffuse[0], light.diffuse[1], light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader->Program, "light3.specular"), light.specular[0], light.specular[1], light.specular[2]);
	glUniform1f(glGetUniformLocation(shader->Program, "light3.constant"), light.constant);
	glUniform1f(glGetUniformLocation(shader->Program, "light3.linear"), light.linear);
	glUniform1f(glGetUniformLocation(shader->Program, "light3.quadratic"), light.quadratic);
	glUniform3f(glGetUniformLocation(shader->Program, "light3.position"), light.position[0] + 1.8f, light.position[1], light.position[2] - 3.2f);
	glUniform3f(glGetUniformLocation(shader->Program, "light3.direction"), light.direction[0], light.direction[1], light.direction[2]);
	glUniform1f(glGetUniformLocation(shader->Program, "light3.cutOff"), light.cutOff);
	glUniform1f(glGetUniformLocation(shader->Program, "light3.outerCutOff"), light.outerCutOff);

	glUniform1i(glGetUniformLocation(shader->Program, "normalMapped"), normalMapped);
}

void setContext(bool attach, int windowWidth, int windowHeight)
{
	//decide whether to draw to the framebuffer or not
	if(attach == true)
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
	else
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, windowWidth, windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void speechGenerator()
{
	//RNG
	int selector = (rand() % 7) + 1;

	//play a random audio clip
	if (selector == 0) SoundEngine->play2D("res/Sounds/LezLine1.wav", GL_FALSE);
	if (selector == 1) SoundEngine->play2D("res/Sounds/LezLine2.wav", GL_FALSE);
	if (selector == 2) SoundEngine->play2D("res/Sounds/LezLine3.wav", GL_FALSE);
	if (selector == 3) SoundEngine->play2D("res/Sounds/SassyLine1.wav", GL_FALSE);
	if (selector == 4) SoundEngine->play2D("res/Sounds/SassyLine2.wav", GL_FALSE);
	if (selector == 5) SoundEngine->play2D("res/Sounds/LezSassy1.wav", GL_FALSE);
	if (selector == 6) SoundEngine->play2D("res/Sounds/LezSassy2.wav", GL_FALSE);
	if (selector == 7) SoundEngine->play2D("res/Sounds/LezSassy3.wav", GL_FALSE);
}
int main(int argc, char *argv[])
{

	//create a window and initialise GLFW and glew
	initglfw();

	animShader = new Shader("res/shaders/AnimFog.vert", "res/shaders/AnimFog.frag");
	shader = new Shader("res/shaders/fogMap.vert", "res/shaders/fogMap.frag");
	UIShader = new Shader("res/shaders/skyboxShader.vert", "res/shaders/skyboxShader.frag");

	//load models from files
	initModels();

	// initialise the shaders
	shadowShader = new Shader("res/shaders/DepthRTT.vertexshader", "res/shaders/DepthRTT.fragmentshader");
	//generate a frame buffer for the shadow map
	initFrameBuffer();


	SkyBox::Instance()->init("res/FogMap");

	//randomise
	srand(time(NULL));


	do {
		cout << gameStart << endl;
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//if the game has begun
		if(gameStart == true){

			if (musicStarted == false) {
				//start the ambient background music
				AmbientEngine->setSoundVolume(0.1f);
				AmbientEngine->play2D("res/Sounds/AmbientMusic.wav", GL_TRUE);

				musicStarted = true;
			}


			// speech timer
			timer -= deltaTime;
			if (timer <= 0.f)
			{
				speechGenerator();
				timer += 90.0f;
			}

			//sassies animation timer
			sassytimer -= deltaTime;
			if (sassytimer <= 0.f)
			{
				SassyAnim = true;
				sassyInAnim += 0.01f;
			}

			if (sassyInAnim >= sassyAnimLength)
			{
				sassyInAnim = 0.0f;
				sassytimer += 30.0f;
				SassyAnim = false;
			}

			//round animation timer
			roundtimer -= deltaTime;
			if (roundtimer <= 0.f)
			{
				alphaBool = true;
			}

			if (alphaBool == true)
			{
				if (alpha > 0)
				{
					alpha -= 0.01;
				}
				else
				{
					alphaBool = false;
					roundtimer = timer + FLT_MAX;
				}
			}
			

			//set up framebuffer context
			setContext(true, 1024, 1024);

			// Use the shadow shader
			glUseProgram(shadowShader->Program);

			// Compute the MVP matrix from the light's point of view
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
			glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::mat4 depthModelMatrix = glm::mat4(1.0);

			msStack.push(depthModelMatrix);
			msStack.top() = glm::scale(msStack.top(), glm::vec3(0.2,0.2,0.2));
			msStack.top() = glm::translate(msStack.top(), glm::vec3(0.0f, 5.5f, 0.0f));
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * msStack.top();
			glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "depthMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));

			//draw the shadow(vertices and indices only)
			ground->DrawVMesh(*shadowShader);
			props->DrawVMesh(*shadowShader);
			msStack.pop();

			//disable the framebuffer
			setContext(false, windowWidth, windowHeight);

			//set up all the  lights
			setLights(shader);

			// Compute the MVP matrix from keyboard and mouse input
			ProjectionMatrix = glm::perspective(camera.GetZoom(), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f); ;// getProjectionMatrix();
			ViewMatrix = camera.GetViewMatrix();
			ModelMatrix = glm::mat4(1.0);

			mStack.push(ModelMatrix);
			mStack.top() = glm::scale(mStack.top(), glm::vec3(0.2f, 0.2f, 0.2f));
			mStack.top() = glm::translate(mStack.top(), glm::vec3(0.0f, 5.5f, 0.0f));
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * mStack.top();
			glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
			mStack.pop();

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(glGetUniformLocation(shader->Program, "MVP"), 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader->Program, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader->Program, "V"), 1, GL_FALSE, &ViewMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader->Program, "DepthBiasMVP"), 1, GL_FALSE, &depthBiasMVP[0][0]);
			glUniform3f(glGetUniformLocation(shader->Program, "LightInvDirection_worldspace"), dirLight.direction[0], dirLight.direction[1], dirLight.direction[2]);


			//set fog values based on input
			glUniform1i(glGetUniformLocation(shader->Program, "fogSelector"), fogSelector);
			glUniform1i(glGetUniformLocation(shader->Program, "depthFog"), depthFog);

			//set the shadow to the highest possible texture (to avoid overwriting by the model class)
			glActiveTexture(GL_TEXTURE31);
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			glUniform1i(glGetUniformLocation(shader->Program, "shadowMap"), 31);

			//ground
			mStack.push(ModelMatrix);
			mStack.top() = glm::scale(mStack.top(), glm::vec3(0.2f, 0.2f, 0.2f));
			mStack.top() = glm::translate(mStack.top(), glm::vec3(0.0f, 5.5f, 0.0f));
			setModel(mStack.top(), shader->Program);
			ground->DrawDMesh(*shader);
			roofs->DrawDMesh(*shader);
			props->DrawDMesh(*shader);
			lamps->DrawDMesh(*shader);
			mStack.pop();

			mStack.push(ModelMatrix);
			glm::mat4 axis_offset_mat = glm::translate(mStack.top(), glm::vec3(2.6f, 0.6f, 8.5f));
			//mStack.top() = glm::scale(mStack.top(), glm::vec3(0.105f, 0.105f, 0.105f));
			glm::mat4 rotation_mat = glm::rotate(mStack.top(), glm::radians(testervalue), glm::vec3(1.0f, 0.0f, 0.0f));
			//mStack.top() = glm::translate(mStack.top(), glm::vec3(-2.6f, -0.6f, -8.5f));
			//mStack.top() = glm::translate(mStack.top(), glm::vec3(4.75f, -0.5f, 3.0f));
			mStack.top() = ModelMatrix * axis_offset_mat * rotation_mat;
			setModel(mStack.top(), shader->Program);
			//gun->DrawDMesh(*shader);
			mStack.pop();

			//animated model part
			animShader->Use();
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "V"), 1, GL_FALSE, &ViewMatrix[0][0]);
			setMaterial();
			setAnimLights();

			mStack.push(ModelMatrix);
			mStack.top() = glm::scale(mStack.top(), glm::vec3(0.105f, 0.105f, 0.105f));
			//mStack.top() = glm::translate(mStack.top(), glm::vec3(4.75f, -0.5f, 3.0f));
			mStack.top() = glm::rotate(mStack.top(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "M_matrix"), 1, GL_FALSE, glm::value_ptr(mStack.top()));
			glm::mat4 matr_normals = glm::mat4(glm::transpose(glm::inverse(mStack.top())));
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals));
			MVP = ProjectionMatrix * camera.GetViewMatrix() * mStack.top();
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));


			Sassy.draw(animShader->Program, SassyAnim);
			//Weapon.draw(animShader->Program, SassyAnim);

			mStack.push(ModelMatrix);
			mStack.top() = glm::translate(mStack.top(), glm::vec3(0.0, 1.0, -5.0));
			//mStack.top() = glm::translate(mStack.top(), glm::vec3(camera.lez.lezX, camera.lez.lezY, camera.lez.lezZ));
			mStack.top() = glm::scale(mStack.top(), glm::vec3(0.1f, 0.1f, 0.1f));
			mStack.top() = glm::rotate(mStack.top(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			//mStack.top() = glm::rotate(mStack.top(), glm::radians(-(camera.GetYaw()) + 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "M_matrix"), 1, GL_FALSE, glm::value_ptr(mStack.top()));
			matr_normals = glm::mat4(glm::transpose(glm::inverse(mStack.top())));
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals));
			MVP = ProjectionMatrix * camera.GetViewMatrix() * mStack.top();
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

			//Leslie.draw(animShader->Program, LeslieAnim);

			mStack.pop();

			shader->Use();

			glm::vec3 rayTrace = camera.GetFront();
			//draw the bullet and shoot in the direction of the camera front


			for (vector<Model>::iterator it_obj1 = collisionObjs.begin(); it_obj1 < collisionObjs.end(); it_obj1++) // cycle through all objects
			{
				mStack.push(ModelMatrix);
				mStack.top() = glm::translate(mStack.top(), glm::vec3(camera.GetPosition().x, camera.GetPosition().y - 0.031, camera.GetPosition().z - testervalue));
				mStack.top() = glm::scale(mStack.top(), glm::vec3(0.2f, 0.2f, 0.2f));
				setModel(mStack.top(), shader->Program);
				it_obj1->DrawDMesh(*shader);

				mStack.pop();
			}

			//if player is firing
			if (fired == true)
			{
				//to check button isnt being held down
				if (bulletRemoved == false)
				{
					// and if the player has ammo
					if (ammo > 0)
					{	//reduce ammo once and then stop any more from coming off per click
						ammo--;
						//raytrace front at this point 
						 rayTrace = camera.GetFront();
						 mStack.push(ModelMatrix);
						 //mStack.top() = glm::scale(mStack.top(), glm::vec3(0.2f, 0.2f, 0.2f));
						 //mStack.top() * vec4(rayTrace, 1);// = glm::rotate(mStack.top(), , rayTrace);
						 mStack.top() = glm::translate(mStack.top(), glm::vec3(camera.GetPosition()));
						 mStack.top() = glm::scale(mStack.top(), glm::vec3(0.2f, 0.2f, 0.2f));
						 setModel(mStack.top(), shader->Program);
						 bullet->DrawDMesh(*shader);
						 collisionObjs.push_back(*bullet);
						 mStack.pop();
						//draw the bullet and shoot in the direction of the camera front


						bulletRemoved = true;

						//draw the muzzle flash for a length of time
						muzzleTimer -= deltaTime;
						if (muzzleTimer >= 0.f)
						{
							muzzleFlash->Draw(*shader);

						}
						//and play the noise of the gunshot
						SoundEngine->play2D("res/Sounds/gun.wav", GL_FALSE);
					}
				}
			}
			else
			{
				bulletRemoved = false;
				//reset timer
				if (secondMuzzleTimer <= 0.0f)
					muzzleTimer += 0.15f;
				else
					secondMuzzleTimer -= deltaTime;
			}

			if (ammo == 0)
			{
				reloadTimer -= deltaTime;
				if (reloadTimer <= 0.f)
				{
					ammo = 6;
					reloadTimer += 3.0f;
				}
			}

			animShader->Use();

			mStack.push(ModelMatrix);
			mStack.top() = glm::translate(mStack.top(), glm::vec3(10.75f, -0.5f, -13.0f + testervalue));
			mStack.top() = glm::scale(mStack.top(), glm::vec3(0.105f, 0.105f, 0.105f));
			mStack.top() = glm::rotate(mStack.top(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "M_matrix"), 1, GL_FALSE, glm::value_ptr(mStack.top()));
			matr_normals = glm::mat4(glm::transpose(glm::inverse(mStack.top())));
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals));
			MVP = ProjectionMatrix * camera.GetViewMatrix() * mStack.top();
			glUniformMatrix4fv(glGetUniformLocation(animShader->Program, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

			//Choomah.draw(animShader->Program, true);

			shader->Use();

			//draw the skybox
			SkyBox::Instance()->draw();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthFunc(GL_ALWAYS);

			//draw the UI
			glm::mat4 translate_2d_text = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
			glm::mat4 scale_2d_text = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
			TextRendering::Instance()->draw("BIG LEZ", glm::vec3(1.0f, 0.75f, 0.79f), 1.0f, text_matrix_2D * translate_2d_text * scale_2d_text);
			translate_2d_text = glm::translate(glm::mat4(1.0), glm::vec3(925.0f, 0.0f, 0.0f));
			TextRendering::Instance()->draw(std::to_string(ammo), glm::vec3(1.0f, 0.75f, 0.79f), 1.0f, text_matrix_2D * translate_2d_text * scale_2d_text);
			translate_2d_text = glm::translate(glm::mat4(1.0), glm::vec3(350.0f, 710.0f, 0.0f));
			TextRendering::Instance()->draw("ROUND : ", glm::vec3(1.0f, 0.75f, 0.79f), 0.0f, text_matrix_2D * translate_2d_text * scale_2d_text);
			translate_2d_text = glm::translate(glm::mat4(1.0), glm::vec3(512.0f, 768.0f / 2, 0.0f));
			TextRendering::Instance()->draw(".", glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, text_matrix_2D * translate_2d_text * scale_2d_text);



			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);



			//remove rotation from matrix and apply to the skybox
			glm::mat4 pv = ProjectionMatrix * glm::mat4(glm::mat3(camera.GetViewMatrix()));
			SkyBox::Instance()->update(pv);


		}
		//swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		update();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup shader
	glDeleteProgram(shader->Program);
	glDeleteProgram(shadowShader->Program);

	// Cleanup framebuffer
	glDeleteFramebuffers(1, &Framebuffer);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (gameStart == true) {
		if (firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		if (fired == true)
		{

		}

		GLfloat xOffset = xPos - lastX;
		GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

		lastX = xPos;
		lastY = yPos;

		camera.ProcessMouseMovement(xOffset, yOffset);

		if (camera.getPitch() < 10.75 && camera.getPitch() > -89) {

			GLfloat oY = -(yOffset * 0.125);
			//Leslie.rotate_head_xz *= glm::quat(cos(glm::radians(oY)), sin(glm::radians(oY)) * glm::vec3(0.0f, 0.0f, 1.0f));

		}

	}


}
void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (fired == false)
		{
			fired = true;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		fired = false;

	}
}
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void update()
{

	if (keys[GLFW_KEY_1])
	{
		gameStart = true;
	}

	if (keys[GLFW_KEY_2])
	{
		//Leslie.setAnim(0);
	}
	if (keys[GLFW_KEY_3])
	{
		//Leslie.setAnim(1);
	}

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
		LeslieAnim = true;
	}
	else
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		LeslieAnim = true;
	}
	else
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
		LeslieAnim = true;
	}
	else
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
		LeslieAnim = true;
	}
	else {
		LeslieAnim = false;
	}


	if (keys[GLFW_KEY_H]) {
		testervalue = 0.5;
		Sassy.rotate_head_xz *= glm::quat(cos(glm::radians(testervalue)), sin(glm::radians(testervalue)) * glm::vec3(-1.0f, 0.0f, 0.0f));
	}
	if (keys[GLFW_KEY_J]) {
		testervalue = 0.5;
		Sassy.rotate_head_xz *= glm::quat(cos(glm::radians(testervalue)), sin(glm::radians(testervalue)) * glm::vec3(1.0f, 0.0f, 0.0f));
	}

	light.direction[0]  =  0.5;
	light.direction[1] = -10.0;
	light.direction[2] = -0.68;

}
