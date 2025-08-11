#pragma once
#include "afp/types.hpp"

namespace afp {
/// Decode audio into mono Mid (and optional Side) deterministically.
/// - **Inputs:** URI path or raw bytes.
/// - **Outputs:** `MidSide { mid, side_opt }`.
/// - **Complexity:** O(N) in samples.
/// - **Edge cases:** Empty/invalid → `Error::DecodeError`/`Error::EmptyAudio`.
[[nodiscard]] Result<MidSide> decode_and_downmix(ByteArray input);

/// Remove DC/rumble with a deterministic HPF (IIR).
/// - **Outputs:** New `PCM`.
/// - **Complexity:** O(N).
[[nodiscard]] Result<PCM> dc_highpass(PCM x, float cutoff_hz);

/// Anti-alias prior to downsampling to `target_sr`.
/// - **Outputs:** New `PCM`.
[[nodiscard]] Result<PCM> pre_resample_lowpass(PCM x, std::uint32_t target_sr);

/// Resample Mid (and Side if present) to `target_sr`.
/// - **Outputs:** New `MidSide` with updated `sr`.
/// - **Edge cases:** If same `sr`, return inputs unchanged.
[[nodiscard]] Result<MidSide> resample_if_needed(
    PCM mid, std::optional<PCM> side_opt, std::uint32_t target_sr);
} // namespace afp
