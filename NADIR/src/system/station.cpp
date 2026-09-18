#include <nadir/system/station.hpp>
#include <thread>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <vector>
#else
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif

namespace nadir::system {

StationSnapshot station_snapshot() {
    StationSnapshot out;
    out.logical_cpu_count=std::thread::hardware_concurrency();
#ifdef _WIN32
    char name[256]{};
    DWORD n=sizeof(name);
    if (GetComputerNameA(name,&n)) out.hostname.assign(name,n);
#if defined(_M_X64) || defined(__x86_64__)
    out.architecture="x86_64";
#elif defined(_M_ARM64) || defined(__aarch64__)
    out.architecture="aarch64";
#else
    out.architecture="unknown";
#endif
    out.os="Windows";
    out.uptime_seconds=GetTickCount64()/1000ULL;
    MEMORYSTATUSEX m{};
    m.dwLength=sizeof(m);
    if (GlobalMemoryStatusEx(&m)) {
        out.memory_total_bytes=m.ullTotalPhys;
        out.memory_available_bytes=m.ullAvailPhys;
    }
#else
    char host[256]{};
    if (gethostname(host,sizeof(host))==0) out.hostname=host;
    utsname u{};
    if (uname(&u)==0) {
        out.os=std::string(u.sysname)+" "+u.release;
        out.architecture=u.machine;
    }
    struct sysinfo s{};
    if (sysinfo(&s)==0) {
        out.uptime_seconds=static_cast<std::uint64_t>(s.uptime);
        const auto unit=static_cast<std::uint64_t>(s.mem_unit);
        out.memory_total_bytes=static_cast<std::uint64_t>(s.totalram)*unit;
        out.memory_available_bytes=static_cast<std::uint64_t>(s.freeram+s.bufferram)*unit;
    }
#endif
    return out;
}

std::vector<InterfaceAddress> interface_addresses() {
    std::vector<InterfaceAddress> out;
#ifdef _WIN32
    ULONG size=15000;
    std::vector<unsigned char> buffer(size);
    auto* aa=reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
    ULONG rc=GetAdaptersAddresses(AF_UNSPEC,GAA_FLAG_INCLUDE_PREFIX,nullptr,aa,&size);
    if (rc==ERROR_BUFFER_OVERFLOW) {
        buffer.resize(size);
        aa=reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
        rc=GetAdaptersAddresses(AF_UNSPEC,GAA_FLAG_INCLUDE_PREFIX,nullptr,aa,&size);
    }
    if (rc!=NO_ERROR) return out;
    for (auto* a=aa;a;a=a->Next) {
        const bool up=a->OperStatus==IfOperStatusUp;
        for (auto* u=a->FirstUnicastAddress;u;u=u->Next) {
            if (!u->Address.lpSockaddr) continue;
            char buf[INET6_ADDRSTRLEN]{};
            std::string family;
            if (u->Address.lpSockaddr->sa_family==AF_INET) {
                auto* p=&reinterpret_cast<sockaddr_in*>(u->Address.lpSockaddr)->sin_addr;
                inet_ntop(AF_INET,p,buf,sizeof(buf));
                family="IPv4";
            } else if (u->Address.lpSockaddr->sa_family==AF_INET6) {
                auto* p=&reinterpret_cast<sockaddr_in6*>(u->Address.lpSockaddr)->sin6_addr;
                inet_ntop(AF_INET6,p,buf,sizeof(buf));
                family="IPv6";
            } else continue;
            out.push_back({a->AdapterName,family,buf,up,a->IfType==IF_TYPE_SOFTWARE_LOOPBACK});
        }
    }
#else
    ifaddrs* list=nullptr;
    if (getifaddrs(&list)!=0) return out;
    for (auto* p=list;p;p=p->ifa_next) {
        if (!p->ifa_addr) continue;
        const int af=p->ifa_addr->sa_family;
        if (af!=AF_INET && af!=AF_INET6) continue;
        char buf[INET6_ADDRSTRLEN]{};
        const void* addr=af==AF_INET?static_cast<void*>(&reinterpret_cast<sockaddr_in*>(p->ifa_addr)->sin_addr):static_cast<void*>(&reinterpret_cast<sockaddr_in6*>(p->ifa_addr)->sin6_addr);
        if (!inet_ntop(af,addr,buf,sizeof(buf))) continue;
        out.push_back({p->ifa_name,af==AF_INET?"IPv4":"IPv6",buf,(p->ifa_flags&IFF_UP)!=0,(p->ifa_flags&IFF_LOOPBACK)!=0});
    }
    freeifaddrs(list);
#endif
    return out;
}

}
