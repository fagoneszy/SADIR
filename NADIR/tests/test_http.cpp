#include <nadir/data/http.hpp>

int main() {
    const nadir::data::HttpClient client;
    if (client.get("http://example.test/data").ok) return 1;
    if (client.get("https://example.test/\" --output injected").ok) return 2;
    if (client.get("https://example.test/%0aevil").ok) return 3;
    return 0;
}
