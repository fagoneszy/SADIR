#include <nadir/data/http.hpp>

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::string url = argc == 2 ? argv[1] : "https://example.com";
    const nadir::data::HttpClient client;
    if (!client.available()) { std::cerr << "curl unavailable\n"; return 1; }
    const auto response = client.get(url);
    if (!response.ok || response.status < 200 || response.status >= 300 || response.body.empty()) {
        std::cerr << "network gate failed: " << response.error << " status=" << response.status << "\n";
        return 2;
    }
    std::cout << "NETWORK PASS " << response.status << " " << response.body.size() << " B\n";
    return 0;
}
