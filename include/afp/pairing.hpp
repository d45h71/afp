#pragma once
#include "afp/types.hpp"

namespace afp {
/// For given anchor, select up to `max_targets_per_anchor` targets in [dt_min, dt_max].
/// - **Inputs:** `peaks` sorted by (t,f), `anchor_idx`.
/// - **Outputs:** `Array<Peak>` targets (k ≤ max).
/// - **Complexity:** O(W log W) per anchor.
[[nodiscard]] Result<Array<Peak>> select_targets(
    const Array<Peak>& peaks,
    std::uint32_t anchor_idx,
    const PairingCfg& pair,
    std::uint16_t fprime);

/// Same as `select_targets` but with a precomputed |Δf| histogram.
/// - **Outputs:** `Array<Peak>` targets.
/// - **Complexity:** O(W log W) per anchor.
[[nodiscard]] Result<Array<Peak>> select_targets_with_hist(
    const Array<Peak>& peaks,
    std::uint32_t anchor_idx,
    const PairingCfg& pair,
    const Array<std::uint32_t>& df_hist);

/// Quantize Δt (frames) into a Δt_bin clamped to bit budget.
/// - **Outputs:** `u32` Δt_bin.
/// - **Range:** [0 .. (2^bits_dt - 1)].
[[nodiscard]] Result<std::uint32_t> quantize_dt(std::uint32_t dt_frames,
                                                std::uint16_t delta_bin_frames,
                                                std::uint8_t bits_dt);
} // namespace afp
