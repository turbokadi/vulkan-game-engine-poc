#include <Engine/Model.hpp>

//STD
#include <cassert>
#include <cstring>

namespace Engine {
    Model::Model(Engine::Device& pDevice, const std::vector<Vertex> &pVertices): mDevice(pDevice) {
        createVertexBuffer(pVertices);
    }

    Model::~Model() {
        vkDestroyBuffer(mDevice.device(), mVertexBuffer, nullptr);
        vkFreeMemory(mDevice.device(), mVertexBufferMemory, nullptr);
    }

    void Model::bind(VkCommandBuffer pCommandBuffer) {
        VkBuffer buffers[] = {mVertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, buffers, offsets);
    }

    void Model::draw(VkCommandBuffer pCommandBuffer) {
        vkCmdDraw(pCommandBuffer, mVertexCount, 1, 0, 0);
    }

    void Model::createVertexBuffer(const std::vector<Vertex> &pVertices) {
        mVertexCount = static_cast<uint32_t>(pVertices.size());
        assert(mVertexCount >= 3 && "Vertex count must be at least 3.");
        VkDeviceSize bufferSize = sizeof(pVertices[0]) * mVertexCount;

        mDevice.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             mVertexBuffer,
                             mVertexBufferMemory);

        void* data;
        vkMapMemory(mDevice.device(), mVertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, pVertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(mDevice.device(), mVertexBufferMemory);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindigDescriptions(1);
        bindigDescriptions[0].binding = 0;
        bindigDescriptions[0].stride = sizeof(Vertex);
        bindigDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindigDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributesDescriptions(1);
        attributesDescriptions[0].binding = 0;
        attributesDescriptions[0].location = 0;
        attributesDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributesDescriptions[0].offset = 0;
        return attributesDescriptions;
    }
}
