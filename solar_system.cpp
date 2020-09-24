#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <shader.h>
#include <cube.h>
#include <arcball.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
//initialize funciton
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
unsigned int loadTexture(const char *);
void render();
void createSun();
void createEarth();
void createMoon();

// initialize variables
const float PI = acos(-1);
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
Shader *sunShader = NULL;
unsigned int SCR_WIDTH = 900;
unsigned int SCR_HEIGHT = 900;
float alphaN = 32, betaN = 32;
float alpha, beta;
vector<float> vertices_s, vertices_e, vertices_m;
vector<int> indices_s, indices_e, indices_m;
vector<float> texCoords_s, texCoords_e, texCoords_m;
vector<float> normals_s, normals_e, normals_m;
unsigned int VBO[3],VAO[3],EBO[3]; // 0 sun 1 earth 2 moon
int vSize[3], tSize[3], iSize[3], nSize[3];
static unsigned int sun, earth, moon;  // texture ids for solar
float x,y,z, xy,s,t;
glm::mat4 projection, view, model;
float radius_s = 0.3f;
float radius_e = 0.2f;
float radius_m = 0.1f;
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool arcballCamRot = true;
int length=0;
glm::vec3 cameraPos(0.0f, 3.0f, 7.0f);
float speed1 = glm::radians(10.0f);
float speed2 = glm::radians(30.0f);
float speed3 = glm::radians(60.0f);
glm::vec3 pointLightPositions[] = {
    glm::vec3( 0, 0,  0)
};


