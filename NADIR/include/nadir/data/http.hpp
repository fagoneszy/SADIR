#pragma once
#include <cstddef>
#include <string>

namespace nadir::data {

inline constexpr std::size_t max_http_body_bytes = 64U * 1024U * 1024U;

struct HttpResponse {
    bool ok{};
    long status{};
    std::string body;
    std::string error;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    HttpResponse get(const std::string& url) const;
    bool available() const;
};

}
