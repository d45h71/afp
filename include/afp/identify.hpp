#pragma once
#include "afp/types.hpp"

namespace afp {
/// Identify the best-matching track and offset for a query audio clip.
/// - **Process:** extract → fetch/parse → vote → select → coverage/entropy gates.
/// - **Outputs:** `IdentifyResult::Match` or `IdentifyResult::NoMatch`.
[[nodiscard]] Result<IdentifyResult> identify_audio(
    ByteArray query_input,
    IdentifyCfg cfg,
    std::string_view kv_path);
} // namespace afp