int main()
{
    
    mainWindow = glAllInit();
    
    globalShader = new Shader("global.vs", "global.fs"); // which shows sphere earth, moon
    sunShader = new Shader("sun.vs","sun.fs"); //which shows sun
    // get textures
    sun = loadTexture("sun.jpg");
    moon = loadTexture("moon.jpg");
    earth = loadTexture("earth.jpg");
    // get shader of sun
    sunShader -> use();
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
    sunShader ->setMat4("projection", projection);
    
    sunShader->setInt("material.diffuse", 0);
    sunShader->setInt("material.specular", 1);
    sunShader->setFloat("material.shininess", 32);
    
    sunShader->setVec3("viewPos", cameraPos);
    
    // transfer lighting parameters to fragment shader
    // directional light
    sunShader->setVec3("dirLight.direction", -10.2f, -10.0f, -10.3f);
    sunShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    sunShader->setVec3("dirLight.diffuse", 0.1f, 0.1f, 0.1f);
    sunShader->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);
    
    //other shaders
    globalShader -> use();
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader ->setMat4("projection", projection);
    
    globalShader->setInt("material.diffuse", 0);
    globalShader->setInt("material.specular", 1);
    globalShader->setFloat("material.shininess", 32);
    
    globalShader->setVec3("viewPos", cameraPos);
    
    // transfer lighting parameters to fragment shader
    // directional light
    globalShader->setVec3("dirLight.direction", -10.2f, -10.0f, -10.3f);
    globalShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    globalShader->setVec3("dirLight.diffuse", 0.1f, 0.1f, 0.1f);
    globalShader->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);
    // point light
    globalShader->setVec3("pointLights.position", pointLightPositions[0]);
    globalShader->setVec3("pointLights.ambient", 0.20f, 0.20f, 0.20f);
    globalShader->setVec3("pointLights.diffuse", 1.0f, 1.0f, 1.0f);
    globalShader->setVec3("pointLights.specular", 1.0f, 1.0f, 1.0f);
    globalShader->setFloat("pointLights.constant", 1.0f);
    globalShader->setFloat("pointLights.linear", 0.09f);
    globalShader->setFloat("pointLights.quadratic", 0.032f);
    
    // create spheres
    createSun();
    createEarth();
    createMoon();
    
    
    // in order to make vector to array
    float* arrV = &vertices_s[0];
    float* arrT = &texCoords_s[0];
    int* arrI = &indices_s[0];
    float* arrN = &normals_s[0];
    
    float* arrV1 = &vertices_e[0];
    float* arrT1 = &texCoords_e[0];
    int* arrI1 = &indices_e[0];
    float* arrN1 = &normals_e[0];
    
    float* arrV2 = &vertices_m[0];
    float* arrT2 = &texCoords_m[0];
    int* arrI2 = &indices_m[0];
    float* arrN2 = &normals_m[0];
    
    // make same size as sizeof(array) to arrange space
    vSize[0] = (float)(sizeof(arrV)*vertices_s.size());
    tSize[0] = (float)(sizeof(arrT)*texCoords_s.size());
    iSize[0] = (int)(sizeof(arrI)*indices_s.size());
    nSize[0] = (float)(sizeof(arrN)*normals_s.size());
    
    vSize[1] = (float)(sizeof(arrV1)*vertices_e.size());
    tSize[1] = (float)(sizeof(arrT1)*texCoords_e.size());
    iSize[1] = (int)(sizeof(arrI1)*indices_e.size());
    nSize[1] = (float)(sizeof(arrN1)*normals_e.size());
    
    vSize[2] = (float)(sizeof(arrV2)*vertices_m.size());
    tSize[2] = (float)(sizeof(arrT2)*texCoords_m.size());
    iSize[2] = (int)(sizeof(arrI2)*indices_m.size());
    nSize[2] = (float)(sizeof(arrN2)*normals_m.size());
    
    //sun
    glGenVertexArrays(1,&VAO[0]);
    glGenBuffers(1,&VBO[0]);
    glGenBuffers(1,&EBO[0]);
    
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
    glBufferData(GL_ARRAY_BUFFER,vSize[0]+tSize[0] + nSize[0],0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER,0,vSize[0], arrV);
    glBufferSubData(GL_ARRAY_BUFFER,vSize[0], tSize[0], arrT);
    glBufferSubData(GL_ARRAY_BUFFER,vSize[0]+ tSize[0], nSize[0], arrN);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize[0], arrI, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 2*sizeof(float),(void*)(vSize[0]));
    glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)(vSize[0] + tSize[0]));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
   
    //earth
    glGenVertexArrays(1,&VAO[1]);
    glGenBuffers(1,&VBO[1]);
    glGenBuffers(1,&EBO[1]);
    
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
    glBufferData(GL_ARRAY_BUFFER,vSize[1]+tSize[1] + nSize[1],0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER,0,vSize[1], arrV1);
    glBufferSubData(GL_ARRAY_BUFFER,vSize[1], tSize[1], arrT1);
    glBufferSubData(GL_ARRAY_BUFFER,vSize[1]+ tSize[1], nSize[1], arrN1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize[1], arrI1, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 2*sizeof(float),(void*)(vSize[1]));
    glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)(vSize[1] + tSize[1]));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    //moon
    glGenVertexArrays(1,&VAO[2]);
    glGenBuffers(1,&VBO[2]);
    glGenBuffers(1,&EBO[2]);
    
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER,VBO[2]);
    glBufferData(GL_ARRAY_BUFFER,vSize[2]+tSize[2]+nSize[2],0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER,0,vSize[2], arrV2);
    glBufferSubData(GL_ARRAY_BUFFER,vSize[2], tSize[2], arrT2);
    glBufferSubData(GL_ARRAY_BUFFER,vSize[2]+ tSize[2], nSize[2], arrN2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize[2], arrI2, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 2*sizeof(float),(void*)(vSize[2]));
    glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)(vSize[2] + tSize[2]));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    glDeleteVertexArrays(3, VAO);
    glDeleteBuffers(3,VBO);
    glDeleteBuffers(3,EBO);
    
    glfwTerminate();
    return 0;
}

GLFWwindow *glAllInit()
{
    GLFWwindow *window;
    
    // glfw: initialize and configure
    if (!glfwInit()) {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW1_2015147506", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    
    // OpenGL states
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Allow modern extension features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }
    
    return window;
}

