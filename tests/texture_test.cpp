#include <opengl.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string_view>
#include <iostream>
#include <memory>
#include <chrono>

static GLFWwindow* window {nullptr};

void initialize_window() noexcept
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback([](int error,const char* description){
        std::cerr << "GLFW error {}: " << description << std::endl;
        std::terminate();
    });

    window = glfwCreateWindow(800,600,"test",nullptr,nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,[](GLFWwindow* window,int width,int height){glViewport(0,0,width,height);});

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::terminate();
    }
}

using namespace rkki;

constexpr std::string_view vertex_shader_glsl
{
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "\n"
    "uniform mat4 transform;\n"
    "\n"
    "out vec4 ourColor;\n"
    "out vec2 TexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "gl_Position = transform * vec4(aPos, 1.0);\n"
    "ourColor = aColor;\n"
    "TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);\n"
    "}\n\0"
};

constexpr std::string_view fragment_shader_glsl
{
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec4 ourColor;\n"
    "in vec2 TexCoord;\n"
    "\n"
    "uniform sampler2D ourTexture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n\0"
};

static constexpr std::array<float,36> rect_vertices
{
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  // top right
    1.0f,  -1.0f, 0.0f,1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f,1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // bottom left
    -1.0f, 1.0f,  0.0f,1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f   // top left
};

static constexpr std::array<unsigned int,6> rect_indices
{
    0,1,3,
    1,2,3
};

struct Image
{
    std::unique_ptr<unsigned char[]> data;
    int width;
    int height;
    int channels;
};

Image load_image(std::string_view path) noexcept(false)
{
    Image img;
    img.data = std::unique_ptr<unsigned char[]>(stbi_load(path.data(),&img.width,&img.height,&img.channels,0));
    if(!img.data)
        throw std::runtime_error("Failed to load image");

    return img;
}

int main() noexcept
{
    initialize_window();

    try
    {
        // fill opengl code here
        glbind::Program program((glbind::VShader(vertex_shader_glsl)),(glbind::FShader(fragment_shader_glsl)));
        glbind::VertexArray<glbind::VertexBufferType::Static,4,2> vertices(rect_vertices,rect_indices);

        program.use();
        program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(0.5f,0.5f,0.5f)));

        auto img {load_image("E:\\Programming-Projects\\glbind\\wires.jpg")};
        glbind::TextureRGB<glbind::ColorChannelType::RGB> tex(img.data.get(),0,0,img.width,img.height);
        tex.bind();
        
        for(int i = 0;i < 2;i++)
        {
            vertices.draw();
            glfwPollEvents();
            glfwSwapBuffers(window);
            glClearColor(0.2f,0.3f,0.3f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    catch(const std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "unknow exception catched" << std::endl;
    }
}