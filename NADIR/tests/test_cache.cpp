#include <nadir/data/cache.hpp>

#include <filesystem>
#include <fstream>

int main() {
    const auto root = std::filesystem::temp_directory_path() / "nadir-cache-test";
    std::filesystem::remove_all(root);
    nadir::data::CacheStore cache(root.string());
    const nadir::data::Source source{.id="test.source", .format="txt", .license="test-license", .url="https://example.test/data"};
    const auto stored=cache.store(source,"known content",200);
    if (!stored || !cache.verify(source.id)) return 1;
    const auto info=cache.info(source.id);
    if (!info || info->license!="test-license" || info->format!="txt") return 2;
    std::ofstream change(stored->data_path,std::ios::app|std::ios::binary);
    change << "!";
    change.close();
    if (cache.verify(source.id)) return 3;
    std::filesystem::remove_all(root);
    return 0;
}
