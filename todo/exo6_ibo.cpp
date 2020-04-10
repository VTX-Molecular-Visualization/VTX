#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <cstddef>
#include <vector>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>

using namespace glimac;

const float PIf = 3.14159265358979f;

const GLuint VERTEX_ATTR_POSITION = 0;
const GLuint VERTEX_ATTR_COLOR = 1;

struct Vertex2DColor
{
    glm::vec2 position;
    glm::vec3 color;
    
    Vertex2DColor() {}
    Vertex2DColor(const glm::vec2 &p, const glm::vec3 &c) 
        : position(p), color(c) {}
    Vertex2DColor(const Vertex2DColor &v)
        : position(v.position), color(v.color) {}
};

int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(800, 600, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    std::cout << "Loading and compiling shaders" << std::endl;
    // Load and compile shaders
    FilePath applicationPath(argv[0]);
    Program program = loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl",
                                applicationPath.dirPath() + "shaders/triangle.fs.glsl");
    program.use();
    std::cout << "Compiled" << std::endl;
    
    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    
    // Create VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    const int nbTriangles = 300;
    const float radius = 1.f;
    const float twoPi_nbTriangles = 2.f * PIf / nbTriangles;
    
    
    // Init triangle data    
    std::cout << "Init triangles data" << std::endl;
    std::vector<Vertex2DColor> vertices;
    const Vertex2DColor center(glm::vec2(0.f, 0.f), glm::vec3(1.f,0.f,0.f));
    
    vertices.push_back(center);        
    for (int i = 0; i < nbTriangles; i++)
    {
        const float x = center.position.x + (radius * cosf(i * twoPi_nbTriangles));
        const float y = center.position.y + (radius * sinf(i * twoPi_nbTriangles));
        vertices.push_back(Vertex2DColor(glm::vec2(x, y), glm::vec3(0.f,0.f,1.f)));
    }
    vertices.push_back(vertices[1]);
    
    // Load data
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2DColor), vertices.data(), GL_STATIC_DRAW);
    
    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    std::cout << "Init IBO" << std::endl;
    // Create IBO
    GLuint ibo;
    // Bind IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    
    std::vector<uint32_t> indices;
    // Create indices array
    for (int i = 1; i < nbTriangles + 1; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.back() = 1;
    // Fill IBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    // Unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    
    // => On bind l'IBO sur GL_ELEMENT_ARRAY_BUFFER; puisqu'un VAO est actuellement bindé,
    // cela a pour effet d'enregistrer l'IBO dans le VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Activate attribute position
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glVertexAttribPointer(
        VERTEX_ATTR_POSITION, // Id attribute
        2, // # component
        GL_FLOAT, // type component
        GL_FALSE, // not normalized
        sizeof(Vertex2DColor), // size vertex
        (void *)offsetof(Vertex2DColor, position) 
    );
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
    glVertexAttribPointer(
        VERTEX_ATTR_COLOR, // Id attribute
        3, // # component
        GL_FLOAT, // type component
        GL_FALSE, // not normalized
        sizeof(Vertex2DColor), // size vertex
        (void *)offsetof(Vertex2DColor, color)
    );
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Application loop:
    bool done = false;
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }
    
        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        glClear(GL_COLOR_BUFFER_BIT);
        // "vao" devient le VAO courant, OpenGL l'utilisera lors du dessin pour avoir des informations sur les sommets
        glBindVertexArray(vao);
        
       // => On utilise glDrawElements à la place de glDrawArrays
        // Cela indique à OpenGL qu'il doit utiliser l'IBO enregistré dans le VAO
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
        // Plus de VAO courant:
        glBindVertexArray(0);
        
        // Update the display
        windowManager.swapBuffers();
    }    
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

    return EXIT_SUCCESS;
}
