#include <Engine/Pipeline.hpp>

//STD
#include <fstream>

//INNER
#include <GameCommon/Logger.hpp>
#include <iostream>

namespace Engine {
    Pipeline::Pipeline(const std::string &pVertFilePath, const std::string &pFragmentFilePath) {
        createGraphicsPipeline(pVertFilePath, pFragmentFilePath);
    }

    std::vector<char> Pipeline::readFile(const std::string& pFilePath) {
        std::fstream file;
        file.open(pFilePath, std::ios::ate | std::ios::binary | std::ios::in);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file : " + pFilePath);
        }

        auto fileSize{static_cast<size_t>(file.tellg())};
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    void Pipeline::createGraphicsPipeline(const std::string &pVertFilePath, const std::string &pFragmentFilePath) {
        auto vertCode{readFile(pVertFilePath)};
        auto fragCode{readFile(pFragmentFilePath)};

        Logger::Debug( OSS << "Shader loaded : " << pVertFilePath << ", Size -> " << vertCode.size() );
        Logger::Debug( OSS << "Shader loaded : " << pFragmentFilePath << ", Size -> " << fragCode.size() );
    }
}

