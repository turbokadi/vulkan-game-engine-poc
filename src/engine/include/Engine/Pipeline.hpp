#ifndef MINECRAFTBUTACTUALLYGOOD_PIPELINE_HPP
#define MINECRAFTBUTACTUALLYGOOD_PIPELINE_HPP

//STD
#include <string>
#include <vector>

namespace Engine {
    class Pipeline {
    public:
        Pipeline(const std::string& pVertFilePath, const std::string& pFragmentFilePath);

    private:
        static std::vector<char> readFile(const std::string& pFilePath);

        void createGraphicsPipeline(const std::string& pVertFilePath, const std::string& pFragmentFilePath);
    };
}

#endif //MINECRAFTBUTACTUALLYGOOD_PIPELINE_HPP
