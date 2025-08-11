#pragma once
#include "afp/types.hpp"
#include <map>

namespace afp {
/// Deterministic monotonic calibration of confidence into [0,1].
/// - **Inputs:** votes peak, coverage, entropy.
/// - **Outputs:** calibrated score.
[[nodiscard]] float calibrate_confidence(std::uint32_t votes_peak,
                                         float coverage, float entropy);

/// Convert offset bin to seconds: `sec = off_bin * delta_bin_frames * hop / sr`.
/// - **Outputs:** seconds as `f64`.
[[nodiscard]] double bin_to_seconds(std::int32_t off_bin, std::uint32_t hop,
                                    std::uint32_t sr,
                                    std::uint16_t delta_bin_frames);

/// Group anchor times by key and return per-key sorted-unique times.
/// - **Outputs:** map `Key → Array<u32>`.
[[nodiscard]] std::map<Key, Array<std::uint32_t>> group_times_by_key(
    const Array<KeyWithTime>& pairs);

/// Possibly compress value bytes (opaque policy).
/// - **Outputs:** new `ByteArray`.
[[nodiscard]] ByteArray maybe_compress(ByteArray bytes, const char* algo);

/// Observe postings length into a small histogram (implementation-defined).
void observe_hotkey_histogram(Array<std::uint32_t>& hist, std::size_t len);

/// Render histogram to a public array for the report.
/// - **Outputs:** `Array<u32>`.
[[nodiscard]] Array<std::uint32_t> render_histogram(
    const Array<std::uint32_t>& hist);

/// Collect non-fatal warnings accumulated during build.
/// - **Outputs:** `Array<String>`.
[[nodiscard]] Array<std::string> collect_warnings();

/// Estimate total frames for the given media and feature config.
/// - **Outputs:** frame count `u32`.
[[nodiscard]] std::uint32_t estimate_frames(std::string_view uri,
                                            const FeatureCfg& feat);

/// Compute CRC64 over canonicalized audio content.
/// - **Outputs:** 64-bit checksum.
[[nodiscard]] std::uint64_t crc64_of_uri(std::string_view uri);

/// Derive layout version byte from `KeyLayout`.
/// - **Outputs:** version nibble/byte as `u8`.
[[nodiscard]] std::uint8_t derive_version(const KeyLayout& layout);

/// Histogram `H[dF]` for `dF = |f_t - f_a|` in `[0..F'-1]`.
/// - **Outputs:** `Array<u32>` of length F'.
[[nodiscard]] Array<std::uint32_t> histogram_abs_delta_f(
    const Array<Peak>& peaks, std::uint16_t fprime);

/// Deterministic stable sort by `(t_anchor, key)` for returned keys.
/// - **Outputs:** newly sorted vector.
[[nodiscard]] Array<KeyWithTime> stable_sort_by(Array<KeyWithTime> arr);
} // namespace afp
