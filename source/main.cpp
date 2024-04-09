#include <hex/plugin.hpp>
#include <content/nodes.h>
#include <hex/api/content_registry.hpp>
#include <romfs/romfs.hpp>

IMHEX_PLUGIN_SETUP("ImHex Data processor Zlib", "zziger", "Adds Zlib nodes to the data processor view") {
    hex::log::debug("Using romfs: '{}'", romfs::name());
    for (auto &path : romfs::list("lang"))
        hex::ContentRegistry::Language::addLocalization(nlohmann::json::parse(romfs::get(path).string()));

    hex::plugin::data_processor_zlib::registerDeflateNode();
}