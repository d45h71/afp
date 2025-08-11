#pragma once
#include "afp/types.hpp"
#include <optional>
#include <utility>

namespace afp {
/// KV open mode.
enum class KVMode {
  /// Create new store (or truncate).
  Create,
  /// Read-write existing store.
  ReadWrite,
  /// Read-only existing store.
  ReadOnly,
};

/// Opaque KV handle (no state exposed).
struct KVHandle {
};

/// Open a KV store at `path` with a mode and shard count.
/// - **Outputs:** `KVHandle` or `Error::KvOpenError`.
[[nodiscard]] Result<KVHandle> open(std::string_view path, KVMode mode,
                                    std::uint16_t shards);

/// Compute shard id for a given `key` (e.g., by high bits or hash).
/// - **Outputs:** shard id in `[0..shards)`.
[[nodiscard]] std::uint16_t shard_for_key(const KVHandle& h, Key key);

/// Get value bytes for `key` from `shard`, or `None` if absent.
/// - **Outputs:** `Option<ByteArray>`.
[[nodiscard]] Result<std::optional<ByteArray>> get(
    const KVHandle& h, std::uint16_t shard, Key key);

/// Append a value block to `(shard,key)` atomically.
/// - **Outputs:** `OK` or `Error::KvWriteError`.
[[nodiscard]] Result<OK> put_append(const KVHandle& h, std::uint16_t shard,
                                    Key key, ByteArray value);

/// Iterator type placeholder for sorted (Key, Value) pairs used in bulk merge.
struct SortedKeyValueIter {
  /// Implementation-private placeholder.
  void* _priv{nullptr};
  /// Next `(Key, ByteArray)` or `nullopt`.
  std::optional<std::pair<Key, ByteArray>> next();
};

/// Bulk merge sorted key/value pairs into `shard` (compaction/finalization).
/// - **Outputs:** `OK` or `Error::KvMergeError`.
[[nodiscard]] Result<OK> bulk_merge(const KVHandle& h, std::uint16_t shard,
                                    SortedKeyValueIter iter);

/// Close the KV store and flush pending ops.
/// - **Outputs:** `OK`.
[[nodiscard]] Result<OK> close(const KVHandle& h);

/// Store track metadata in a reserved keyspace.
/// - **Outputs:** `OK` or `Error::KvWriteError`.
[[nodiscard]] Result<OK> kv_put_trackmeta(const KVHandle& h,
                                          const TrackMeta& meta);

/// Optional per-shard merge/compact step.
/// - **Outputs:** `OK` or `Error::KvMergeError`.
[[nodiscard]] Result<OK> finalize_shards(const KVHandle& h);
} // namespace afp
