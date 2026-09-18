#include <nadir/core/sha256.hpp>
#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nadir::core {

namespace {

constexpr std::array<std::uint32_t, 64> k{
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

std::uint32_t rotr(std::uint32_t x, std::uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

}

std::string sha256(std::span<const std::uint8_t> input) {
    std::vector<std::uint8_t> data(input.begin(), input.end());
    const std::uint64_t bit_len = static_cast<std::uint64_t>(data.size()) * 8;
    data.push_back(0x80);
    while ((data.size() % 64) != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i) data.push_back(static_cast<std::uint8_t>((bit_len >> (i * 8)) & 0xff));

    std::uint32_t h0=0x6a09e667,h1=0xbb67ae85,h2=0x3c6ef372,h3=0xa54ff53a;
    std::uint32_t h4=0x510e527f,h5=0x9b05688c,h6=0x1f83d9ab,h7=0x5be0cd19;

    for (std::size_t off=0; off<data.size(); off+=64) {
        std::array<std::uint32_t,64> w{};
        for (int i=0;i<16;++i) {
            const std::size_t j=off+i*4;
            w[i]=(static_cast<std::uint32_t>(data[j])<<24)|(static_cast<std::uint32_t>(data[j+1])<<16)|(static_cast<std::uint32_t>(data[j+2])<<8)|data[j+3];
        }
        for (int i=16;i<64;++i) {
            const auto s0=rotr(w[i-15],7)^rotr(w[i-15],18)^(w[i-15]>>3);
            const auto s1=rotr(w[i-2],17)^rotr(w[i-2],19)^(w[i-2]>>10);
            w[i]=w[i-16]+s0+w[i-7]+s1;
        }
        auto a=h0,b=h1,c=h2,d=h3,e=h4,f=h5,g=h6,h=h7;
        for (int i=0;i<64;++i) {
            const auto s1=rotr(e,6)^rotr(e,11)^rotr(e,25);
            const auto ch=(e&f)^((~e)&g);
            const auto t1=h+s1+ch+k[i]+w[i];
            const auto s0=rotr(a,2)^rotr(a,13)^rotr(a,22);
            const auto maj=(a&b)^(a&c)^(b&c);
            const auto t2=s0+maj;
            h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
        }
        h0+=a;h1+=b;h2+=c;h3+=d;h4+=e;h5+=f;h6+=g;h7+=h;
    }

    std::ostringstream out;
    out<<std::hex<<std::setfill('0');
    for (auto v:{h0,h1,h2,h3,h4,h5,h6,h7}) out<<std::setw(8)<<v;
    return out.str();
}

std::string sha256(const std::string& data) {
    return sha256(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(data.data()), data.size()));
}

std::string sha256_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return {};
    std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return sha256(data);
}

}
