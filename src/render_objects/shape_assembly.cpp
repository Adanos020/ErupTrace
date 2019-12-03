#include <render_objects/shape_assembly.hpp>

model_assembly_info load_model(const std::string_view in_path)
{
    tinyobj::ObjReader model_loader;

    if (!model_loader.ParseFromFile(in_path.data()))
    {
        throw std::runtime_error(model_loader.Warning() + model_loader.Error());
    }

    model_assembly_info info;
    const tinyobj::attrib_t& attributes = model_loader.GetAttrib();
    for (const tinyobj::shape_t& shape : model_loader.GetShapes())
    {
        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            info.vertices.push_back(vertex{
                position_3D{
                    attributes.vertices[3 * index.vertex_index + 0],
                    attributes.vertices[3 * index.vertex_index + 1],
                    attributes.vertices[3 * index.vertex_index + 2],
                },
                direction_3D{
                    attributes.normals[3 * index.normal_index + 0],
                    attributes.normals[3 * index.normal_index + 1],
                    attributes.normals[3 * index.normal_index + 2],
                },
                index.texcoord_index < 0 ?
                barycentric_2D{ 0.f, 0.f } :
                barycentric_2D{
                    attributes.texcoords[2 * index.texcoord_index + 0],
                    attributes.texcoords[2 * index.texcoord_index + 1],
                },
            });
        }
    }
    return info;
}