unsigned int loadTexture(const char *texFileName) {
    unsigned int texture;
    
    // Create texture ids.
    glGenTextures(1, &texture);
    
    // All upcomming GL_TEXTURE_2D operations now on "texture" object
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters for wrapping.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);   // vertical flip the texture
    unsigned char *image = stbi_load(texFileName, &width, &height, &nrChannels, 0);
    if (!image) {
        printf("texture %s loading error ... \n", texFileName);
    }
    else printf("texture %s loaded\n", texFileName);
    
    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;
    
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture;
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // get view
    view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = view * camArcBall.createRotationMatrix();
    glm::mat4 transform;
    float currentTime = glfwGetTime();
    
    // get shader of sun
    sunShader->use();
    
    transform = glm::mat4(1.0f);
    //rotation of sun itself
    transform = glm::rotate(transform, speed2*currentTime, glm::vec3(0.0f,1.0f,0.0f));
    
    sunShader -> setMat4("transform", transform);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sun);
    sunShader->setMat4("view",view);
    glBindVertexArray(VAO[0]);
    glDrawElements(GL_TRIANGLES, 5952, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    
    globalShader->use();
    transform = glm::mat4(1.0f);
    //rotation of earth itself and around sun
    transform = glm::rotate(transform, speed2*currentTime, glm::vec3(0.0f,1.0f,0.0f));
    transform = glm::translate(transform, glm::vec3(1.5f,0.0f,0.0f));
    transform = glm::rotate(transform, speed2*currentTime, glm::vec3(0.0f,1.0f,0.0f));
    globalShader -> setMat4("transform", transform);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earth);
    globalShader->setMat4("view",view);
    glBindVertexArray(VAO[1]);
    glDrawElements(GL_TRIANGLES, 5952, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    globalShader->use();
    transform = glm::mat4(1.0f);
    
    transform = glm::rotate(transform, speed2*currentTime, glm::vec3(0.0f,1.0f,0.0f));
    transform = glm::translate(transform, glm::vec3(1.5f,0.0f,0.0f));
    
    transform = glm::rotate(transform, 3*speed2*currentTime, glm::vec3(0.0f,1.0f,0.0f));
    
    transform = glm::translate(transform, glm::vec3(-0.5f,0.0f,0.0f));
    globalShader -> setMat4("transform", transform);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, moon);
    globalShader->setMat4("view",view);
    glBindVertexArray(VAO[2]);
    glDrawElements(GL_TRIANGLES, 5952, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glfwSwapBuffers(mainWindow);
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (arcballCamRot)
        camArcBall.mouseButtonCallback( window, button, action, mods );
    else
        modelArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    if (arcballCamRot)
        camArcBall.cursorCallback( window, x, y );
    else
        modelArcBall.cursorCallback( window, x, y );
}

float betai = PI/betaN;
float alphai = 2*PI/alphaN;

void createSun(){
    for(int i = 0; i<= betaN; i++){
        beta = (-PI/2)+(i * betai); // from -PI/2 ~PI/2
        xy = radius_s * cos(beta);
        z = radius_s * sin(beta);
        
        for(int j = 0; j <= alphaN; j++){
            
            alpha = j * alphai;
            x = xy*cos(alpha);
            y = xy*sin(alpha);
            vertices_s.push_back(x);
            vertices_s.push_back(y);
            vertices_s.push_back(z);
            float lengthInv = 1.0f/radius_s;
            normals_s.push_back(x*lengthInv);
            normals_s.push_back(y*lengthInv);
            normals_s.push_back(z*lengthInv);
        }
    }
    for(int i = 0;i<=betaN;i++){
        for(int j=0;j<=betaN;j++){
            texCoords_s.push_back((float)j/betaN);
            texCoords_s.push_back((float)i/betaN);
        }
    }
    //
    //  _____
    //  |   /|
    //  |  / |
    //  | /  |
    //  |/   |
    //  ------
    //
    int i1, i2;
    for(int i = 0; i <= betaN; i++){
        i1 = i*(alphaN+1);
        i2 = i1 + alphaN + 1;
        for(int j = 0; j < alphaN; j++, i1++, i2++){
            if(i == 0){
                indices_s.push_back(i1+1);
                indices_s.push_back(i2);
                indices_s.push_back(i2+1);
            }
            else if(i==(betaN-1)){
                indices_s.push_back(i1+1);
                indices_s.push_back(i1);
                indices_s.push_back(i2);
            }else{
                
                indices_s.push_back(i1+1);
                indices_s.push_back(i1);
                indices_s.push_back(i2);
                indices_s.push_back(i1+1);
                indices_s.push_back(i2);
                indices_s.push_back(i2+1);
            }
        }
    }
}

