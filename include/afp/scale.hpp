#pragma once
#include "afp/types.hpp"

namespace afp {
/// Convert magnitudes to log/PCEN, crop to [f0..f1], and clip percentiles.
/// - **Preconditions:** `band_max_hz > band_min_hz`.
/// - **Outputs:** `ScaledSpec { val[T,F'], unit, f0_bin, fprime }`.
/// - **Complexity:** O(T * F').
[[nodiscard]] Result<ScaledSpec> scale_and_band(STFTSpec spec, FeatureCfg feat);

/// Optional frequency-only Difference-of-Gaussians enhancement.
/// - **Outputs:** `DogOutput { det, base }`.
/// - **Complexity:** O(T * F').
[[nodiscard]] Result<DogOutput> dog_enhance_freq(
    ScaledSpec scaled, bool use_dog, float sigma1_bins, float sigma2_bins);

/// Separable 1D Gaussian blur along frequency for each row.
/// - **Outputs:** new `Matrix<f32>`.
[[nodiscard]] Result<Matrix<float>> gaussian_blur_freq(
    Matrix<float> m, float sigma_bins);

/// Compute robust percentile clip bounds over all cells.
/// - **Outputs:** `ClipBounds { lo, hi }`.
[[nodiscard]] Result<ClipBounds> percentile_clip_bounds(
    const ScaledSpec& scaled, float p_lo, float p_hi);
} // namespace afp
