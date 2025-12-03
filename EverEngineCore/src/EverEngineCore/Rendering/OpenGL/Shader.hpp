#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace EverEngine
{
    class ShaderType {
    public:
        enum class Type {
            Vertex,
            Fragment,
            Geometry,
            Compute,
            TessEval,
            TessControl
        };

        static constexpr GLenum ToGL(Type t) {
            return GLTable[static_cast<size_t>(t)];
        }

    private:
        static constexpr GLenum GLTable[] = {
            GL_VERTEX_SHADER,
            GL_FRAGMENT_SHADER,
            GL_GEOMETRY_SHADER,
            GL_COMPUTE_SHADER,
            GL_TESS_EVALUATION_SHADER,
            GL_TESS_CONTROL_SHADER
        };
    };

    struct GLShaderType{
        static constexpr GLenum Vertex = ShaderType::ToGL(ShaderType::Type::Vertex);
        static constexpr GLenum Fragment = ShaderType::ToGL(ShaderType::Type::Fragment);
        static constexpr GLenum Geometry = ShaderType::ToGL(ShaderType::Type::Geometry);
        static constexpr GLenum Compute = ShaderType::ToGL(ShaderType::Type::Compute);
        static constexpr GLenum TessEval = ShaderType::ToGL(ShaderType::Type::TessEval);
        static constexpr GLenum TessControl = ShaderType::ToGL(ShaderType::Type::TessControl);
    };
    
    class Shader
    {
    public:
        Shader(const std::unordered_map<unsigned int, const char*>& sources);
        Shader(const std::unordered_map<unsigned int, std::string>& sources);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        unsigned int get_program() const { return m_id; }
        bool is_valid() const { return m_id != 0; }

        void use() const;

        void set_bool(const std::string& name, bool value) const;
        void set_float(const std::string& name, float value) const;
        void set_int(const std::string& name, int value) const;
        
        void set_vec2(const std::string& name, const glm::vec2& value) const;
        void set_vec2(const std::string& name, float x, float y) const;
        void set_vec3(const std::string& name, const glm::vec3& value) const;
        void set_vec3(const std::string& name, float x, float y, float z) const;
        void set_vec4(const std::string& name, const glm::vec4& value) const;
        void set_vec4(const std::string& name, float x, float y, float z, float w) const;

        void set_mat2(const std::string& name, const glm::mat2& value) const;
        void set_mat3(const std::string& name, const glm::mat3& value) const;
        void set_mat4(const std::string& name, const glm::mat4& value) const;

    private:
        unsigned int m_id;

        unsigned int compile_shader(unsigned int type, const std::string& source) const;
        void check_compile_errors(unsigned int shader, const std::string& type) const;
        void destroy();

        GLint get_uniform_location(const std::string& name) const;

        mutable std::unordered_map<std::string, GLint> m_uniformCache;
    };

}

#endif