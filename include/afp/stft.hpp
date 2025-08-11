#pragma once
#include "afp/types.hpp"

namespace afp {
/// Compute magnitude STFT (optionally reassigned).
/// - **Inputs:** `PCM`, Hann window, `fft=frame_size`, `hop=hop_size`.
/// - **Outputs:** `STFTSpec { mag[T,K], sr, fft, hop, unit="linear" }`.
/// - **Complexity:** O(T * fft log fft).  **Edge:** T==0 → `Error::NoFrames`.
[[nodiscard]] Result<STFTSpec> stft_magnitude(PCM mid, FeatureCfg feat);
} // namespace afp
