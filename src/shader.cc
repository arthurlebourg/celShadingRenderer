#include "shader.hh"

Shader::Shader(std::string &vertex_shader_src, std::string &fragment_shader_src)
{
    int success;

    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    shader_program_ = glCreateProgram();
    std::string vertex_shader_content = read_file(vertex_shader_src);
    std::string fragment_shader_content = read_file(fragment_shader_src);
    char *vertex_shd_src =
        (char *)std::malloc(vertex_shader_content.length() * sizeof(char));
    char *fragment_shd_src =
        (char *)std::malloc(fragment_shader_content.length() * sizeof(char));

    vertex_shader_content.copy(vertex_shd_src, vertex_shader_content.length());
    fragment_shader_content.copy(fragment_shd_src,
                                 fragment_shader_content.length());

    glShaderSource(vertex_shader_, 1, (const GLchar **)&(vertex_shd_src), NULL);
    glCompileShader(vertex_shader_);
    free(vertex_shd_src);

    glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_, 512, NULL, log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << log << std::endl;
        return;
    }

    glShaderSource(fragment_shader_, 1, (const GLchar **)&(fragment_shd_src),
                   NULL);
    glCompileShader(fragment_shader_);
    free(fragment_shd_src);

    glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_, 512, NULL, log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << log << std::endl;
        return;
    }

    glAttachShader(shader_program_, vertex_shader_);
    glAttachShader(shader_program_, fragment_shader_);
    glLinkProgram(shader_program_);

    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader_program_, 512, NULL, log);
        std::cout << "ERROR::SHADER::LINKAGE_FAILED\n" << log << std::endl;
        return;
    }
}

Shader::~Shader()
{
    glDeleteShader(vertex_shader_);
    glDeleteShader(fragment_shader_);
}

void Shader::use()
{
    glUseProgram(shader_program_);
}

void Shader::set_mat4_uniform(const char *name, glm::mat4 mat)
{
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set_mat4_uniform(const char *name, btScalar *mat)
{
    float res[16];
    for (int i = 0; i < 16; i++)
    {
        res[i] = (float)mat[i];
    }
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, res);
}

void Shader::set_vec3_uniform(const char *name, glm::vec3 vec)
{
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

void Shader::bind_texture(std::shared_ptr<Object> obj)
{
    glBindTexture(GL_TEXTURE_2D, obj->get_texture());
    unsigned tex_location =
        glGetUniformLocation(shader_program_, "texture_sampler");
    glUniform1i(tex_location, 0);
    TEST_OPENGL_ERROR();
}
