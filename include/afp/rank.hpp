#pragma once
#include "afp/types.hpp"
#include "afp/kv.hpp"

namespace afp {
/// Accumulate votes for `(track_id, off_bin)` across all query keys.
/// - **Outputs:** `Map<(u32,i32), u32>` vote counts.
/// - **Complexity:** linear in emitted anchors.
[[nodiscard]] Result<Map<std::tuple<std::uint32_t, std::int32_t>,
                         std::uint32_t>> vote_offsets(
    const Array<KeyWithTime>& query_keys,
    const KVHandle& kvh,
    const PairingCfg& pair,
    const KeyLayout& layout);

/// Peak/compactness stats for the winning mode.
struct BestStats {
  /// Tallest bin height.
  std::uint32_t peak{};
  /// Interquartile range (bins) around the mode.
  float iqr_bins{};
};

/// Best `(track, offset_bin)` by peak height and compactness.
/// - **Outputs:** `{track_id, off_bin, stats:{peak, iqr_bins}}`.
struct BestByVotes {
  /// Winning track identifier.
  std::uint32_t track_id{};
  /// Winning offset bin.
  std::int32_t off_bin{};
  /// Stats (peak height, IQR in bins).
  BestStats stats;
};

/// Select the best `(track, off_bin)` pair given the vote histogram.
/// - **Outputs:** `BestByVotes`.
[[nodiscard]] Result<BestByVotes> select_best_by_votes(
    const Map<std::tuple<std::uint32_t, std::int32_t>, std::uint32_t>& votes);

/// Fraction of query frames that contributed ≥1 vote to the winner.
/// - **Outputs:** coverage in `[0,1]`.
[[nodiscard]] Result<float> frame_coverage(
    std::uint32_t best_track,
    std::int32_t best_off_bin,
    const Array<KeyWithTime>& query_keys,
    const KVHandle& kvh,
    const PairingCfg& pair);

/// Shannon entropy (bits) of the offset histogram around a window.
/// - **Outputs:** entropy value.
[[nodiscard]] Result<float> histogram_entropy(
    const Map<std::int32_t, std::uint32_t>& track_votes,
    const Array<std::int32_t>& window_bins);

/// Project per-(track,off_bin) votes to a histogram for one track.
/// - **Outputs:** `Map<off_bin, count>`.
[[nodiscard]] Map<std::int32_t, std::uint32_t> project_track_hist(
    const Map<std::tuple<std::uint32_t, std::int32_t>, std::uint32_t>& votes,
    std::uint32_t track_id);

/// Construct a window of bins around the winning offset bin.
/// - **Outputs:** contiguous list of off_bins to evaluate.
[[nodiscard]] Array<std::int32_t> window_around(std::int32_t off_bin);
} // namespace afp
