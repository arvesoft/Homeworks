#include "helper.h"
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

double _angle = 45.f;
double aspect = 1.f;
double near_d = 0.1f;
double far_d = 1000.f;

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

// Vertex buffer and vectors
vector< vec3 > vertexVector;
vector< vec3 > vertices;
GLuint vertexBuffer;

// New Matrices
GLuint viewMatrixUni;
GLuint projectMatrixUni;

int widthTexture, heightTexture;
int fullScreen = 0;
int oldWidht = 0, oldHeight = 0, lookW = 0, lookH = 0;

GLfloat camSpeed = 0;
GLfloat heightFactor = 10.0f;

vec3 gaze_vec;
vec3 up_vec;
vec3 eye_vec;

void initializeJPG(){

  eye_vec = { widthTexture / 2.f, widthTexture / 10.f , widthTexture / -4.f };
  up_vec = {0.0f, 1.f, 0.0f};
  gaze_vec = {0.0f, 0, 1.f};

  for( int z = 0; z <= heightTexture; z++){
    for( int x = 0; x <= widthTexture; x++){

      vec3 pos_vec = {x, 0.0f, z};
      vertexVector.push_back(pos_vec);
    }
  }

  // Sadece vertexleri pushla
  for (int z = 0; z < heightTexture; z++) {
    for (int x = 0; x <= widthTexture; x++) {

        int temp = z * widthTexture + x;

        if( temp % (widthTexture + 1) != widthTexture ){
          // Saat yonunde sol alt ucgen
          vertices.push_back(vertexVector[temp + widthTexture + 1]);
          vertices.push_back(vertexVector[temp + widthTexture + 2]);
          vertices.push_back(vertexVector[temp + 1]);
          // Saat yonunde sag ust ucgen
          vertices.push_back(vertexVector[temp]);
          vertices.push_back(vertexVector[temp + widthTexture + 1]);
          vertices.push_back(vertexVector[temp + 1]);
        }
    }
}

  unsigned int verticesBufferSize = vertices.size() * sizeof(vec3);

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // The vertex array pointer parameter is interpreted as an
                                              // offset within the buffer object measured in basic machine units.

  glBufferData(GL_ARRAY_BUFFER, verticesBufferSize, &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), NULL);

}

void reCalculateView(){

  eye_vec += gaze_vec * camSpeed;

  mat4 viewM = lookAt(eye_vec, gaze_vec + eye_vec, up_vec);
  mat4 projectM =  perspective(_angle, aspect, near_d, far_d);
  mat4 MVPM = projectM * viewM;

  glUniformMatrix4fv(projectMatrixUni, 1, GL_FALSE, &MVPM[0][0]);
  glUniformMatrix4fv(viewMatrixUni, 1, GL_FALSE, &viewM[0][0]);

  int width, height;
  glfwGetWindowSize(win, &width, &height);
  glViewport(0, 0, width, height);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());

}

static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

void rotateMatrix(int check, int check2){

    //std::cout << "Girerken gaze = " << gaze_vec.x << " " << gaze_vec.y << " " << gaze_vec.z << std::endl;
    //std::cout << "Girerken up = " << up_vec.x << " " << up_vec.y << " " << up_vec.z << std::endl;
    vec3 ou, ov = up_vec, ow = gaze_vec;

    if(check2 == 1) ou = cross(ov, ow);
    else ou = up_vec;

    double cosTeta;
    double sinTeta;

    if(check == 1){

        cosTeta = cos(1.0f / 180.0 * M_PI);
        sinTeta = sin(1.0f / 180.0 * M_PI);
    }
    else{

        cosTeta = cos(-1.0f / 180.0 * M_PI);
        sinTeta = sin(-1.0f / 180.0 * M_PI);
    }
    // rotationMatrix for X
    mat4 rotationMatrix(
        1, 0, 0, 0,
        0, cosTeta, -sinTeta, 0,
        0, sinTeta,  cosTeta, 0,
        0, 0      , 0       , 1);

    mat4 M_temp;

    if( abs(ou.x) <= abs(ou.y) && abs(ou.x) <= abs(ou.z) ){
        ov.x = 0;
        ov.y = -ou.z;
        ov.z = ou.y;
    }
    else if( abs(ou.y) <= abs(ou.z) && abs(ou.y) <= abs(ou.x) ){
        ov.x = -ou.z;
        ov.y = 0;
        ov.z = ou.x;
    }
    else{
        ov.x = -ou.y;
        ov.y = ou.x;
        ov.z = 0;
    }

    ow = normalize( cross(ou, ov) );
    ou = normalize(ou);
    ov = normalize(ov);

    mat4 M_undo(
         ou.x, ov.x, ow.x, 0,
         ou.y, ov.y, ow.y, 0,
         ou.z, ov.z, ow.z, 0,
         0   , 0   , 0   , 1);

    mat4 M_do(
        ou.x, ou.y, ou.z, 0,
        ov.x, ov.y, ov.z, 0,
        ow.x, ow.y, ow.z, 0,
        0   , 0   , 0   , 1);

    M_temp = M_do * (rotationMatrix * M_undo);
    ow = gaze_vec;
    // gaze_vec = .xyz gave error??
    gaze_vec.x = (M_temp * vec4(ow, 0)).x;
    gaze_vec.y = (M_temp * vec4(ow, 0)).y;
    gaze_vec.z = (M_temp * vec4(ow, 0)).z;
    gaze_vec = normalize(gaze_vec);

    up_vec = normalize( cross(gaze_vec, cross(up_vec, gaze_vec) ) );

    //std::cout << "Cikarken gaze = " << gaze_vec.x << " " << gaze_vec.y << " " << gaze_vec.z << std::endl;
    //std::cout << "Cikarken up = " << up_vec.x << " " << up_vec.y << " " << up_vec.z << std::endl;
}


