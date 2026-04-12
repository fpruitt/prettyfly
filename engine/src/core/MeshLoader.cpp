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

    std::vector<float> allPositions;
    std::vector<unsigned int> allIndices;

    for (cgltf_size m = 0; m < data->meshes_count; m++) {
        cgltf_mesh& gltfMesh = data->meshes[m];

        for (cgltf_size p = 0; p < gltfMesh.primitives_count; p++) {
            cgltf_primitive& primitive = gltfMesh.primitives[p];

            // Find position attribute
            cgltf_accessor* posAccessor = nullptr;
            for (cgltf_size a = 0; a < primitive.attributes_count; a++) {
                if (primitive.attributes[a].type == cgltf_attribute_type_position) {
                    posAccessor = primitive.attributes[a].data;
                    break;
                }
            }

            if (!posAccessor) {
                std::cerr << "[MeshLoader] Primitive " << p << " of mesh " << m << " has no positions, skipping\n";
                continue;
            }

            // Track the base vertex offset before appending new verts
            // so indices from this primitive point to the right vertices
            unsigned int baseVertex = (unsigned int)(allPositions.size() / 3);

            // Append positions
            for (cgltf_size v = 0; v < posAccessor->count; v++) {
                float pos[3];
                cgltf_accessor_read_float(posAccessor, v, pos, 3);
                allPositions.push_back(pos[0]);
                allPositions.push_back(pos[1]);
                allPositions.push_back(pos[2]);
            }

            // Append indices, offset by baseVertex
            if (primitive.indices) {
                for (cgltf_size i = 0; i < primitive.indices->count; i++) {
                    allIndices.push_back(baseVertex + (unsigned int)cgltf_accessor_read_index(primitive.indices, i));
                }
            }
            else {
                // No indices — generate them sequentially
                for (unsigned int i = 0; i < (unsigned int)posAccessor->count; i++) {
                    allIndices.push_back(baseVertex + i);
                }
            }
        }
    }

    if (allPositions.empty()) {
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
        allPositions.size() * sizeof(float),
        allPositions.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        allIndices.size() * sizeof(unsigned int),
        allIndices.data(),
        GL_STATIC_DRAW);

    mesh.indexed = true;
    mesh.indexCount = (int)allIndices.size();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cgltf_free(data);

    std::cout << "[MeshLoader] Loaded: " << path
        << " | meshes: " << data->meshes_count
        << " | vertices: " << (allPositions.size() / 3)
        << " | indices: " << allIndices.size() << "\n";

    return mesh;
}
