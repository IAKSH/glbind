#include "timer.hpp"
#include <frame.hpp>
#include <primitive.hpp>
#include <scope.hpp>
#include <shader.hpp>
#include <texture.hpp>
#include <vertex.hpp>
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

constexpr std::string_view kenel_post_effect_fragment_shader
{
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec4 ourColor;\n"
    "in vec2 TexCoord;\n"
    "\n"
    "uniform sampler2D screenTexture;\n"
    "\n"
    "const float offset = 1.0 / 300.0;\n"  
    "\n"
    "void main()\n"
    "{\n"
    "vec2 offsets[9] = vec2[](\n"
    "    vec2(-offset,  offset), // 左上\n"
    "    vec2( 0.0f,    offset), // 正上\n"
    "    vec2( offset,  offset), // 右上\n"
    "    vec2(-offset,  0.0f),   // 左\n"
    "    vec2( 0.0f,    0.0f),   // 中\n"
    "    vec2( offset,  0.0f),   // 右\n"
    "    vec2(-offset, -offset), // 左下\n"
    "    vec2( 0.0f,   -offset), // 正下\n"
    "    vec2( offset, -offset)  // 右下\n"
    ");\n"
    "\n"
    "float kernel[9] = float[](\n"
    "    -1, -1, -1,\n"
    "    -1,  9, -1,\n"
    "    -1, -1, -1\n"
    ");\n"
    "\n"
    "vec3 sampleTex[9];\n"
    "for(int i = 0; i < 9; i++)\n"
    "{\n"
    "    sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));\n"
    "}\n"
    "vec3 col = vec3(0.0);\n"
    "for(int i = 0; i < 9; i++)\n"
    "    col += sampleTex[i] * kernel[i];\n"
    "\n"
    "FragColor = ourColor * vec4(col, 1.0);\n"
    "//FragColor = ourColor * vec4(vec3(1.0 - col), 1.0);\n"
    "}\n\0"
};

static constexpr std::array<float,36> rect_vertices
{
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  // top right
    1.0f,  -1.0f, 0.0f,1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f,1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // bottom left
    -1.0f, 1.0f,  0.0f,1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f   // top left
};

static constexpr std::array<float,36> rect_vertices_colorful
{
    1.0f,  1.0f,  0.3f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 1.0f,  // top right
    1.0f,  -1.0f, 0.0f,0.6f, 0.3f, 1.0f, 1.0f, 1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f,0.7f, 0.3f, 0.1f, 1.0f, 0.0f, 0.0f,  // bottom left
    -1.0f, 1.0f,  0.0f,1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
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
        graphics::Program program((graphics::VShader(vertex_shader_glsl)),(graphics::FShader(fragment_shader_glsl)));
        graphics::Program post_kernel_program((graphics::VShader(vertex_shader_glsl)),(graphics::FShader(kenel_post_effect_fragment_shader)));
        graphics::VertexBuffer<graphics::BufferType::Static,36> vbo(rect_vertices_colorful);
        graphics::ElementBuffer<graphics::BufferType::Static,6> ebo(rect_indices);
        graphics::VertexArray vao(vbo,ebo);
        // position attrib
        vao.enable_attrib(0,3,9,0,false);
        // color attrib
        vao.enable_attrib(1,4,9,3,false);
        // texture coord attrib
        vao.enable_attrib(2,2,9,7,false);

        program.use();
        program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(0.5f,0.5f,1.0f)));
        post_kernel_program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(0.75f,0.6f,1.0f)));

        std::array<std::string_view,3> img_pathes
        {
            "E:\\Programming-Projects\\glbind\\tests\\img\\R-C.png",
            "E:\\Programming-Projects\\glbind\\tests\\img\\wires.png",
            "E:\\Programming-Projects\\glbind\\tests\\img\\3.png"
        };

        std::array<Image,3> images;
        for(std::size_t i = 0;i < 3;i++)
            images[i] = load_image(img_pathes[i]);

        std::array<std::unique_ptr<graphics::TextureRGBA>,3> textures;
        for(std::size_t i = 0;i < 3;i++)
        {
            auto& img {images[i]};
            textures[i] = std::make_unique<graphics::TextureRGBA>(img.data.get(),img.channels,0,0,img.width,img.height);
        }

        graphics::TextureRGB frame_tex1(nullptr,3,0,0,800,600);
        graphics::Frame frame1(frame_tex1);

        graphics::TextureRGB frame_tex2(nullptr,3,0,0,800,600);
        graphics::Frame frame2(frame_tex2);

        rkki::test::TimeRecorder recorder;
        
        std::size_t tex_index {0};
        while(true)
        {
            graphics::Scope([&]()
            {
                // draw obj to frame1
                graphics::Scope(0,0,800,600,[&]()
                {
                    program.use();
                    program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(0.7f,0.5f,1.0f)));
                    frame1.use();
                    textures[tex_index]->bind();
                    graphics::draw<graphics::Primitives::Triangles>(vao,6);
                });

                // apply kernel effect & draw to frame2
                graphics::Scope(0,0,800,600,[&]()
                {
                    frame2.use();
                    post_kernel_program.use();
                    frame_tex1.bind();
                    post_kernel_program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,0.9f,1.0f)));
                    graphics::draw<graphics::Primitives::Triangles>(vao,6);
                });

                // apply shake effect & draw to screen
                graphics::Scope(0,0,800,600,[&]()
                {
                    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
                    glClear(GL_COLOR_BUFFER_BIT);

                    //post_shake_program.use();
                    frame_tex1.bind();
                    graphics::draw<graphics::Primitives::Triangles>(vao,6);

                    glfwPollEvents();
                    glfwSwapBuffers(window);
                });
            });

            if(recorder.get_time_span_ms() >= rkki::test::MilliSeconds(1000))
            {
                ++tex_index;
                recorder.update();
            }

            if(tex_index > textures.size() - 1)
                tex_index = 0;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    catch(const std::exception& e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
        std::terminate();
    }
    catch(...)
    {
        std::cerr << "unknow exception catched" << std::endl;
        std::terminate();
    }
}