#pragma once
#include "afp/types.hpp"

namespace afp {
/// Compute per-frame thresholds from the Base spectrogram via SNR proxy.
/// - **Outputs:** thresholds `Array<f32>` of length T.
/// - **Complexity:** O(T * F').
[[nodiscard]] Result<Array<float>> per_frame_thresholds(const ScaledSpec& base);

/// 2D local maxima detection on the detection spectrogram (`Det`).
/// - **Outputs:** unfiltered candidate `Peak`s.
/// - **Complexity:** O(T * F').
[[nodiscard]] Result<Array<Peak>> detect_candidates(
    const ScaledSpec& det, std::uint8_t neigh_dt, std::uint8_t neigh_df);

/// Threshold, confirm, per-frame NMS & density control, then sort.
/// - **Outputs:** filtered `Array<Peak>`.
/// - **Edge cases:** If all frames drop, `Error::NoPeaks`.
[[nodiscard]] Result<Array<Peak>> filter_and_nms(
    Array<Peak> cands,
    Array<float> thr,
    const FeatureCfg& feat,
    const ScaledSpec& base);
} // namespace afp
