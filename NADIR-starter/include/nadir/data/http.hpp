#pragma once
#include <string>

namespace nadir::data {

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
