#include <nadir/core/json.hpp>
#include <charconv>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

namespace nadir::json {

bool Value::is_null() const { return std::holds_alternative<std::nullptr_t>(data); }
bool Value::is_bool() const { return std::holds_alternative<bool>(data); }
bool Value::is_number() const { return std::holds_alternative<double>(data); }
bool Value::is_string() const { return std::holds_alternative<std::string>(data); }
bool Value::is_array() const { return std::holds_alternative<Array>(data); }
bool Value::is_object() const { return std::holds_alternative<Object>(data); }
bool Value::as_bool(bool fallback) const { if (auto p=std::get_if<bool>(&data)) return *p; return fallback; }
double Value::as_number(double fallback) const { if (auto p=std::get_if<double>(&data)) return *p; return fallback; }
std::string Value::as_string(std::string fallback) const {
    if (auto p=std::get_if<std::string>(&data)) return *p;
    if (auto p=std::get_if<double>(&data)) { std::ostringstream out; out.precision(17); out<<*p; return out.str(); }
    if (auto p=std::get_if<bool>(&data)) return *p?"true":"false";
    return fallback;
}
const Array* Value::as_array() const { return std::get_if<Array>(&data); }
const Object* Value::as_object() const { return std::get_if<Object>(&data); }
const Value* Value::get(std::string_view key) const {
    const auto* o=as_object();
    if (!o) return nullptr;
    const auto it=o->find(key);
    return it==o->end()?nullptr:&it->second;
}

class Parser {
public:
    explicit Parser(std::string_view text):text_(text){}

    ParseResult run() {
        skip();
        auto v=value();
        if (!v) return {false,{},pos_,error_};
        skip();
        if (pos_!=text_.size()) return {false,{},pos_,"trailing data"};
        return {true,std::move(*v),pos_,{}};
    }

private:
    std::string_view text_;
    std::size_t pos_{};
    std::string error_;

    void skip() {
        while (pos_<text_.size()) {
            const char c=text_[pos_];
            if (c==' ' || c=='\t' || c=='\r' || c=='\n') ++pos_;
            else break;
        }
    }

    bool consume(char c) {
        skip();
        if (pos_<text_.size() && text_[pos_]==c) { ++pos_; return true; }
        return false;
    }

    bool literal(std::string_view s) {
        if (text_.substr(pos_,s.size())!=s) return false;
        pos_+=s.size();
        return true;
    }

    static void append_utf8(std::string& out, unsigned cp) {
        if (cp<=0x7f) out.push_back(static_cast<char>(cp));
        else if (cp<=0x7ff) {
            out.push_back(static_cast<char>(0xc0 | (cp>>6)));
            out.push_back(static_cast<char>(0x80 | (cp&0x3f)));
        } else if (cp<=0xffff) {
            out.push_back(static_cast<char>(0xe0 | (cp>>12)));
            out.push_back(static_cast<char>(0x80 | ((cp>>6)&0x3f)));
            out.push_back(static_cast<char>(0x80 | (cp&0x3f)));
        } else {
            out.push_back(static_cast<char>(0xf0 | (cp>>18)));
            out.push_back(static_cast<char>(0x80 | ((cp>>12)&0x3f)));
            out.push_back(static_cast<char>(0x80 | ((cp>>6)&0x3f)));
            out.push_back(static_cast<char>(0x80 | (cp&0x3f)));
        }
    }

    static int hex(char c) {
        if (c>='0' && c<='9') return c-'0';
        if (c>='a' && c<='f') return c-'a'+10;
        if (c>='A' && c<='F') return c-'A'+10;
        return -1;
    }

    std::optional<unsigned> unicode_escape() {
        if (pos_+4>text_.size()) return std::nullopt;
        unsigned cp=0;
        for (int i=0;i<4;++i) {
            const int v=hex(text_[pos_++]);
            if (v<0) return std::nullopt;
            cp=(cp<<4)|static_cast<unsigned>(v);
        }
        return cp;
    }

