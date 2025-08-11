#pragma once
#include "afp/types.hpp"
#include "afp/kv.hpp"
#include "afp/keys.hpp"

namespace afp {
/// Build a sharded KV index from a manifest of tracks.
/// - **Process:** open KV → per-track extract/group/pack/append → finalize → report.
/// - **Outputs:** `BuildReport`.
/// - **Edge cases:** Empty/short/corrupt tracks → warnings, not fatal by default.
[[nodiscard]] Result<BuildReport> build_db(
    Array<std::pair<std::uint32_t, std::string>> manifest,
    BuildCfg cfg,
    std::string_view kv_path);
} // namespace afp
