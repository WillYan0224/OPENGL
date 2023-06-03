#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/* For debug */
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1 // for type cast
    };
     
    std::string line;
    std::stringstream ss[2]; // 
    ShaderType type = ShaderType::NONE; // default state

    while(getline(stream, line))
    {
        /* Error Handling */
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
        // return multiple value by struct
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // source -> src:  string -> char
    glShaderSource(id, 1, &src, nullptr); 
    glCompileShader(id);

    /* Error Handling */
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); 
    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char)); 
    	// OR IN heap: char* <var> = new [] 

    	glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed on compiling " <<
            (type == GL_FRAGMENT_SHADER ? "Fragment!" : "Vertex!") << " shader!\n" ;
        std::cout << message << std::endl;
        glDeleteShader(id);

    	return 0;
    }

	return id; 
}


static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) // Avoid leaking (Static)
{
    unsigned int program = glCreateProgram();  
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); 
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader); 


    /* ------- link vs & fs -------  */        
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
	
	/* ------- clear shader ------- */
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 640, "OpenGL Practice", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // Vsync

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

   
    float positions[] = { // mem buffer
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
    	-0.5f,  0.5f, // 3
    };

    unsigned int indices[] = { 
    	0, 1 , 2,
        2, 3 , 0,
    };

    std::cout << glGetString(GL_VERSION) << std::endl;

    /* ------------- Vertex Buffer  ------------- */
    unsigned int buffer;
    glGenBuffers(1, &buffer); // Gen buffer id
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // bind = select

	glBufferData(GL_ARRAY_BUFFER, 6 * 2 *sizeof(float), positions, GL_STATIC_DRAW);

 
    glEnableVertexAttribArray(0); //  Enable vertex attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // buffer layout

    /* ------------- インデックス -------------  */
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW); 

    /* ------------- shader ------------- */ 
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader); // Shader bounded

	/* ------------- Uniform: Shader Painting -------------  */ // 
    int location = glGetUniformLocation(shader, "u_Color");
    glUniform4f(location, 0.6f, 0.2, 0.8f, 1.0f); // 4f: vec4(4 Components) & Float
   
    

      

    float r = 0.0f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT); 

        glUniform4f(location, r, 0.5f, 0.2f, 1.0f); // 4f: vec4(4 Components) & Float
        // glDrawArrays(GL_TRIANGLES, 0, 3); // Vertex Buffer
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);  // Index Buffer
        // glDrawElements(GL_TRIANGLES, 3, GLfloat, NULL); 

        /* ------------- Change color ------------- */
        if (r > 1.0f)
            increment = -0.005;
        else if (r < 0.0f)
            increment = 0.005;
			r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader); // clear shader

    glfwTerminate();
    return 0;
}