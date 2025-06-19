// NOT A PROJECT TO BE TAKEN SERIOUSLY
// Audio: FMOD (https://www.fmod.com/docs)

#pragma once

#include "main_helper.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

/****************************************/
// Audio Context
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
            LOG_MSG("Num")
            fprintf(stdout, "Number of shapes: %zu\n", shapes.size());
            fprintf(stdout, "Number of materials: %zu\n", materials.size());
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
};
} // namespace context