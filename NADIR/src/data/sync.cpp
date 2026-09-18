#include <nadir/data/sync.hpp>
#include <nadir/core/time.hpp>

namespace nadir::data {

SyncEngine::SyncEngine(std::string cache_root):cache_(std::move(cache_root)) {}

SyncResult SyncEngine::fetch(const Source& source) const {
    SyncResult out;
    out.source_id=source.id;
    if (!source.syncable) { out.error="source is query/manual/auth gated"; return out; }
    if (source.interval_seconds>0) {
        const auto last=cache_.last_fetch_ns(source.id);
        const auto now=core::now_utc().unix_ns;
        const auto wait=static_cast<std::int64_t>(source.interval_seconds)*1000000000LL;
        if (last && now-*last<wait) {
            out.ok=true;
            out.skipped=true;
            const auto latest=cache_.latest(source.id);
            if (latest) out.path=*latest;
            return out;
        }
    }
    const auto r=http_.get(source.url);
    out.status=r.status;
    if (!r.ok) { out.error=r.error; return out; }
    const auto cached=cache_.store(source,r.body,r.status);
    if (!cached) { out.error="cache write failed"; return out; }
    out.ok=true;
    out.bytes=cached->bytes;
    out.sha256=cached->sha256;
    out.path=cached->data_path;
    return out;
}

}
