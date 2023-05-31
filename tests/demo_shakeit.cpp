// not finish

#include "timer.hpp"
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

constexpr std::string_view shake_post_effect_vertex_shader
{
    "#version 330 core\n"
    "//layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "layout (location = 2) in vec2 texCoords;\n"
    "\n"
    "out vec2 TexCoords;\n"
    "\n"
    "uniform bool  chaos;\n"
    "uniform bool  confuse;\n"
    "uniform bool  shake;\n"
    "uniform float time;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.xy, 0.0f, 1.0f); \n"
    "    vec2 texture = texCoords;\n"
    "    if(chaos)\n"
    "    {\n"
    "        float strength = 0.3;\n"
    "        vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength);        \n"
    "        TexCoords = pos;\n"
    "    }\n"
    "    else if(confuse)\n"
    "    {\n"
    "        TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        TexCoords = texture;\n"
    "    }\n"
    "    if (shake)\n"
    "    {\n"
    "        float strength = 0.01;\n"
    "        gl_Position.x += cos(time * 10) * strength;        \n"
    "        gl_Position.y += cos(time * 15) * strength;        \n"
    "    }\n"
    "}\n"
};

constexpr std::string_view shake_post_effect_fragment_shader
{
    "#version 330 core\n"
    "in  vec2  TexCoord;\n"
    "in vec4 ourColor;\n"
    "out vec4  color;\n"
    "\n"
    "uniform sampler2D scene;\n"
    "uniform vec2      offsets[9];\n"
    "uniform int       edge_kernel[9];\n"
    "uniform float     blur_kernel[9];\n"
    "uniform bool chaos;\n"
    "uniform bool confuse;\n"
    "uniform bool shake;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    color = vec4(0.0f);\n"
    "    vec3 sample[9];\n"
    "    // 如果使用卷积矩阵，则对纹理的偏移像素进行采样\n"
    "    if(chaos || shake)\n"
    "        for(int i = 0; i < 9; i++)\n"
    "            sample[i] = vec3(texture(scene, TexCoord.st + offsets[i]));\n"
    "\n"
    "    // 处理特效\n"
    "    if(chaos)\n"
    "    {           \n"
    "        for(int i = 0; i < 9; i++)\n"
    "            color += vec4(sample[i] * edge_kernel[i], 0.0f);\n"
    "        color.a = 1.0f;\n"
    "    }\n"
    "    else if(confuse)\n"
    "    {\n"
    "        color = vec4(1.0 - texture(scene, TexCoord).rgb, 1.0);\n"
    "    }\n"
    "    else if(shake)\n"
    "    {\n"
    "        for(int i = 0; i < 9; i++)\n"
    "            color += vec4(sample[i] * blur_kernel[i], 0.0f);\n"
    "        color.a = 1.0f;\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        color =  texture(scene, TexCoord);\n"
    "    }\n"
    "}\n"
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
        glbind::Program program((glbind::VShader(vertex_shader_glsl)),(glbind::FShader(fragment_shader_glsl)));
        glbind::Program post_kernel_program((glbind::VShader(vertex_shader_glsl)),(glbind::FShader(kenel_post_effect_fragment_shader)));
        glbind::Program post_shake_program((glbind::VShader(shake_post_effect_vertex_shader)),(glbind::FShader(shake_post_effect_fragment_shader)));
        glbind::VertexArray<glbind::VertexBufferType::Static,4,2> vertices(rect_vertices_colorful,rect_indices);

        program.use();
        program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(0.5f,0.5f,1.0f)));
        post_kernel_program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(0.75f,0.6f,1.0f)));

        // TODO: can't work
        post_shake_program.set_uniform("chaos",true);
        post_shake_program.set_uniform("shake",true);

        std::array<std::string_view,3> img_pathes
        {
            "E:\\Programming-Projects\\glbind\\tests\\img\\R-C.png",
            "E:\\Programming-Projects\\glbind\\tests\\img\\wires.png",
            "E:\\Programming-Projects\\glbind\\tests\\img\\3.png"
        };

        std::array<Image,3> images;
        for(std::size_t i = 0;i < 3;i++)
            images[i] = load_image(img_pathes[i]);

        std::array<std::unique_ptr<glbind::TextureRGBA<glbind::ColorChannelType::RGBA>>,3> textures;
        for(std::size_t i = 0;i < 3;i++)
        {
            auto& img {images[i]};
            textures[i] = std::make_unique<glbind::TextureRGBA<glbind::ColorChannelType::RGBA>>(img.data.get(),0,0,img.width,img.height);
        }

        glbind::TextureRGB<glbind::ColorChannelType::RGB> frame_tex1(nullptr,0,0,800,600);
        glbind::FrameRBG frame1(frame_tex1);

        glbind::TextureRGB<glbind::ColorChannelType::RGB> frame_tex2(nullptr,0,0,800,600);
        glbind::FrameRBG frame2(frame_tex2);

        rkki::test::TimeRecorder recorder;
        
        std::size_t tex_index {0};
        while(true)
        {
            glbind::Scope([&]()
            {
                // draw obj to frame1
                glbind::Scope(0,0,800,600,[&]()
                {
                    program.use();
                    program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(0.7f,0.5f,1.0f)));
                    frame1.use();
                    textures[tex_index]->bind();
                    vertices.draw();
                });

                // apply kernel effect & draw to frame2
                glbind::Scope(0,0,800,600,[&]()
                {
                    frame2.use();
                    post_kernel_program.use();
                    frame_tex1.bind();
                    post_kernel_program.set_uniform("transform",glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,0.9f,1.0f)));
                    vertices.draw();
                });

                // apply shake effect & draw to screen
                glbind::Scope(0,0,800,600,[&]()
                {
                    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
                    glClear(GL_COLOR_BUFFER_BIT);

                    //post_shake_program.use();
                    frame_tex1.bind();
                    vertices.draw();

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