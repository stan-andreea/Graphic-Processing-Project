#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm typesF

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 fireflyMatrix;
glm::mat4 birdMatrix;


// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::mat4 lightRotation;



// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint firefly1Loc;
GLint vaseLoc;
GLint birdLoc;

GLint candle1Loc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

gps::Camera animationCamera(
    glm::vec3(0.0f, 10.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D scene;
gps::Model3D firefly1;
gps::Model3D firefly2;
gps::Model3D firefly3;
gps::Model3D firefly4;
gps::Model3D firefly5;
gps::Model3D trees1;
gps::Model3D trees2;
gps::Model3D trees3;
gps::Model3D trees4;
gps::Model3D vase;
gps::Model3D bird;

gps::Model3D candle1;

GLfloat angle;

gps::Model3D screenQuad;

// shaders
gps::Shader myBasicShader;

//skybox
std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

//shadow
GLuint shadowMapFBO;
GLuint depthMapTexture;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

GLfloat lightAngle;

bool showDepthMap;


int retina_width=1920, retina_height=1080;

float Yaw;
float Pitch;
// camera options
float MovementSpeed;
float MouseSensitivity;
float Zoom;

//preview animation
bool activateScenePreview = false;
float previewAngle = 0.0f;


//fog
int fog = 1;
GLint fogLoc;
GLfloat fogDensity = 0.05f;

//fireflies
glm::vec3 firefly1Pos;

//candles
glm::vec3 candle1Pos;

//variables for animation.
float treesAcceleration1;
float treesAcceleration2;
float treesAcceleration3=0.0f;
float treesAcceleration4=0.0f;
float treesSpeed1;
float treesSpeed2;
float treesSpeed3=0.0003f;
float treesSpeed4=-0.0003f;
float treesAngle1=0.03f;
float treesAngle2=-0.03f;
float treesAngle3=0.0f;
float treesAngle4=0.0f;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // line view
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // point view
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    // normal view
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        activateScenePreview = !activateScenePreview;

    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        showDepthMap = !showDepthMap;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        myCamera.viewPosition();
    }

}

float lastX = 512, lastY = 384;
bool firstMouse = true;
float yaw = 0.0f;
float pitch = 0;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (activateScenePreview) {
        return;
    }
    
    //TODO
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.08f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    myCamera.rotate(pitch, yaw);
}

float fov = 45.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
    myBasicShader.useShaderProgram();
    projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 100.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


}

int frame{ 0 };

//1.76475 1.22621 - 11.4299
//1.71593 1.18992 - 10.4317


// set camera position
// set camera target
// 
// if (frame < maxvalue)
// {
//  forward
// } else {
//  activatescene previ= false
// 
// }
// 
// 
// forword nuj cat 

void previewScene() {
    if (activateScenePreview == true) {
        if(frame <250)
            animationCamera.preview(previewAngle);
    
        if (frame >= 250) {
            if (frame == 250) {
                previewAngle = 0.0f;
                animationCamera.setPosition(glm::vec3(1.76475f, 2.0f, -11.4299f), glm::vec3(1.71593f, 2.0f, -10.4317f));

            }
            //animationCamera.setTarget(glm::vec3(1.71593f, 1.18992f, -10.4317f));

            if (frame < 550) {
                animationCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
                //update view matrix
                view = animationCamera.getViewMatrix();
                myBasicShader.useShaderProgram();
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
               
                normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            }
            else {
                activateScenePreview = false;
            }
        }
        else {
            previewAngle += 0.6f;
        }

        frame++;
    }
}

void processKeyboardMovement() {
    if (activateScenePreview) {
        return;
    }

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}
   
    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for scene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for scene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }
}

void initSkyBoxFace() {
    
    faces.push_back("models/skybox/Sky_Prelude07RT.tga"); //right
    faces.push_back("models/skybox/Sky_Prelude07LF.tga"); //left
    faces.push_back("models/skybox/Sky_Prelude07UP.tga"); //top
    faces.push_back("models/skybox/Sky_Prelude07DN.tga"); //bottom
    faces.push_back("models/skybox/Sky_Prelude07BK.tga"); //back
    faces.push_back("models/skybox/Sky_Prelude07FT.tga"); //front
    
}

void initSkyBox()
{
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    mySkyBox.Load(faces);
}

void initn9() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 0.5f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 1.0f, far_plane = 5.0f;
    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void initOpenGLWindow() {
    myWindow.Create(retina_width, retina_height, "OpenGL Project Core");
    
    glfwGetFramebufferSize(myWindow.getWindow(), &retina_width, &retina_height);
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetScrollCallback(myWindow.getWindow(), scroll_callback);
}

