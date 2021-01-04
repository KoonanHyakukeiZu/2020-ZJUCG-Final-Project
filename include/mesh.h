#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.h>

#include <string>
#include <vector>
#include <stddef.h>
using namespace std;

struct Vertex_Simple{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

struct Vertex_Extra{
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex_Simple>       vertices_simple;
    vector<Vertex_Extra>        vertices_extra;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;
    bool extra = false;

    // constructor simple
    Mesh(vector<Vertex_Simple> vertices_si, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices_simple = vertices_si;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    // constructor extra
    Mesh(vector<Vertex_Simple> vertices_si, vector<Vertex_Extra> vertices_ex, vector<unsigned int> indices, vector<Texture> textures)
    {
        assert(vertices_si.size() == vertices_ex.size());
        this->vertices_simple = vertices_si;
        this->vertices_extra = vertices_ex;
        this->indices = indices;
        this->textures = textures;
        this->extra = true;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    // modified copy constructor
    Mesh(const Mesh & another_mesh)
    {
        this->extra = another_mesh.extra;
        this->vertices_simple = another_mesh.vertices_simple;
        if(this->extra)
        {
            this->vertices_extra = another_mesh.vertices_extra;
        }
        this->indices = another_mesh.indices;
		this->textures = another_mesh.textures;
        setupMesh();
    }
	~Mesh()
	{
		this->vertices_simple.clear();
		if (extra)
			this->vertices_extra.clear();
		this->indices.clear();
		this->textures.clear();
		glDeleteBuffers(1, &VBO);
		if (extra)
			glDeleteBuffers(1, &VBO_extra);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
	}

    // render the mesh
    void Draw(Shader &shader) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
	void cleanUp()
	{
		vertices_simple.clear();
		vertices_extra.clear();
		textures.clear();
		indices.clear();
		glDeleteBuffers(1, &VBO);
		if(extra)
			glDeleteBuffers(1, &VBO_extra);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
	}
private:
    // render data 
    unsigned int VBO, EBO, VBO_extra;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
		if (extra)
			glGenBuffers(1, &VBO_extra);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices_simple.size() * sizeof(Vertex_Simple), &vertices_simple[0], GL_STATIC_DRAW);
        if(extra)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO_extra);
            glBufferData(GL_ARRAY_BUFFER, vertices_extra.size() * sizeof(Vertex_Extra), &vertices_extra[0], GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Simple), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Simple), (void*)offsetof(Vertex_Simple, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_Simple), (void*)offsetof(Vertex_Simple, TexCoords));
        if(extra)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO_extra);
            // vertex tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Extra), (void*)0);
            // vertex bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Extra), (void*)offsetof(Vertex_Extra, Bitangent));

        }

        glBindVertexArray(0);
    }
};
#endif