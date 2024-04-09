#include <format>
#include <hex/api/content_registry.hpp>
#include <hex/api/localization_manager.hpp>
#include <hex/helpers/utils.hpp>
#include <hex/ui/imgui_imhex_extensions.h>
#include <hex/data_processor/node.hpp>
#include <zlib.h>

namespace hex::plugin::data_processor_zlib
{
    class NodeZlibDeflate final : public dp::Node
    {
        static constexpr size_t CHUNK_SIZE = 16384;
    public:
        NodeZlibDeflate() : Node("zziger.data_processor_zlib.nodes.deflate.header",
                                     {
                                         dp::Attribute(dp::Attribute::IOType::In, dp::Attribute::Type::Buffer, "hex.builtin.nodes.common.input"),
                                         dp::Attribute(dp::Attribute::IOType::Out, dp::Attribute::Type::Buffer, "hex.builtin.nodes.common.output")
                                        }) { }

        void process() override
        {
            const std::vector<u8> input = this->getBufferOnInput(0);

            if (input.empty())
                throwNodeError("Input cannot be empty");

            auto data = inflate_data(input);
            this->setBufferOnOutput(1, data);
        }

    private:
        std::vector<u8> inflate_data(std::vector<u8> input)
        {
            const auto length = input.size();
            std::vector<u8> result;

            z_stream stream;
            stream.zalloc = nullptr;
            stream.zfree = nullptr;
            stream.opaque = nullptr;
            stream.avail_in = 0;
            stream.next_in = nullptr;

            int status = inflateInit(&stream);
            if (status != Z_OK)
                throwNodeError("Failed to initialize Zlib");

            stream.avail_in = length;
            stream.next_in = &input[0];

            do {
                u8 temp[CHUNK_SIZE];

                stream.avail_out = CHUNK_SIZE;
                stream.next_out = temp;
                status = inflate(&stream, Z_NO_FLUSH);

                switch (status) {
                case Z_NEED_DICT:
                    throwNodeError(std::format("Provided stream requires zdict (not supported)"));
                case Z_DATA_ERROR:
                    throwNodeError(std::format("Zlib data error: {}", stream.msg == nullptr ? "(null)" : stream.msg));
                case Z_MEM_ERROR:
                    throwNodeError(std::format("Zlib memory error: {}", stream.msg == nullptr ? "(null)" : stream.msg));
                case Z_STREAM_ERROR:
                    throwNodeError(std::format("Zlib stream error: {}", stream.msg == nullptr ? "(null)" : stream.msg));
                default:
                    break;
                }

                const u32 available = CHUNK_SIZE - stream.avail_out;
                result.insert(result.end(), temp, temp + available);
            } while (stream.avail_out == 0);

            inflateEnd(&stream);
            return result;
        }
    };

    void registerDeflateNode()
    {
        ContentRegistry::DataProcessorNode::add<NodeZlibDeflate>("zziger.data_processor_zlib.nodes.decompression", "zziger.data_processor_zlib.nodes.deflate.header");
    }
}