void initOpenGLState() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

    //transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    


    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initModels() {
    scene.LoadModel("models/cemetery.obj");
    screenQuad.LoadModel("models/quad.obj");
    firefly1.LoadModel("models/firefly1.obj");
    firefly2.LoadModel("models/firefly2.obj");
    firefly3.LoadModel("models/firefly3.obj");
    firefly4.LoadModel("models/firefly4.obj");
    firefly5.LoadModel("models/firefly5.obj");
    trees1.LoadModel("models/trees1.obj");
    trees2.LoadModel("models/trees2.obj");
    trees3.LoadModel("models/trees3.obj");
    trees4.LoadModel("models/trees4.obj");
    vase.LoadModel("models/vase.obj");
    bird.LoadModel("models/bird.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for scene
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = activateScenePreview ? animationCamera.getViewMatrix() : myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 100.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(17.4143f, 11.3404f, -1.04549f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(0.9f, 0.9f, 0.9f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //fog
    
    fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1f(fogLoc,fogDensity);

    //firefly
    firefly1Pos = glm::vec3(0.006643f, -2.37917f, 0.781286f);
    firefly1Loc = glGetUniformLocation(myBasicShader.shaderProgram, "firefly1Pos");
    glUniform3fv(firefly1Loc, 1, glm::value_ptr(firefly1Pos));
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "isFirefly"),0);

    //candles
    candle1Pos = glm::vec3(-0.467099f, -0.383682f, 1.18074f);
    candle1Loc = glGetUniformLocation(myBasicShader.shaderProgram, "candle1Pos");
    glUniform3fv(candle1Loc, 1, glm::value_ptr(candle1Pos));

    

}

void computeWind() {
    treesAcceleration1 = -treesAngle1*0.0002;
    treesSpeed1 += treesAcceleration1;
    treesAngle1 += treesSpeed1;

    treesAcceleration2 = -treesAngle2 * 0.0002;
    treesSpeed2 += treesAcceleration2;
    treesAngle2 += treesSpeed2;

    treesAcceleration3 = -treesAngle3 * 0.0002;
    treesSpeed3 += treesAcceleration3;
    treesAngle3 += treesSpeed3;

    treesAcceleration4 = -treesAngle4 * 0.0002;
    treesSpeed4 += treesAcceleration4;
    treesAngle4 += treesSpeed4;
}

float delta = 0;
int birdFrame = 0;

void renderObject(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    //send scene normal matrix data to shader
    if (!depthPass) {
        glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw scene
    scene.Draw(shader);

    computeWind();
    float shear[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    vase.Draw(shader);
  

    shear[4] = treesAngle1;
    memcpy(glm::value_ptr(model), shear, sizeof(shear));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    trees1.Draw(shader);

    shear[4] = treesAngle2;
    memcpy(glm::value_ptr(model), shear, sizeof(shear));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    trees2.Draw(shader);

    shear[4] = treesAngle3;
    memcpy(glm::value_ptr(model), shear, sizeof(shear));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    trees3.Draw(shader);

    shear[4] = treesAngle4;
    memcpy(glm::value_ptr(model), shear, sizeof(shear));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    trees4.Draw(shader);

    model = glm::mat4(1.0f);

   

    myBasicShader.useShaderProgram();

    // draw the bird
    
    delta += 0.01f;
    if (birdFrame == 1800) {
        birdMatrix = glm::mat4(1.0f);
        birdFrame = 0;
        delta = 0.0f;
        //birdMatrix = glm::translate(birdMatrix, glm::vec3(-20.0f, 0, 25.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(birdMatrix));
        //send normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        bird.Draw(myBasicShader);
    }
    else {
        birdMatrix = glm::mat4(1.0f);
        birdMatrix = glm::translate(birdMatrix, glm::vec3(delta, 0, delta * 2.5f));
        birdFrame++;
        //birdMatrix = glm::rotate(birdMatrix, 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        //glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(birdMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(birdMatrix));
        //send normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        bird.Draw(myBasicShader);
    }


   

    //depthMapShader.useShaderProgram();
    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fireflyMatrix));

    glm::mat4 modelFirefly = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelFirefly));

    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "isFirefly"), 1);
    
    firefly1.Draw(myBasicShader);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "isFirefly"), 0);
   // glUniformMatrix4fv(glGetUniformLocation(.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fireflyMatrix));

    /*firefly2.Draw(shader);
    firefly3.Draw(shader);
    firefly4.Draw(shader);
    firefly5.Draw(shader);*/
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    renderObject(depthMapShader, true);

    /*glClear(GL_DEPTH_BUFFER_BIT);*/
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap) {

        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, retina_width, retina_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mySkyBox.Draw(skyboxShader, view, projection);

        myBasicShader.useShaderProgram();

        view = activateScenePreview ? animationCamera.getViewMatrix() : myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

 
        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        renderObject(myBasicShader, false);
    }

    
    previewScene();
}

void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
    initn9();
	initUniforms();
    setWindowCallbacks();
    initSkyBoxFace();
    initSkyBox();
    

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {

        processKeyboardMovement();
	    renderScene();
       
        
		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
