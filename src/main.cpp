#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "ShaderProgram.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// Utility function to check for shader compilation errors
void checkShaderCompileStatus(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader Compilation Failed: " << log << std::endl;
    }
}

// Utility function to check for shader program link errors
void checkProgramLinkStatus(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program Linking Failed: " << log << std::endl;
    }
}

// Function to create and compile the compute shader
GLuint createComputeShader(const char* shaderSource) {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);
    checkShaderCompileStatus(shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    checkProgramLinkStatus(program);

    glDeleteShader(shader);
    return program;
}
// COMPUTE

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1); // vsync


    glewExperimental = true;

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("GLEW Init error");
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // build and compile shaders
    // -------------------------
    Shader shader("./res/Shaders/2.stencil_testing.vs", "./res/Shaders/2.stencil_testing.fs");
    Shader shaderSingleColor("./res/Shaders/2.stencil_testing.vs", "./res/Shaders/2.stencil_single_color.fs");
    Shader quad("./res/Shaders/quad.vs", "./res/Shaders/quad.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // quad (cw)
    // float quadVertices[] = {
    //     // positions   // texCoords
    //     -1.0f,  1.0f,   0.0f, 1.0f,
    //     -1.0f, -1.0f,   0.0f, 0.0f,
    //     1.0f, -1.0f,   1.0f, 0.0f,

    //     -1.0f,  1.0f,   0.0f, 1.0f,
    //     1.0f, -1.0f,   1.0f, 0.0f,
    //     1.0f,  1.0f,   1.0f, 1.0f
    // };  

    // quad (ccw_
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
        1.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f,   0.0f, 0.0f,  // bottom-left

        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
        1.0f,  1.0f,   1.0f, 1.0f,  // top-right
        1.0f, -1.0f,   1.0f, 0.0f   // bottom-right
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int cubeTexture = loadTexture("./res/Textures/marble.jpg");
    unsigned int floorTexture = loadTexture("./res/Textures/metal.png");


    GLuint framebuffer, texture;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create the texture (2D texture for color attachment)
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    // COMPUTE

    // Create Shader Storage Buffer Object (SSBO)
    const int dataSize = 1024; // Array of 1024 integers
    std::vector<int> data(dataSize, 0); // Initialize to 0
    //GLuint computeProgram = createComputeShader(computeShaderSource);
    // GLuint computeTexture = createComputeShader(computeTextureSource);
    //GLuint computeRay = createComputeShader(computeRaySource);

    ShaderProgram computeShaderProgram;
    {
        std::string shaderSource;
        std::ifstream shaderFile;
        shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            shaderFile.open("./res/Shaders/compute_shader.glsl");
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            shaderSource = shaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        ShaderProgram::ShaderCreateInfo computeInfo;
        computeInfo.type = ShaderProgram::ShaderCreateInfo::Type::COMPUTE;
        computeInfo.sourceData = shaderSource.c_str();
        computeInfo.sourceSize = shaderSource.size();

        computeShaderProgram.addShader(computeInfo);
    }
    computeShaderProgram.linkProgram();

    ShaderProgram rayTraceProgram;
    {
        std::string shaderSource;
        std::ifstream shaderFile;
        shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            shaderFile.open("./res/Shaders/ray_trace.glsl");
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            shaderSource = shaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        ShaderProgram::ShaderCreateInfo computeInfo;
        computeInfo.type = ShaderProgram::ShaderCreateInfo::Type::COMPUTE;
        computeInfo.sourceData = shaderSource.c_str();
        computeInfo.sourceSize = shaderSource.size();

        rayTraceProgram.addShader(computeInfo);
    }
    rayTraceProgram.linkProgram();

    ShaderProgram rayTraceMultiProgram;
    {
        std::string shaderSource;
        std::ifstream shaderFile;
        shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try {
            // open files
            shaderFile.open("./res/Shaders/ray_trace_multi.glsl");
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            shaderSource = shaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        ShaderProgram::ShaderCreateInfo computeInfo;
        computeInfo.type = ShaderProgram::ShaderCreateInfo::Type::COMPUTE;
        computeInfo.sourceData = shaderSource.c_str();
        computeInfo.sourceSize = shaderSource.size();

        rayTraceMultiProgram.addShader(computeInfo);
    }
    rayTraceMultiProgram.linkProgram();
    checkProgramLinkStatus(rayTraceMultiProgram.getProgramId());

    {
        computeShaderProgram.bind();

        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize * sizeof(int), data.data(), GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        // Dispatch Compute Shader
        glDispatchCompute(dataSize / 256, 1, 1); // 1024 elements, 256 per workgroup → 4 workgroups

        // Ensure execution finishes
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Retrieve the modified data from GPU
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(int), data.data());

        // Print some results
        std::cout << "First 10 elements after compute shader:\n";
        for (int i = 0; i < 10; ++i) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    {
        struct Sphere {
            glm::vec3 center;
            float radius;
            glm::vec3 color;
            float reflectivity;
        };

        rayTraceMultiProgram.bind();

        // Define a list of spheres
        std::vector<Sphere> spheres = {
            {{ 5.0f,  1.0f,  6.0f}, 1.0f, {1.0f, 0.2f, 0.2f}, 0.5f},  // Red, 50% reflective
            {{5.5f,  1.5f,  8.0f}, 0.7f, {0.2f, 1.0f, 0.2f}, 0.2f},  // Green, 20% reflective
            {{ 1.5f,  1.5f,  10.5f}, 1.2f, {0.2f, 0.2f, 1.0f}, 0.7f}   // Blue, 70% reflective
        };

        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(Sphere), spheres.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo); // Bind to binding=1
        rayTraceMultiProgram.setInt("numSpheres", spheres.size());
    }

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // glBindTexture(GL_TEXTURE_2D, texture);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!


        // set uniforms
        // shaderSingleColor.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // shaderSingleColor.setMat4("view", view);
        // shaderSingleColor.setMat4("projection", projection);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
        // glStencilMask(0x00);
        // // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // // --------------------------------------------------------------------
        // glStencilFunc(GL_ALWAYS, 1, 0xFF);
        // glStencilMask(0xFF);
        // // cubes
        // glBindVertexArray(cubeVAO);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        // shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        // shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
        // // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
        // // the objects' size differences, making it look like borders.
        // // -----------------------------------------------------------------------------------------------------------------------------
        // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // glStencilMask(0x00);
        // glDisable(GL_DEPTH_TEST);
        // shaderSingleColor.use();
        // float scale = 1.1f;
        // // cubes
        // glBindVertexArray(cubeVAO);
        // glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        // model = glm::scale(model, glm::vec3(scale, scale, scale));
        // shaderSingleColor.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(scale, scale, scale));
        // shaderSingleColor.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);
        // glStencilMask(0xFF);
        // glStencilFunc(GL_ALWAYS, 0, 0xFF);
        // glEnable(GL_DEPTH_TEST);

        // COMPUTE
        // {
        //     glUseProgram(computeProgram);    
        //     // Dispatch Compute Shader
        //     glDispatchCompute(dataSize / 256, 1, 1); // 1024 elements, 256 per workgroup → 4 workgroups

        //     // Ensure execution finishes
        //     glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        //     // Retrieve the modified data from GPU
        //     glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(int), data.data());

        //     // Print some results
        //     std::cout << "First 10 elements after compute shader:\n";
        //     for (int i = 0; i < 10; ++i) {
        //         std::cout << data[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //glUseProgram(computeRay);
        rayTraceMultiProgram.bind();
        // rayTraceMultiProgram.setMat4("viewMatrix", view);
        // rayTraceMultiProgram.setMat4("projMatrix", projection);
        rayTraceMultiProgram.setMat4("invProjMatrix", glm::inverse(projection));
        rayTraceMultiProgram.setMat4("invViewMatrix", glm::inverse(view));
        glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glDispatchCompute((SCR_WIDTH + 15) / 16, (SCR_HEIGHT + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  // Ensure updates are visible


        glClearColor(0.1f, 0.1f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        quad.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        //quad.setMat4("projection", projection);
        shader.setInt("screenTexture", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}