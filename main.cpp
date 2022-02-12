#include "imgui-master/imgui.h"
#include "imgui-master/backends/imgui_impl_opengl3.h"
#include "imgui-master/backends/imgui_impl_glfw.h"

#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include "Headers/Player.h"
#include "Headers/Bounds.h"
#include<vector>
#include<cstdlib>
#include<ctime>
#include<Windows.h>

#pragma comment(lib, "winmm.lib")

using namespace ImGui;
void processKeyboardInput();


const int timeOfDay = 3;//default:20

float LastTimeOfDay = 0.f;
float StartingTimeOfDay = 3.1415f / 2;
unsigned long long triangles = 0;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Window window("Game Engine", 1024, 800);
long Xpos = 0, Ypos = 0;
int mouseDirX = 0, mouseDirY = 0;
Camera camera(glm::vec3(-50.0f , 20.f, 100.f));
glm::vec3 lastCameraPosition = glm::vec3(100.f, 30.f, 0.f);

void updateMouse() {
	mouseDirX = window.getXpos() - Xpos;
	mouseDirY = Ypos-window.getYpos();
	Xpos = window.getXpos();
	Ypos = window.getYpos();
}
void mouseWheelCallback(GLFWwindow* window, double smth, double directionWheel) {
	camera.updateCameraZoom(directionWheel);
}

