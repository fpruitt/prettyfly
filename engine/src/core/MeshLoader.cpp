#include "../vendor/cgltf.h"

#include "MeshLoader.h"
#include <iostream>
#include <vector>


Mesh loadGLTF(const std::string& path)
{
    Mesh mesh;

    cgltf_options options = {};
    cgltf_data* data = nullptr;

    cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
    if (result != cgltf_result_success) {
        std::cerr << "[MeshLoader] Failed to parse GLTF file: " << path << std::endl;
        return mesh;
    }

    result = cgltf_load_buffers(&options, data, path.c_str());
    if (result != cgltf_result_success || !data) {
        std::cerr << "[MeshLoader] Failed to load buffers: " << path << std::endl;
        cgltf_free(data);
        return mesh;
    }

    std::vector<float> vertexData;
    std::vector<unsigned int> allIndices;
    bool anyNormals = false;

    for (cgltf_size m = 0; m < data->meshes_count; m++)
    {
        cgltf_mesh& gltfMesh = data->meshes[m];

        for (cgltf_size p = 0; p < gltfMesh.primitives_count; p++)
        {
            cgltf_primitive& primitive = gltfMesh.primitives[p];

            cgltf_accessor* posAccessor = nullptr;
            cgltf_accessor* normalAccessor = nullptr;

            for (cgltf_size a = 0; a < primitive.attributes_count; a++) {
                if (primitive.attributes[a].type == cgltf_attribute_type_position)
                {
                    posAccessor = primitive.attributes[a].data;
                }
                    
                if (primitive.attributes[a].type == cgltf_attribute_type_normal)
                {
                    normalAccessor = primitive.attributes[a].data;
                }
            }

            if (!posAccessor) continue;
            if (normalAccessor) anyNormals = true;

            unsigned int baseVertex = (unsigned int)(vertexData.size() / 6);

            for (cgltf_size v = 0; v < posAccessor->count; v++) {
                // Position
                float pos[3] = {};
                cgltf_accessor_read_float(posAccessor, v, pos, 3);
                vertexData.push_back(pos[0]);
                vertexData.push_back(pos[1]);
                vertexData.push_back(pos[2]);

                // Normal (or zero if not present)
                float normal[3] = { 0.0f, 1.0f, 0.0f };
                if (normalAccessor)
                {
                    cgltf_accessor_read_float(normalAccessor, v, normal, 3);
                }
                vertexData.push_back(normal[0]);
                vertexData.push_back(normal[1]);
                vertexData.push_back(normal[2]);
            }

            if (primitive.indices)
            {
                for (cgltf_size i = 0; i < primitive.indices->count; i++)
                {
                    allIndices.push_back(baseVertex + (unsigned int)cgltf_accessor_read_index(primitive.indices, i));
                }
            }
            else
            {
                for (unsigned int i = 0; i < (unsigned int)posAccessor->count; i++)
                {
                    allIndices.push_back(baseVertex + i);
                }
            }

            std::cout << "Mesh " << m << " primitive " << p
                << " | baseVertex: " << baseVertex
                << " | verts: " << posAccessor->count
                << " | indices: " << (primitive.indices ? primitive.indices->count : 0)
                << " | hasNormals: " << (normalAccessor ? "yes" : "no") << "\n";
        }
    }

    if (vertexData.empty())
    {
        std::cerr << "[MeshLoader] No geometry found in: " << path << "\n";
        cgltf_free(data);
        return mesh;
    }

    // Upload to GPU
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(float),
        vertexData.data(),
        GL_STATIC_DRAW);

    // Position: location 0, offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal: location 1, offset 3 floats
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        allIndices.size() * sizeof(unsigned int),
        allIndices.data(),
        GL_STATIC_DRAW);

    mesh.indexed = true;
    mesh.indexCount = (int)allIndices.size();
    mesh.hasNormals = anyNormals;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cgltf_free(data);

    std::cout << "[MeshLoader] Loaded: " << path
        << " | vertices: " << (vertexData.size() / 6)
        << " | indices: " << allIndices.size()
        << " | normals: " << (anyNormals ? "yes" : "no") << "\n";

    return mesh;
}
