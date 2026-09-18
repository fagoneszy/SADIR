#include <nadir/data/http.hpp>
#include <array>
#include <cstdio>
#include <cstdlib>

namespace nadir::data {

static std::string quote(const std::string& s) {
#ifdef _WIN32
    std::string out="\"";
    for (char c:s) {
        if (c=='\"') out+="\\\"";
        else out+=c;
    }
    out+='\"';
    return out;
#else
    std::string out="'";
    for (char c:s) {
        if (c=='\'') out+="'\\''";
        else out+=c;
    }
    out+='\'';
    return out;
#endif
}

HttpClient::HttpClient() = default;
HttpClient::~HttpClient() = default;

HttpResponse HttpClient::get(const std::string& url) const {
    HttpResponse r;
    const std::string cmd="curl -L --fail --silent --show-error --max-time 120 --user-agent \"NADIR/0.2 public-data-client\" --write-out \"\\nNADIR_HTTP:%{http_code}\" "+quote(url);
#ifdef _WIN32
    FILE* pipe=_popen(cmd.c_str(),"rb");
#else
    FILE* pipe=popen(cmd.c_str(),"r");
#endif
    if (!pipe) { r.error="curl process failed"; return r; }
    std::array<char,8192> buffer{};
    while (true) {
        const auto n=std::fread(buffer.data(),1,buffer.size(),pipe);
        if (n>0) r.body.append(buffer.data(),n);
        if (n<buffer.size()) break;
    }
#ifdef _WIN32
    const int code=_pclose(pipe);
#else
    const int code=pclose(pipe);
#endif
    const std::string marker="\nNADIR_HTTP:";
    const auto p=r.body.rfind(marker);
    if (p!=std::string::npos) {
        const auto status=r.body.substr(p+marker.size());
        r.body.resize(p);
        try { r.status=std::stol(status); } catch (...) { r.status=0; }
    }
    r.ok=code==0 && r.status>=200 && r.status<300;
    if (!r.ok) r.error="curl/http failure";
    return r;
}

bool HttpClient::available() const {
#ifdef _WIN32
    return std::system("curl --version >NUL 2>&1")==0;
#else
    return std::system("curl --version >/dev/null 2>&1")==0;
#endif
}

}