    std::optional<std::string> string() {
        skip();
        if (pos_>=text_.size() || text_[pos_]!='\"') return std::nullopt;
        ++pos_;
        std::string out;
        while (pos_<text_.size()) {
            char c=text_[pos_++];
            if (c=='\"') return out;
            if (c=='\\') {
                if (pos_>=text_.size()) break;
                const char e=text_[pos_++];
                if (e=='\"' || e=='\\' || e=='/') out.push_back(e);
                else if (e=='b') out.push_back('\b');
                else if (e=='f') out.push_back('\f');
                else if (e=='n') out.push_back('\n');
                else if (e=='r') out.push_back('\r');
                else if (e=='t') out.push_back('\t');
                else if (e=='u') {
                    auto cp=unicode_escape();
                    if (!cp) { error_="invalid unicode escape"; return std::nullopt; }
                    if (*cp>=0xd800 && *cp<=0xdbff && pos_+6<=text_.size() && text_[pos_]=='\\' && text_[pos_+1]=='u') {
                        pos_+=2;
                        auto lo=unicode_escape();
                        if (lo && *lo>=0xdc00 && *lo<=0xdfff) *cp=0x10000+((*cp-0xd800)<<10)+(*lo-0xdc00);
                    }
                    append_utf8(out,*cp);
                } else { error_="invalid escape"; return std::nullopt; }
            } else {
                if (static_cast<unsigned char>(c)<0x20) { error_="control character in string"; return std::nullopt; }
                out.push_back(c);
            }
        }
        error_="unterminated string";
        return std::nullopt;
    }

    std::optional<double> number() {
        skip();
        const std::size_t start=pos_;
        if (pos_<text_.size() && text_[pos_]=='-') ++pos_;
        if (pos_>=text_.size()) return std::nullopt;
        if (text_[pos_]=='0') ++pos_;
        else {
            if (text_[pos_]<'1' || text_[pos_]>'9') return std::nullopt;
            while (pos_<text_.size() && text_[pos_]>='0' && text_[pos_]<='9') ++pos_;
        }
        if (pos_<text_.size() && text_[pos_]=='.') {
            ++pos_;
            if (pos_>=text_.size() || text_[pos_]<'0' || text_[pos_]>'9') return std::nullopt;
            while (pos_<text_.size() && text_[pos_]>='0' && text_[pos_]<='9') ++pos_;
        }
        if (pos_<text_.size() && (text_[pos_]=='e' || text_[pos_]=='E')) {
            ++pos_;
            if (pos_<text_.size() && (text_[pos_]=='+' || text_[pos_]=='-')) ++pos_;
            if (pos_>=text_.size() || text_[pos_]<'0' || text_[pos_]>'9') return std::nullopt;
            while (pos_<text_.size() && text_[pos_]>='0' && text_[pos_]<='9') ++pos_;
        }
        const std::string s(text_.substr(start,pos_-start));
        char* end=nullptr;
        const double v=std::strtod(s.c_str(),&end);
        if (!end || *end!='\0' || !std::isfinite(v)) return std::nullopt;
        return v;
    }

    std::optional<Value> array() {
        if (!consume('[')) return std::nullopt;
        Array out;
        skip();
        if (consume(']')) return Value{std::move(out)};
        while (true) {
            auto v=value();
            if (!v) return std::nullopt;
            out.push_back(std::move(*v));
            skip();
            if (consume(']')) break;
            if (!consume(',')) { error_="expected comma in array"; return std::nullopt; }
        }
        return Value{std::move(out)};
    }

    std::optional<Value> object() {
        if (!consume('{')) return std::nullopt;
        Object out;
        skip();
        if (consume('}')) return Value{std::move(out)};
        while (true) {
            auto k=string();
            if (!k) { error_="expected object key"; return std::nullopt; }
            if (!consume(':')) { error_="expected colon"; return std::nullopt; }
            auto v=value();
            if (!v) return std::nullopt;
            out.insert_or_assign(std::move(*k),std::move(*v));
            skip();
            if (consume('}')) break;
            if (!consume(',')) { error_="expected comma in object"; return std::nullopt; }
        }
        return Value{std::move(out)};
    }

    std::optional<Value> value() {
        skip();
        if (pos_>=text_.size()) { error_="unexpected end"; return std::nullopt; }
        if (text_[pos_]=='{') return object();
        if (text_[pos_]=='[') return array();
        if (text_[pos_]=='\"') {
            auto s=string();
            if (!s) return std::nullopt;
            return Value{std::move(*s)};
        }
        if (literal("true")) return Value{true};
        if (literal("false")) return Value{false};
        if (literal("null")) return Value{nullptr};
        auto n=number();
        if (n) return Value{*n};
        error_="invalid value";
        return std::nullopt;
    }
};

ParseResult parse(std::string_view text) { return Parser(text).run(); }

std::optional<std::string> read_text_file(const std::string& path) {
    std::ifstream in(path,std::ios::binary);
    if (!in) return std::nullopt;
    std::ostringstream out;
    out<<in.rdbuf();
    return out.str();
}

}
