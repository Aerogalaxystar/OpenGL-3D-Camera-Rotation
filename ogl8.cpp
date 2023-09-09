#include<iostream>
#include<glad/glad.h>
#include<GL/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include "Camera.h"

const unsigned int width =1920;
const unsigned  int height=1080;


void ProcessInput(GLFWwindow *);
void mousecb(GLFWwindow *,double xpos,double ypos);
void scrollcb(GLFWwindow *,double xoffset,double yoffset);

glm::vec3 cameraPos   =glm::vec3(0.0f,0.0f,3.0f);
glm::vec3 cameraFront =glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 cameraUp    =glm::vec3(0.0f,1.0f,0.0f);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool firstMouse=true;
float lastX = width / 2.0f;
float lastY = height / 2.0f;
float deltaTime=0.0f;  //time between last frame and current frame
float lastFrame=0.0f;


GLfloat vertices[] =
{
//               COORDINATES                  /     COLORS          //
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};

// Indices for vertices order
GLuint indices[] =
{
    0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};



int main()
{
	// Initialize GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(width,height, "Camera GL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mousecb);
    glfwSetScrollCallback(window, scrollcb);
     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	gladLoadGL();

	glViewport(0, 0,width,height);
	
	Shader shaderProgram("default.vert", "default.frag");



	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));

	EBO EBO1(indices, sizeof(indices));


	VAO1.LinkAttrib(VBO1,0,3,GL_FLOAT,8*sizeof(float),(void*)0);
	VAO1.LinkAttrib(VBO1,1,3,GL_FLOAT,8*sizeof(float),(void*)(3*sizeof(float)));
	VAO1.LinkAttrib(VBO1,2,2,GL_FLOAT,8*sizeof(float),(void*)(6*sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

   Texture brick("wall.jpg",GL_TEXTURE_2D,GL_TEXTURE0,GL_RGB,GL_UNSIGNED_BYTE);
   brick.texUnit(shaderProgram,"tex0",0);
   
   float rotation=0.0f;
   double prevTime=glfwGetTime();
   	 glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        float currentFrame=glfwGetTime();
        deltaTime=currentFrame-lastFrame;
        lastFrame=currentFrame;
        ProcessInput(window);
      
		shaderProgram.Activate();
	
		glm::mat4 model=glm::mat4(1.0f);
		glm::mat4 view=glm::mat4(1.0f);
		glm::mat4 proj=glm::mat4(1.0f);
	
		const float radius = 5.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        //view= glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        view=camera.GetViewMatrix(); 
		double crntTime=glfwGetTime();
		if(crntTime-prevTime>=1/60)
		{
			rotation +=0.9f;
			prevTime=crntTime;
		}
		float angle = 0.0f * 1;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		view=glm::translate(view,glm::vec3(0.0f,-0.28f,-2.0f));
		proj=glm::perspective(glm::radians(camera.Zoom),(float)width/height,0.1f,100.0f);
	
	    int modelloc = glGetUniformLocation(shaderProgram.ID,"model");
	    glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(model));
	    int viewloc = glGetUniformLocation(shaderProgram.ID,"view");
	    glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(view));
	    int projloc = glGetUniformLocation(shaderProgram.ID,"proj");
	    glUniformMatrix4fv(projloc,1,GL_FALSE,glm::value_ptr(proj));
	    
		
		glUniform1f(uniID, 0.5f);
	    brick.Bind();
		VAO1.Bind();
	
		glDrawElements(GL_TRIANGLES,sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	
		glfwPollEvents();
	}
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	brick.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void ProcessInput(GLFWwindow *window)
{
	 if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
void mousecb(GLFWwindow* window, double xpos, double ypos)
{
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
    
   camera.ProcessMouseMovement(xoffset, yoffset);

}
void scrollcb(GLFWwindow *window,double xoffset,double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
