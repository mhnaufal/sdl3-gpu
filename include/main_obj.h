// NOT A PROJECT TO BE TAKEN SERIOUSLY
// OBJ File: https://github.com/tinyobjloader/tinyobjloader/blob/release/tiny_obj_loader.h

#pragma once

#include "main_helper.h"
#include <glm/glm.hpp>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

/****************************************/
// OBJ File Context
/****************************************/
namespace context {
struct ContextObj {
public:
    tinyobj::attrib_t attrib{};
    std::vector<tinyobj::shape_t> shapes{};
    std::vector<tinyobj::material_t> materials{};
    std::string file{};
    bool is_ready{false};
    std::string err{};
    std::string warn{};
    std::vector<glm::vec3> positions{};
    std::vector<glm::vec2> uvs{};
    std::vector<std::vector<int>> faces{};

    ContextObj(std::string path) : file(path) {
        is_ready = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str());
        if (!warn.empty()) {
            CHECK_ERROR("Warning while loading OBJ file", warn.c_str());
        }
        
        if (!err.empty()) {
            CHECK_ERROR("Error while loading OBJ file", err.c_str());
        }
        
        if (!is_ready) {
            CHECK_ERROR("Failed to load OBJ file\n", "");
        } else {
            fprintf(stdout, "Number of shapes: %zu\n", shapes.size());
            fprintf(stdout, "Number of materials: %zu\n", materials.size());

            positions = get_all_positions();
            uvs = get_all_uvs();
            faces = get_all_faces();

            // for (const auto& shape : shapes) {
            //         std::cout << "Processing shape: " << shape.name << std::endl;
                    
            //         // Get face information
            //         size_t index_offset = 0;
            //         for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            //             int fv = shape.mesh.num_face_vertices[f]; // Number of vertices in this face (typically 3 for triangles)
                        
            //             std::cout << "\nFace " << f << " (vertices: " << fv << "):" << shape.mesh.num_face_vertices.size() << std::endl;
                        
            //             // For each vertex in the face
            //             for (size_t v = 0; v < fv; v++) {
            //                 tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                            
            //                 // --- Get Position ---
            //                 float vx = attrib.vertices[3 * idx.vertex_index + 0];
            //                 float vy = attrib.vertices[3 * idx.vertex_index + 1];
            //                 float vz = attrib.vertices[3 * idx.vertex_index + 2];
                            
            //                 std::cout << "  Vertex " << v << " position: (" 
            //                         << vx << ", " << vy << ", " << vz << ")";
                            
            //                 // --- Get UV Coordinates (if available) ---
            //                 if (idx.texcoord_index >= 0) {
            //                     float u = attrib.texcoords[2 * idx.texcoord_index + 0];
            //                     float v = attrib.texcoords[2 * idx.texcoord_index + 1];
            //                     std::cout << "  UV: (" << u << ", " << v << ")";
            //                 } else {
            //                     std::cout << "  UV: (none)";
            //                 }
                            
            //                 std::cout << std::endl;
            //             }
                        
            //             index_offset += fv;
            //         }
            //     }
        }
    }

    ~ContextObj() {
        if (is_ready) {
            attrib.vertices.clear();
            attrib.normals.clear();
            attrib.texcoords.clear();
            shapes.clear();
            materials.clear();
        }
    }

private:
    auto get_all_positions() -> std::vector<glm::vec3> {
        positions.reserve(attrib.vertices.size() / 3);
        for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
            positions.emplace_back(
                attrib.vertices[i],
                attrib.vertices[i + 1],
                attrib.vertices[i + 2]);
        }
        return positions;
    }

    auto get_all_uvs() -> std::vector<glm::vec2> {
        uvs.reserve(attrib.texcoords.size() / 2);
        for (size_t i = 0; i < attrib.texcoords.size(); i += 2) {
            uvs.emplace_back(
                attrib.texcoords[i],
                attrib.texcoords[i + 1]);
        }
        return uvs;
    }

    auto get_all_faces() -> std::vector<std::vector<int>> {
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                if (index.vertex_index >= 0) {
                    faces.push_back({index.vertex_index});
                }
            }
        }
        return faces;
    }
};
} // namespace context