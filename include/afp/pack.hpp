#pragma once
#include "afp/types.hpp"
#include <optional>

namespace afp {
/// Pack one (key, track) list of anchor times into bytes (LEB128 for varints).
/// - **Outputs:** `ByteArray`.
/// - **Complexity:** O(n) for `n = times_sorted.len()`.
[[nodiscard]] Result<ByteArray> pack_posting_block(
    std::uint32_t track_id, const Array<std::uint32_t>& times_sorted);

/// Iterator over anchors parsed from concatenated posting blocks.
struct PostingIter {
  /// Implementation-private placeholder.
  void* _priv{nullptr};
  /// Return next `(track_id, t_anchor)` or `std::nullopt` at end.
  std::optional<Anchor> next();
};

/// Parse concatenated posting blocks to a stream of `(track_id, t_anchor)`.
/// - **Outputs:** `PostingIter`.
/// - **Failure:** `Error::IntegrityError` for malformed/truncated input.
[[nodiscard]] Result<PostingIter> parse_posting_blocks(ByteArray buf);
} // namespace afp
