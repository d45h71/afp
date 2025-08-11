#pragma once
#include "afp/types.hpp"

namespace afp {
/// Convert audio input into a sorted list of `(Key, t_anchor)` per spec pipeline.
/// - **Process:** decode → safety → resample → STFT → scale/band/clip → DoG → thresholds →
///   candidates → NMS → pairing → quantize → pack.
/// - **Outputs:** sorted `Array<KeyWithTime>`.
[[nodiscard]] Result<Array<KeyWithTime>> extract_keys_for_track(
    ByteArray input,
    FeatureCfg feat,
    PairingCfg pair,
    KeyLayout layout);

/// Pack `(f_a, f_t, dt_bin)` into a fixed-width `Key` per layout and endianness.
/// - **Preconditions:** fields fit within their bit budgets.
/// - **Failure:** `Error::NumericOverflow` if any field exceeds allocation.
[[nodiscard]] Result<Key> pack_key(std::uint32_t f_a, std::uint32_t f_t,
                                   std::uint32_t dt_bin, KeyLayout layout);
} // namespace afp
