#include "Shader.hpp"
#include "EverEngineCore/Log.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include "../../Platform/Generic/FileSystem.hpp" 

namespace EverEngine
{
    Shader::Shader(const std::unordered_map<GLenum, const char*>& sources)
        : m_id(0)
    {
        m_id = glCreateProgram();
        std::vector<GLuint> shaderIDs;

        for (const auto& [type, path] : sources)
        {
            // Використовуємо FileSystem замість власного load_file
            if (!FileSystem::File::Exists(path))
            {
                LOG_ERROR("ERROR::SHADER::FILE_NOT_FOUND: {}", path);
                continue;
            }

            std::string code = FileSystem::File::ReadText(path);
            if (code.empty())
            {
                LOG_ERROR("ERROR::SHADER::FILE_EMPTY: {}", path);
                continue;
            }

            GLuint shaderID = compile_shader(type, code);
            if (shaderID != 0)
            {
                glAttachShader(m_id, shaderID);
                shaderIDs.push_back(shaderID);
            }
        }

        if (shaderIDs.empty())
        {
            LOG_CRIT("ERROR::SHADER::NO_VALID_SHADERS_COMPILED");
            glDeleteProgram(m_id);
            m_id = 0;
            return;
        }

        glLinkProgram(m_id);
        check_compile_errors(m_id, "PROGRAM");

        for (GLuint id : shaderIDs)
        {
            glDeleteShader(id);
        }

        if (is_valid())
        {
            LOG_INFO("Shader program created successfully (ID: {})", m_id);
        }
    }
    
    Shader::Shader(const std::unordered_map<GLenum, std::string>& sources)
        : m_id(0)
    {
        m_id = glCreateProgram();
        std::vector<GLuint> shaderIDs;

        for (const auto& [type, path] : sources)
        {
            if (!FileSystem::File::Exists(path))
            {
                LOG_ERROR("ERROR::SHADER::FILE_NOT_FOUND: {}", path);
                continue;
            }

            std::string code = FileSystem::File::ReadText(path);
            if (code.empty())
            {
                LOG_ERROR("ERROR::SHADER::FILE_EMPTY: {}", path);
                continue;
            }

            GLuint shaderID = compile_shader(type, code);
            if (shaderID != 0)
            {
                glAttachShader(m_id, shaderID);
                shaderIDs.push_back(shaderID);
            }
        }

        if (shaderIDs.empty())
        {
            LOG_CRIT("ERROR::SHADER::NO_VALID_SHADERS_COMPILED");
            glDeleteProgram(m_id);
            m_id = 0;
            return;
        }

        glLinkProgram(m_id);
        check_compile_errors(m_id, "PROGRAM");

        for (GLuint id : shaderIDs)
        {
            glDetachShader(m_id, id);
            glDeleteShader(id);
        }

        if (is_valid())
        {
            LOG_INFO("Shader program created successfully (ID: {})", m_id);
        }
    }
    
    Shader::~Shader()
    {
        destroy();
    }

    Shader::Shader(Shader&& other) noexcept
        : m_id(other.m_id)
        , m_uniformCache(std::move(other.m_uniformCache))
    {
        other.m_id = 0;
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            destroy();

            m_id = other.m_id;
            m_uniformCache = std::move(other.m_uniformCache);

            other.m_id = 0;
        }

        return *this;
    }

    void Shader::destroy()
    {
        if (m_id != 0)
        {
            glDeleteProgram(m_id);
            m_id = 0;
            m_uniformCache.clear();
        }
    }

    unsigned int Shader::compile_shader(unsigned int type, const std::string& source) const
    {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        std::string typeStr;
        switch (type)
        {
            case GL_VERTEX_SHADER:   typeStr = "VERTEX"; break;
            case GL_FRAGMENT_SHADER: typeStr = "FRAGMENT"; break;
            case GL_GEOMETRY_SHADER: typeStr = "GEOMETRY"; break;
            case GL_COMPUTE_SHADER: typeStr = "COMPUTE"; break;
            case GL_TESS_EVALUATION_SHADER: typeStr = "TESS_EVALUATION"; break;
            case GL_TESS_CONTROL_SHADER: typeStr = "TESS_CONTROL"; break;
            
    #ifdef GL_MESH_SHADER_NV
            case GL_MESH_SHADER_NV: typeStr = "MESH"; break;
    #endif
    #ifdef GL_TASK_SHADER_NV
            case GL_TASK_SHADER_NV: typeStr = "TASK"; break;
    #endif

            default: typeStr = "UNKNOWN"; break;
        }

        check_compile_errors(shader, typeStr);

        return shader;
    }

    void Shader::check_compile_errors(unsigned int shader, const std::string& type) const
    {
        int success;
        char infoLog[1024];

        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                LOG_ERROR("ERROR::SHADER::COMPILATION_ERROR::{}\n{}", type, infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                LOG_ERROR("ERROR::PROGRAM_LINKING_ERROR of type: {}\n{}", type, infoLog);
            }
        }
    }

    GLint Shader::get_uniform_location(const std::string& name) const
    {
        auto it = m_uniformCache.find(name);
        if (it != m_uniformCache.end())
        {
            return it->second;
        }

        GLint location = glGetUniformLocation(m_id, name.c_str());

        if (location == -1)
        {
            LOG_WARN("WARNING::UNIFORM::{}::DOESN'T_EXIST", name);
        }

        m_uniformCache[name] = location;
        return location;
    }

    void Shader::use() const
    {
        if (is_valid())
        {
            glUseProgram(m_id);
        }
        else
        {
            LOG_ERROR("ERROR::SHADER::USE::INVALID_PROGRAM");
        }
    }

    void Shader::set_bool(const std::string& name, bool value) const
    {
        glUniform1i(get_uniform_location(name), static_cast<int>(value));
    }

    void Shader::set_float(const std::string& name, float value) const
    {
        glUniform1f(get_uniform_location(name), value);
    }

    void Shader::set_int(const std::string& name, int value) const
    {
        glUniform1i(get_uniform_location(name), value);
    }

    void Shader::set_vec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(get_uniform_location(name), 1, &value[0]);
    }

    void Shader::set_vec2(const std::string& name, float x, float y) const
    {
        glUniform2f(get_uniform_location(name), x, y);
    }

    void Shader::set_vec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(get_uniform_location(name), 1, &value[0]);
    }

    void Shader::set_vec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(get_uniform_location(name), x, y, z);
    }

    void Shader::set_vec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(get_uniform_location(name), 1, &value[0]);
    }

    void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(get_uniform_location(name), x, y, z, w);
    }

    void Shader::set_mat2(const std::string& name, const glm::mat2& value) const
    {
        glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, &value[0][0]);
    }

    void Shader::set_mat3(const std::string& name, const glm::mat3& value) const
    {
        glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &value[0][0]);
    }

    void Shader::set_mat4(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &value[0][0]);
    }

} // namespace EverEngine