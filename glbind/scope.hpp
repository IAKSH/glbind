#pragma once

#include <glad/glad.h>
#include <functional>
#include <array>

namespace graphics
{
    class StatusManager
    {
    private:
        struct
        {
            int vao_id;
            int vbo_id;
            int ebo_id;
            int texture_2d_id;
            int framebuffer_id;
            int renderbuffer_id;
            int program_id;
            int cullface_mode;
            int polygon_mode_front;
            int polugon_mode_back;
            int depth_func;
            int stencil_func;
            int stencil_ref;
            int stencil_value_mask;
            int stencil_write_mask;
            int stencil_fail_op;
            int stencil_depth_fail_op;
            int stencil_pass_op;
            int stencil_back_func;
            int stencil_back_ref;
            int stencil_back_value_mask;
            int stencil_back_fail_op;
            int stencil_back_depth_fail_op;
            int stencil_back_pass_op;
            int stencil_clear_value;
            bool stencil_test;
            bool depth_test;
            bool depth_writemask;
            bool blend_test;
            bool scissor_test;
            float point_size;
            float line_width;
            std::array<int,4> viewport;
            std::array<float,2> depth_range;
            //std::array<float,4> clear_color;
            std::array<float,4> blend_color;
        } status_record;

    public:
        StatusManager() noexcept
        {
            int polygon_mode[2];
            glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
            status_record.polygon_mode_front = polygon_mode[0];
            status_record.polugon_mode_back = polygon_mode[1];

            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &status_record.vao_id);
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &status_record.vbo_id);
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &status_record.ebo_id);
            glGetIntegerv(GL_TEXTURE_BINDING_2D,&status_record.texture_2d_id);
            glGetIntegerv(GL_FRAMEBUFFER_BINDING,&status_record.framebuffer_id);
            glGetIntegerv(GL_RENDERBUFFER_BINDING,&status_record.renderbuffer_id);
            glGetIntegerv(GL_CURRENT_PROGRAM,&status_record.program_id);
            glGetIntegerv(GL_VIEWPORT,status_record.viewport.data());
            glGetIntegerv(GL_STENCIL_FUNC,&status_record.stencil_func);
            glGetIntegerv(GL_STENCIL_REF,&status_record.stencil_ref);
            glGetIntegerv(GL_STENCIL_VALUE_MASK,&status_record.stencil_value_mask);
            glGetIntegerv(GL_STENCIL_WRITEMASK,&status_record.stencil_write_mask);
            glGetIntegerv(GL_STENCIL_FAIL,&status_record.stencil_fail_op);
            glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,&status_record.stencil_depth_fail_op);
            glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,&status_record.stencil_pass_op);
            glGetIntegerv(GL_CULL_FACE_MODE,&status_record.cullface_mode);
            glGetBooleanv(GL_DEPTH_TEST,reinterpret_cast<GLboolean*>(&status_record.depth_test));
            glGetBooleanv(GL_STENCIL_TEST,reinterpret_cast<GLboolean*>(&status_record.stencil_test));
            glGetBooleanv(GL_BLEND,reinterpret_cast<GLboolean*>(&status_record.blend_test));
            glGetBooleanv(GL_SCISSOR_TEST,reinterpret_cast<GLboolean*>(&status_record.scissor_test));
            glGetFloatv(GL_POINT_SIZE,&status_record.point_size);
            glGetFloatv(GL_LINE_WIDTH,&status_record.line_width);
            //glGetFloatv(GL_COLOR_CLEAR_VALUE,status_record.clear_color.data());
            glGetIntegerv(GL_DEPTH_FUNC, &status_record.depth_func);
            glGetFloatv(GL_DEPTH_RANGE, status_record.depth_range.data());
            glGetBooleanv(GL_DEPTH_WRITEMASK,reinterpret_cast<GLboolean*>(&status_record.depth_writemask));
            glGetIntegerv(GL_STENCIL_BACK_FUNC, &status_record.stencil_back_func);
            glGetIntegerv(GL_STENCIL_BACK_REF, &status_record.stencil_back_ref);
            glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &status_record.stencil_back_value_mask);
            glGetIntegerv(GL_STENCIL_BACK_FAIL, &status_record.stencil_back_fail_op);
            glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &status_record.stencil_back_depth_fail_op);
            glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &status_record.stencil_back_pass_op);
            glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &status_record.stencil_clear_value);
            glGetFloatv(GL_BLEND_COLOR, status_record.blend_color.data());
        }

        ~StatusManager() noexcept
        {
            glBindVertexArray(status_record.vao_id);
            glBindBuffer(GL_ARRAY_BUFFER, status_record.vbo_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, status_record.ebo_id);
            glBindTexture(GL_TEXTURE_2D, status_record.texture_2d_id);
            glBindFramebuffer(GL_FRAMEBUFFER, status_record.framebuffer_id);
            glBindRenderbuffer(GL_RENDERBUFFER, status_record.renderbuffer_id);
            glUseProgram(status_record.program_id);
            glViewport(status_record.viewport[0], status_record.viewport[1], status_record.viewport[2], status_record.viewport[3]);
            glStencilFunc(status_record.stencil_func, status_record.stencil_ref, status_record.stencil_value_mask);
            glStencilMaskSeparate(GL_FRONT, status_record.stencil_write_mask);
            glStencilMaskSeparate(GL_BACK, status_record.stencil_write_mask);
            glStencilOpSeparate(GL_FRONT, status_record.stencil_fail_op, status_record.stencil_depth_fail_op, status_record.stencil_pass_op);
            glStencilOpSeparate(GL_BACK, status_record.stencil_fail_op, status_record.stencil_depth_fail_op, status_record.stencil_pass_op);
            glCullFace(status_record.cullface_mode);
            glPolygonMode(GL_FRONT, status_record.polygon_mode_front);
            glPolygonMode(GL_BACK, status_record.polugon_mode_back);
            status_record.depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
            status_record.stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
            status_record.blend_test ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
            status_record.scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
            glPointSize(status_record.point_size);
            glLineWidth(status_record.line_width);
            //glClearColor(status_record.clear_color[0], status_record.clear_color[1], status_record.clear_color[2], status_record.clear_color[3]);
            glDepthFunc(status_record.depth_func);
            glDepthRange(status_record.depth_range[0], status_record.depth_range[1]);
            glDepthMask(status_record.depth_writemask);
            glStencilFuncSeparate(GL_BACK, status_record.stencil_back_func, status_record.stencil_back_ref, status_record.stencil_back_value_mask);
            glStencilMaskSeparate(GL_BACK, status_record.stencil_write_mask);
            glStencilOpSeparate(GL_BACK, status_record.stencil_back_fail_op, status_record.stencil_back_depth_fail_op, status_record.stencil_back_pass_op);
            glClearStencil(status_record.stencil_clear_value);
            glBlendColor(status_record.blend_color[0], status_record.blend_color[1], status_record.blend_color[2], status_record.blend_color[3]);
        }
    };

    /**
     * @brief Clear up some opengl status after calling func.
     * 
     * @param func the (lambda) func you wish to call
     */
    inline void Scope(std::function<void()> func)
    {
        StatusManager status_manager;
        func();
    }

    /**
     * @brief Reset opengl view firstly then call func and clear up status
     * 
     * @param x     opengl view X
     * @param y     opengl view Y
     * @param w     opengl view width
     * @param h     opengl view height
     * @param func  the (lambda) func you wish to call
     */
    [[deprecated]] inline void Scope(int x,int y,int w,int h,std::function<void()> func)
    {
        glViewport(x,y,w,h);
        Scope(func);
    }

    /**
     * @brief Reset view and clear background with color then call func and clear up status
     * 
     * @param x     opengl view X
     * @param y     opengl view Y
     * @param w     opengl view width
     * @param h     opengl view height
     * @param r     background color (red)
     * @param g     background color (green)
     * @param b     background color (blue)
     * @param a     background color (alpha)
     * @param func  the (lambda) func you wish to call
     */
    [[deprecated]] inline void Scope(int x,int y,int w,int h,float r,float g,float b,float a,std::function<void()> func)
    {
        glViewport(x,y,w,h);
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        Scope(func);
    }

    void set_viewport(int x,int y,int w,int h) noexcept
    {
        glViewport(x,y,w,h);
    }

    enum class PloygonModes
    {
        Point = GL_POINT,
        Line = GL_LINE,
        Fill = GL_FILL
    };

    inline void set_front_ploygon_model(PloygonModes mode) noexcept
    {
        glPolygonMode(GL_FRONT,static_cast<GLint>(mode));
    }

    inline void set_back_ploygon_model(PloygonModes mode) noexcept
    {
        glPolygonMode(GL_BACK,static_cast<GLint>(mode));
    }

    inline void set_ploygon_model(PloygonModes mode) noexcept
    {
        set_front_ploygon_model(mode);
        set_back_ploygon_model(mode);
    }

    enum class TestFuncType
    {
        Always = GL_ALWAYS,
        Never = GL_NEVER,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        Lequal = GL_LEQUAL,
        Greater = GL_GREATER,
        Notequal = GL_NOTEQUAL,
        Gequal = GL_GEQUAL
    };

    inline void set_depth_mask(bool mask) noexcept
    {
        glDepthMask(mask);
    }

    inline void set_depth_func(TestFuncType func_type) noexcept
    {
        glDepthFunc(static_cast<GLint>(func_type));
    }

    template <TestFuncType func_type>
    inline void enable_depth_test(bool read_only = false) noexcept
    {
        glEnable(GL_DEPTH_TEST);
        set_depth_mask(!read_only);
        set_depth_func(func_type);
    }

    inline void enable_depth_test(bool read_only = false) noexcept
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(!read_only);
        glDepthFunc(GL_LESS);
    }

    inline void disable_depth_test() noexcept
    {
        glDisable(GL_DEPTH_TEST);
    }

    enum class StencilOpType
    {
        Keep = GL_KEEP,
        Zero = GL_ZERO,
        Replace = GL_REPLACE,
        Incr = GL_INCR,
        //IncrWrap = GL_INCR_WRAP,
        Decr = GL_DECR,
        //DecrWarp = GL_DECR_WARP,
        Invert = GL_INVERT
    };

    inline void set_stencil_func(TestFuncType func_type,int ref,unsigned int mask) noexcept
    {
        glStencilFunc(static_cast<GLint>(func_type),ref,mask);
    }

    inline void set_stencil_mask(unsigned int mask) noexcept
    {
        glStencilMask(mask);
    }

    inline void set_stencil_op(StencilOpType fail_op,StencilOpType deep_test_fail_op,StencilOpType pass_op) noexcept
    {
        glStencilOp(static_cast<GLint>(fail_op),static_cast<GLint>(deep_test_fail_op),static_cast<GLint>(pass_op));
    }

    inline void enable_stencil_test() noexcept
    {
        glEnable(GL_STENCIL_TEST);
    }

    inline void disable_stencil_test() noexcept
    {
        glDisable(GL_STENCIL_TEST);
    }

    enum class BlendFuncType
    {
        Zero = GL_ZERO,
        One = GL_ONE,
        SourceColor = GL_SRC_COLOR,
        OneMinusSourceColor = GL_ONE_MINUS_SRC_COLOR,
        DestinationColor = GL_DST_COLOR,
        OneMinusDestinationColor = GL_ONE_MINUS_DST_COLOR,
        SourceAlpha = GL_SRC_ALPHA,
        OneMinusSourceAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DestinationAlpha = GL_DST_ALPHA,
        OneMinusDestinationAlpha = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor = GL_CONSTANT_COLOR,
        OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA
    };

    inline void enable_blend(BlendFuncType src_factor,BlendFuncType dst_factor) noexcept
    {
        glEnable(GL_BLEND);
        glBlendFunc(static_cast<GLenum>(src_factor),static_cast<GLenum>(dst_factor));
    }

    inline void enable_blend(BlendFuncType src_rgb,BlendFuncType dst_rgb,BlendFuncType src_alpha,BlendFuncType dst_alpha) noexcept
    {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(static_cast<GLenum>(src_rgb),static_cast<GLenum>(dst_rgb),static_cast<GLenum>(src_alpha),static_cast<GLenum>(dst_alpha));
    }

    inline void disable_blend() noexcept
    {
        glDisable(GL_BLEND);
    }
}