glm::vec3 lightColor = glm::vec3(1.f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

glm::vec3 skyColor[5] = {
	glm::vec3((float)133 / 255, (float)247 / 255, (float)255 / 255),
	glm::vec3((float)0 / 255, (float)152 / 255, (float)199 / 255),
	glm::vec3((float)0 / 255, (float)37 / 255, (float)79 / 255),
	glm::vec3((float)0 / 255, (float)94 / 255, (float)166 / 255),
	glm::vec3(1.f,1.f,1.f)
};

glm::vec3 lightColorArray[5] = {
	glm::vec3((float)230 / 255, (float)253 / 255, (float)255 / 255),
	glm::vec3((float)173 / 255, (float)236 / 255, (float)255 / 255),
	glm::vec3((float)47 / 255, (float)120 / 255, (float)204 / 255),
	glm::vec3((float)75 / 255, (float)157 / 255, (float)219 / 255),
	glm::vec3(1.f,1.f,1.f)
};

glm::vec3 skyCurrentColor = skyColor[0];
glm::vec3 lightCurrentColor = lightColorArray[0];
int skyPos = 0;
int lightPosColor = 0;


float clamp(const float& number,const float& left,const float& right) {
	if (left <= right) {
		if (number < left)return left;
		if (number > right)return right;
	}
	if (left > right) {
		if (number > left)return left;
		if (number < right)return right;
	}
	return number;
}

float playerSwordAngle = 0.f;
float playerShieldAngle = -45.f;
bool attacking = false;
float cooldown = 0.f;
bool blocking = false;

void dealDamage() {
	//back left down
	glm::vec3 beginZone = glm::vec3();

	//front up right 
	glm::vec3 endZone = glm::vec3();

}
void attackAnimation() {
	if ((glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		&& !attacking && (cooldown<=0.f) && !blocking) {
		attacking = true;
		cooldown = 0.75f;
		//deal damage function
		dealDamage();
	}
	
	if(attacking)playerSwordAngle -= deltaTime * 300.f;
	if (playerSwordAngle <= -45.f) attacking = false;

	if (cooldown >= 0.f && !attacking) {
		playerSwordAngle += deltaTime * 90.f;
		cooldown -= deltaTime;
	}

	playerSwordAngle = clamp(playerSwordAngle, -45.f, 0.f);
}
void blockingAnimation() {
	if (glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		playerShieldAngle += deltaTime * 200.f;
		blocking = true;
	}
	else {
		playerShieldAngle -= deltaTime * 100.f;
		blocking = false;
	}
	playerShieldAngle = clamp(playerShieldAngle, -45.f, 15.f);
}
void GUIrender() {
	//initialize
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//first
	ImGui::Begin("Details");
	ImGui::Text(("Triangles drawn on screen:" + std::to_string(triangles)).c_str());
	ImGui::Text(("Screen size: " + std::to_string(window.getHeight())+"x"+ std::to_string(window.getHeight())).c_str());
	ImGui::Text(("FPS:" + std::to_string(1.f/deltaTime)).c_str());
	int hour=0.f;
	hour = (sin((glfwGetTime() - LastTimeOfDay) / timeOfDay + StartingTimeOfDay)+1)/2 * 24;
	std::string str = "Time:" + std::to_string(hour);
	ImGui::Text(str.c_str());
	if (ImGui::Button("Day")) {
		LastTimeOfDay = glfwGetTime();
		StartingTimeOfDay = -3.1415f/2.0f;
		skyPos = 2;
		skyCurrentColor = skyColor[2];
		lightPosColor = 2;
		lightCurrentColor = lightColorArray[2];
	}
	if (ImGui::Button("Night")) {
		LastTimeOfDay = glfwGetTime();
		StartingTimeOfDay = 3.1415f/2.0f;
		skyPos = 0;
		skyCurrentColor = skyColor[0];
		lightPosColor = 0;
		lightCurrentColor = lightColorArray[0];
	}

	ImGui::End();


	//second

	/*ImGui::Begin("Time");
	ImGui::Text("Hello There! 222222222222222");
	ImGui::End();*/

	//end
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void interpolate(glm::vec3 val1, glm::vec3 val2, glm::vec3& inter,float step) {
	float stepX = (val2.x - val1.x) / step;
	float stepY = (val2.y - val1.y) / step;
	float stepZ = (val2.z - val1.z) / step;
	inter.x += stepX;
	inter.y += stepY;
	inter.z += stepZ;
	inter.x=clamp(inter.x, val1.x, val2.x);
	inter.y=clamp(inter.y, val1.y, val2.y);
	inter.z=clamp(inter.z, val1.z, val2.z);
}

void skyboxLogic() {
	if (skyCurrentColor==skyColor[skyPos + 1]) ++skyPos;
	if (skyPos == 4)skyPos = 0;

	int const speedOfDay = 330'00;

	interpolate(skyColor[skyPos], skyColor[skyPos+1], skyCurrentColor, speedOfDay * timeOfDay * deltaTime);
	glClearColor(skyCurrentColor.x, skyCurrentColor.y, skyCurrentColor.z, 1.0f);


	if (lightColor == lightColorArray[lightPosColor + 1]) ++lightPosColor;
	if (lightPosColor == 4)lightPosColor = 0;

	interpolate(lightColorArray[lightPosColor], lightColorArray[lightPosColor + 1], lightColor, speedOfDay * timeOfDay * deltaTime);
	glClearColor(lightColor.x, lightColor.y, lightColor.z, 1.0f);
}


float area(float x1, float y1, float x2, float y2, float x3, float y3)
{/* A utility function to calculate area of triangle formed by (x1, y1),
   (x2, y2) and (x3, y3) */
	return fabs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}
bool isInside(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y)
{
	/* A function to check whether point P(x, y) lies inside the triangle formed
   by A(x1, y1), B(x2, y2) and C(x3, y3) */

	/* Calculate area of triangle ABC */
	double A = area(x1, y1, x2, y2, x3, y3);

	/* Calculate area of triangle PBC */
	double A1 = area(x, y, x2, y2, x3, y3);

	/* Calculate area of triangle PAC */
	double A2 = area(x1, y1, x, y, x3, y3);

	/* Calculate area of triangle PAB */
	double A3 = area(x1, y1, x2, y2, x, y);

	/* Check if sum of A1, A2 and A3 is same as A */
	return fabs(A - (A1 + A2 + A3)) < 0.01;
}
float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float posX,float posZ) {
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (posX - p3.x) + (p3.x - p2.x) * (posZ - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (posX - p3.x) + (p1.x - p3.x) * (posZ- p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}



//Shaders Initialize----------------------------------------------
Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
Shader mountainShader("Shaders/vertex_shader.glsl", "Shaders/mountain_fragment_shader.glsl");
Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
Shader waterShader("Shaders/water_vertex_shader.glsl", "Shaders/fragment_shader.glsl");


Mesh terrainMesh;
const float terrainScale = 1.f;
class Tree {
public:
	glm::mat4 ModelMatrix;
	BoundingRegion boundingRegionTree;
	BoundingRegion boundingRegionLeaves;
	Tree(glm::vec3 position,glm::vec3 size,float rotation,BoundingRegion brTree, BoundingRegion brLeaves) {
		ModelMatrix = glm::translate(glm::mat4(1.f), position);
		ModelMatrix = glm::scale(ModelMatrix, size);
		ModelMatrix = glm::rotate(ModelMatrix, rotation,glm::vec3(0.f,1.f,0.f));
		boundingRegionTree = brTree;
		boundingRegionTree.max = brTree.max * size + position;
		boundingRegionTree.min = brTree.min * size + position;
		boundingRegionLeaves = brLeaves;
		boundingRegionLeaves.center =brLeaves.center*size+position;
		boundingRegionLeaves.radius *= size.x/2.0f;

	}
};
int main()
{	
	PlaySound("env2Song.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
	
	skyColor[4] = skyColor[0];
	lightColorArray[4] = lightColorArray[0];
	glClearColor(skyCurrentColor.x, skyCurrentColor.y, skyCurrentColor.z, 1.0f);

	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO(); (void)io;
	StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 400");

	//Textures Initialize ------------------------------------------------------
	GLuint texWood = loadBMP("Resources/Textures/wood.bmp");
	GLuint texRock = loadBMP("Resources/Textures/rock.bmp");
	GLuint texOcean = loadBMP("Resources/Textures/ocean.bmp");
	GLuint texLeaves = loadBMP("Resources/Textures/leaves.bmp");
	GLuint texAmong = loadBMP("Resources/Textures/Among-Us.bmp");

	glEnable(GL_DEPTH_TEST);

	std::vector<Texture> textureWood;
	textureWood.push_back(Texture());
	textureWood[0].id = texWood;
	textureWood[0].type = "texture_diffuse";

	std::vector<Texture> textureRock;
	textureRock.push_back(Texture());
	textureRock[0].id = texRock;
	textureRock[0].type = "texture_diffuse";

	std::vector<Texture> textureOcean;
	textureOcean.push_back(Texture());
	textureOcean[0].id = texOcean;
	textureOcean[0].type = "texture_diffuse";

	std::vector<Texture> textureLeaves;
	textureLeaves.push_back(Texture());
	textureLeaves[0].id = texLeaves;
	textureLeaves[0].type = "texture_diffuse";

	std::vector<Texture> textureAmong;
	textureAmong.push_back(Texture());
	textureAmong[0].id = texAmong;
	textureAmong[0].type = "texture_diffuse";




	MeshLoaderObj loader;

	Mesh sun = loader.loadObj("Resources/Models/sphere.obj", triangles);

	//set player dynamic---------------------------------------
	Mesh playerSwordMesh = loader.loadObj("Resources/Models/PlayerSword.obj", textureWood, triangles);
	Mesh playerShieldMesh = loader.loadObj("Resources/Models/shield.obj", textureWood, triangles);
	Mesh waterMesh = loader.loadObj("Resources/Models/water6.obj", textureOcean, triangles);//waterMesh is 80 width
	terrainMesh = loader.loadObj("Resources/Models/terrainNew.obj", textureRock, triangles);
	Mesh lakeText = loader.loadObj("Resources/Models/dontlookText.obj", triangles);
	Mesh susText = loader.loadObj("Resources/Models/susText.obj", triangles);
	Mesh amongUsMesh = loader.loadObj("Resources/Models/Among Us.obj",textureAmong, triangles);

	Mesh leavesMesh = loader.loadObj("Resources/Models/leavesIncomplete.obj", textureLeaves, triangles,BoundTypes::SPHERE);
	Mesh treeMesh= loader.loadObj("Resources/Models/treeIncomplete.obj", textureWood, triangles);

	std::vector<Tree> treeVector;
	//generating the treeVector 
	const unsigned int numberOfTrees=100;

	for (unsigned int i = 0; i < numberOfTrees; ++i) {
		float ranX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*480 -240;
		float ranZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 480 - 240;
		while ((ranX >= -40.0f && ranX <= 150.0f) && (ranZ >= 10 && ranZ <= 180.0f)) {
			ranX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 480 - 240;
			ranZ = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 480 - 240;
		}

		float heigthMax = 0.0f;
		for (int j = 0, l = terrainMesh.indices.size(); j < l; j += 3) {
			glm::vec3 point1 = terrainMesh.vertices.at(terrainMesh.indices.at(j)).pos * terrainScale;
			glm::vec3 point2 = terrainMesh.vertices.at(terrainMesh.indices.at(j + 1)).pos * terrainScale;
			glm::vec3 point3 = terrainMesh.vertices.at(terrainMesh.indices.at(j + 2)).pos * terrainScale;

			if (isInside(point1.x, point1.z, point2.x, point2.z, point3.x, point3.z,
				ranX, ranZ)) {
				float heigthTemp = barryCentric(point1, point2, point3,
					ranX, ranZ);
				if (heigthTemp > heigthMax) {
					heigthMax = heigthTemp;
				}
			}
		}
		treeVector.push_back(Tree(glm::vec3(ranX, heigthMax, ranZ), glm::vec3(4.0f), ranX+ranZ, treeMesh.region,leavesMesh.region));
	}

	
	bool playSong = false;
	glfwSetScrollCallback(window.getWindow(), mouseWheelCallback);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{

		if (glfwGetKey(window.getWindow(), GLFW_KEY_K) == GLFW_PRESS && !playSong) {
			playSong = true;
			PlaySound("amongUsSong.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
		}
		glEnable(GL_DEPTH_TEST);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		skyboxLogic();

		window.clear();
		if (!window.activeCursor)
			glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		updateMouse();
		processKeyboardInput();

		//Sun--------------------------------------------------------------

		sunShader.use();

		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = glm::mat4(1.0f);

		ModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(35.0f, -20.f, 15.f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.f));
		ModelMatrix = glm::rotate(ModelMatrix, 90.f, glm::vec3(1.f, 0.f, 0.f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(sunShader.getId(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(sunShader.getId(), "lightColor"), 1.f, 1.f, 1.f);
		lakeText.draw(sunShader);
		ModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(35.0f, -30.f, 130.f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.f));
		ModelMatrix = glm::rotate(ModelMatrix, 90.f, glm::vec3(1.f, 0.f, 0.f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(sunShader.getId(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(sunShader.getId(), "lightColor"), 1.f, 1.f, 1.f);
		susText.draw(sunShader);

		ModelMatrix = glm::translate(glm::mat4(1.0f), lightPos);
		//for sun rotation
		lightPos.x = sin((glfwGetTime() - LastTimeOfDay) / timeOfDay + StartingTimeOfDay) * 380.f;
		lightPos.y = cos((glfwGetTime() - LastTimeOfDay) / timeOfDay + StartingTimeOfDay) * 400.f;

		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(sunShader.getId(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(sunShader.getId(), "lightColor"), lightColor.x + 0.2f, lightColor.y + 0.2f, lightColor.z + 0.2f);
		//glUniform3f(glGetUniformLocation(sunShader.getId(), "lightColor"), 1.f, 1.f, 1.f);
		glm::vec3 lightCurrentPos = glm::vec3(lightPos.x, lightPos.y, lightPos.z);

		sun.draw(sunShader);

		//Water -------------------------------------------------

		waterShader.use();
		glUniformMatrix4fv(glGetUniformLocation(waterShader.getId(), "Projection"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(waterShader.getId(), "ViewMatrix"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniform3f(glGetUniformLocation(waterShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(waterShader.getId(), "lightPos"), lightCurrentPos.x, lightCurrentPos.y, lightCurrentPos.z);
		glUniform3f(glGetUniformLocation(waterShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		ModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(38.0f, -0.05f, 95.0f));//38
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.03f));
		glUniformMatrix4fv(glGetUniformLocation(waterShader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform1f(glGetUniformLocation(waterShader.getId(), "offset"), glfwGetTime());
		waterMesh.draw(waterShader);

		mountainShader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Projection"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "ViewMatrix"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightCurrentPos.x, lightCurrentPos.y, lightCurrentPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		
		//Terrain draw----------------------------------------------------
		ModelMatrix = glm::scale(glm::mat4(1.f), glm::vec3(terrainScale));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		terrainMesh.draw(mountainShader);

		//Using shader to draw
		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Projection"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "ViewMatrix"), 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightCurrentPos.x, lightCurrentPos.y, lightCurrentPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//SwordDrawing-------------------------------------
		ModelMatrix = glm::translate(glm::mat4(1.f), camera.getCameraPosition() - glm::vec3(0.f, 0.5f, 0.f) + camera.getCameraViewDirection() * 2.f + camera.getCameraRight() * 2.f);
		ModelMatrix = glm::rotate(ModelMatrix, camera.getYaw() + 90.f, glm::vec3(0.f, -1.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, camera.getPitch(), glm::vec3(1.f, 0.f, 0.f));
		attackAnimation();
		ModelMatrix = glm::rotate(ModelMatrix, playerSwordAngle, glm::vec3(1.f, 0.f, 0.f));

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		playerSwordMesh.draw(shader);

		//ShieldDrawing------------------------------------
		ModelMatrix = glm::translate(glm::mat4(1.f), camera.getCameraPosition() - glm::vec3(0.f, 0.5f, 0.f) + camera.getCameraViewDirection() * 2.f - camera.getCameraRight() * 2.f);
		ModelMatrix = glm::rotate(ModelMatrix, camera.getYaw() + 90.f, glm::vec3(0.f, -1.f, 0.f));
		ModelMatrix = glm::rotate(ModelMatrix, camera.getPitch(), glm::vec3(1.f, 0.f, 0.f));
		blockingAnimation();
		ModelMatrix = glm::rotate(ModelMatrix, playerShieldAngle, glm::vec3(1.f, 0.f, 0.f));

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		playerShieldMesh.draw(shader);
		
		{
		ModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-10.f, -30.f, 150.f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(8.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		amongUsMesh.draw(shader);
		ModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(20.f, -30.f, 150.f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(8.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		amongUsMesh.draw(shader);
		ModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(50.f, -30.f, 150.f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(8.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		amongUsMesh.draw(shader);
		ModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(80.f, -30.f, 150.f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(8.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE, &ModelMatrix[0][0]);
		amongUsMesh.draw(shader);
		}
		
		for (int i = 0; i < numberOfTrees; ++i) {
			if (treeVector[i].boundingRegionTree.containsPoint(camera.cameraPosition)
				|| treeVector[i].boundingRegionLeaves.containsPoint(camera.cameraPosition)) {
				camera.cameraPosition = lastCameraPosition;
			}
			glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "Model"), 1, GL_FALSE,&treeVector[i].ModelMatrix[0][0]);
			treeMesh.draw(shader);
			leavesMesh.draw(shader);
		}
		GUIrender();
		window.update();
	}


	//cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

float gravity = -15.0f;
float heigth = 0.0f;
void processKeyboardInput()
{
	
	for (int i = 0, l = terrainMesh.indices.size(); i < l; i += 3) {
		glm::vec3 point1 = terrainMesh.vertices.at(terrainMesh.indices.at(i)).pos*terrainScale;
		glm::vec3 point2 = terrainMesh.vertices.at(terrainMesh.indices.at(i+1)).pos*terrainScale;
		glm::vec3 point3 = terrainMesh.vertices.at(terrainMesh.indices.at(i+2)).pos*terrainScale;

		if (isInside(point1.x, point1.z,point2.x,point2.z,point3.x, point3.z,
			camera.cameraPosition.x, camera.cameraPosition.z)) {

			float heigthTemp = barryCentric(point1, point2, point3,
				camera.cameraPosition.x, camera.cameraPosition.z);
			if (heigthTemp <= camera.cameraPosition.y //for not going over the head
				|| camera.cameraPosition.y<0) {//for the easter egg -can be removed
				heigth = heigthTemp;
			}
			//heigth = heigthTemp;
		}
	}
	

	if (window.isPressed(GLFW_KEY_SPACE) && camera.cameraPosition.y <= heigth + 6.2f) {
		gravity = 35.f;
		//PlaySound("fartSound.wav", NULL, SND_FILENAME | SND_ASYNC);
	}
	gravity = clamp(gravity-deltaTime*30, -35.f, 35.f);
	lastCameraPosition = camera.cameraPosition;
	camera.cameraPosition += glm::vec3(0.f, gravity * deltaTime, 0.f);
	camera.cameraPosition.y = clamp(camera.cameraPosition.y, heigth+6.f, 10000.f);


	

	//movement
	if (window.isPressed(GLFW_KEY_W))
		camera.updateCameraPos(CameraDirection::FRONT, deltaTime);
	if (window.isPressed(GLFW_KEY_S))
		camera.updateCameraPos(CameraDirection::BACK, deltaTime);
	if (window.isPressed(GLFW_KEY_D))
		camera.updateCameraPos(CameraDirection::RIGHT, deltaTime);
	if (window.isPressed(GLFW_KEY_A))
		camera.updateCameraPos(CameraDirection::LEFT, deltaTime);

	float sensitivity = 7.0f;

	//rotation
	if(mouseDirX!=0 || mouseDirY!=0)
	camera.updateCameraDirection(mouseDirX*deltaTime* sensitivity, mouseDirY*deltaTime* sensitivity);
}