void userInput(GLFWwindow* window, int userInp, int scancode, int action, int mods){

    if(action == GLFW_RELEASE)
        return;

    /// Handle ESC
    else if(userInp == GLFW_KEY_ESCAPE){

        glfwSetWindowShouldClose(win, 1);
        //exit(0);
    }

    /// Handle U and J
    else if(userInp == GLFW_KEY_U){

      camSpeed++;
    }
    else if(userInp == GLFW_KEY_J){

      camSpeed--;
    }

    /// handle O and L
    else if(userInp == GLFW_KEY_O || userInp == GLFW_KEY_L){

        if(userInp == GLFW_KEY_O)
            heightFactor += 0.5;
        else
            heightFactor -= 0.5;

        GLuint heightFactorUni = glGetUniformLocation(idProgramShader, "heightFactor");
        glUniform1f(heightFactorUni, heightFactor);
    }

    else if(userInp == GLFW_KEY_F){

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidmod = glfwGetVideoMode(monitor);

        if(fullScreen == 0){

            fullScreen = 1;

            glfwGetWindowPos(win, &lookW, &lookH);
            glfwGetWindowSize(win, &oldWidht, &oldHeight);
            //std::cout << "Old height = " << oldHeight << " oldWidht = " << oldWidht << std::endl;
            //std::cout << "vidmod height = " << vidmod->height << " vidmod Widht = " << vidmod->width << std::endl;
            glfwSetWindowMonitor(win, monitor, 0, 0, vidmod->width, vidmod->height, vidmod->refreshRate);
        }

        else{

            fullScreen = 0;
            glfwSetWindowMonitor(win, NULL, lookW, lookH, oldWidht, oldHeight, vidmod->refreshRate);
        }
    }

    else if(userInp == GLFW_KEY_A || userInp == GLFW_KEY_D){

        int check = 1;
        if(userInp == GLFW_KEY_A) check = 0;
        rotateMatrix(check, 0);
    }

    else if(userInp == GLFW_KEY_W || userInp == GLFW_KEY_S){

        int check = 1;
        if(userInp == GLFW_KEY_S) check = 0;
        rotateMatrix(check, 1);
    }
}

int main(int argc, char * argv[]) {

  if (argc != 2) {
    printf("Only one texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }

  glEnable(GL_DEPTH_TEST); // Enable depth test
  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);

  GLuint widthUni = glGetUniformLocation(idProgramShader, "widthTexture");
  GLuint heightUni = glGetUniformLocation(idProgramShader, "heightTexture");

  GLuint heightFactorUni = glGetUniformLocation(idProgramShader, "heightFactor");

  projectMatrixUni = glGetUniformLocation(idProgramShader, "MVP");
  viewMatrixUni = glGetUniformLocation(idProgramShader, "MV");

  glUniform1f(widthUni, widthTexture);
  glUniform1f(heightUni, heightTexture);
  glUniform1f(heightFactorUni, heightFactor);

  initializeJPG();
  glfwSetKeyCallback(win, userInput);

  while( !glfwWindowShouldClose(win) ){

    reCalculateView();
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}
