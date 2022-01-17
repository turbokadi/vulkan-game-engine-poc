#ifndef MINECRAFTBUTACTUALLYGOOD_MODEL_HPP
#define MINECRAFTBUTACTUALLYGOOD_MODEL_HPP

//GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//INNER
#include <Engine/Device.hpp>

namespace Engine {
    class Model {
    public:
        struct Vertex {
            glm::vec2 position;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        Model(Engine::Device& pDevice, const std::vector<Vertex>& pVertices);
        ~Model();

        Model(const Model&) = delete;
        void operator=(const Model&) = delete;

        void bind(VkCommandBuffer pCommandBuffer);
        void draw(VkCommandBuffer pCommandBuffer);

    private:
        void createVertexBuffer(const std::vector<Vertex>& pVertices);

        Engine::Device& mDevice;
        VkBuffer mVertexBuffer;
        VkDeviceMemory mVertexBufferMemory;
        uint32_t  mVertexCount;
    };
}


#endif //MINECRAFTBUTACTUALLYGOOD_MODEL_HPP