void createEarth(){
    
        for(int i = 0; i<= betaN; i++){
            beta = (-PI/2)+(i * betai);
            xy = radius_e * cos(beta);
            z = radius_e * sin(beta);
            
            for(int j = 0; j <= alphaN; j++){
                
                alpha = j * alphai;
                
                x = xy*cos(alpha);
                y = xy*sin(alpha);
                
                vertices_e.push_back(x);
                vertices_e.push_back(z);
                vertices_e.push_back(y);
                float lengthInv = 1.0f/radius_e;
                normals_e.push_back(x*lengthInv);
                normals_e.push_back(z*lengthInv);
                normals_e.push_back(y*lengthInv);
            }
        }
        for(int i = 0;i<=betaN;i++){
            for(int j=0;j<=betaN;j++){
                texCoords_e.push_back((float)j/betaN);
                texCoords_e.push_back((float)i/betaN);
            }
        }
        int e1, e2;
        for(int i = 0; i < betaN; i++){
            e1 = i*(alphaN+1);
            e2 = e1 + alphaN + 1;
            for(int j = 0; j < alphaN; j++, e1++, e2++){
                if(i == 0){
                    indices_e.push_back(e1+1);
                    indices_e.push_back(e2);
                    indices_e.push_back(e2+1);
                }
                else if(i==(betaN-1)){
                    indices_e.push_back(e1+1);
                    indices_e.push_back(e1);
                    indices_e.push_back(e2);
                }else{
                    
                    indices_e.push_back(e1+1);
                    indices_e.push_back(e1);
                    indices_e.push_back(e2);
                    indices_e.push_back(e1+1);
                    indices_e.push_back(e2);
                    indices_e.push_back(e2+1);
                }
            }
        }
    }
 
void createMoon(){
        for(int i = 0; i<= betaN; i++){
            beta = PI/2-(i * betai);
            xy = radius_m * cos(beta);
            z = radius_m * sin(beta);
            
            for(int j = 0; j <= alphaN; j++){
                
                alpha = j * alphai;
                
                x = xy*cos(alpha);
                y = xy*sin(alpha);
                vertices_m.push_back(x);
                vertices_m.push_back(y);
                vertices_m.push_back(z);
                float lengthInv = 1.0f/radius_m;
                normals_m.push_back(x*lengthInv);
                normals_m.push_back(y*lengthInv);
                normals_m.push_back(z*lengthInv);
            }
        }
        for(int i = 0;i<=betaN;i++){
            for(int j=0;j<=betaN;j++){
                texCoords_m.push_back((float)j/betaN);
                texCoords_m.push_back((float)i/betaN);
            }
        }
        int m1, m2;
        for(int i = 0; i <= betaN; i++){
            m1 = i*(alphaN+1); // starting from level 0 ~
            m2 = m1 + alphaN + 1;
            for(int j = 0; j < alphaN; j++, m1++, m2++){
                if(i == 0){
                    indices_m.push_back(m1+1);
                    indices_m.push_back(m2);
                    indices_m.push_back(m2+1);
                }
                else if(i==(betaN-1)){
                    indices_m.push_back(m1+1);
                    indices_m.push_back(m1);
                    indices_m.push_back(m2);
                }else{
                    
                    indices_m.push_back(m1+1);
                    indices_m.push_back(m1);
                    indices_m.push_back(m2);
                    indices_m.push_back(m1+1);
                    indices_m.push_back(m2);
                    indices_m.push_back(m2+1);
                }
            }
        }
    }
    
    

