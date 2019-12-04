#include <render_objects/shape_assembly.hpp>

#include <external/tiny_obj_loader.h>

static void read_vertices_untextured(const tinyobj::attrib_t& in_attributes,
    const std::vector<tinyobj::shape_t>& in_shapes, model_assembly_info& out_info)
{
    for (const tinyobj::shape_t& shape : in_shapes)
    {
        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            out_info.vertices.push_back(vertex{
                position_3D{
                    in_attributes.vertices[3 * index.vertex_index + 0],
                    in_attributes.vertices[3 * index.vertex_index + 1],
                    in_attributes.vertices[3 * index.vertex_index + 2],
                },
                direction_3D{
                    in_attributes.normals[3 * index.normal_index + 0],
                    in_attributes.normals[3 * index.normal_index + 1],
                    in_attributes.normals[3 * index.normal_index + 2],
                },
                barycentric_2D{ -1.f, -1.f },
            });
        }
    }
}

static void read_vertices_textured(const tinyobj::attrib_t& in_attributes,
    const std::vector<tinyobj::shape_t>& in_shapes, model_assembly_info& out_info)
{
    for (const tinyobj::shape_t& shape : in_shapes)
    {
        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            out_info.vertices.push_back(vertex{
                position_3D{
                    in_attributes.vertices[3 * index.vertex_index + 0],
                    in_attributes.vertices[3 * index.vertex_index + 1],
                    in_attributes.vertices[3 * index.vertex_index + 2],
                },
                direction_3D{
                    in_attributes.normals[3 * index.normal_index + 0],
                    in_attributes.normals[3 * index.normal_index + 1],
                    in_attributes.normals[3 * index.normal_index + 2],
                },
                barycentric_2D{
                    in_attributes.texcoords[2 * index.texcoord_index + 0],
                    in_attributes.texcoords[2 * index.texcoord_index + 1],
                },
            });
        }
    }
}

model_assembly_info load_model(const std::string_view in_path)
{
    tinyobj::ObjReader model_loader;

    if (!model_loader.ParseFromFile(in_path.data()))
    {
        throw std::runtime_error(model_loader.Warning() + model_loader.Error());
    }

    model_assembly_info info;
    const tinyobj::attrib_t& attributes = model_loader.GetAttrib();
    if (attributes.texcoords.empty())
    {
        read_vertices_untextured(attributes, model_loader.GetShapes(), info);
    }
    else
    {
        read_vertices_textured(attributes, model_loader.GetShapes(), info);
    }
    return info;
}