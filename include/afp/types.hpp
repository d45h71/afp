#pragma once
#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <tuple>
#include <map>
#include <limits>
#include <type_traits>
#include <tl/expected.hpp>

namespace afp {
/// Unified error codes across the pipeline (no I/O/vendor specifics).
enum class Error {
  /// Generic decode failure or unsupported bitstream.
  DecodeError,
  /// Format not supported by the current build.
  UnsupportedFormat,
  /// Resampling failed or produced invalid output.
  ResampleError,
  /// Configuration mismatch between index and query.
  ConfigMismatch,
  /// Invalid argument or precondition not met.
  InvalidArgument,
  /// Arithmetic overflow / out-of-range bit packing.
  NumericOverflow,
  /// KV open/create error.
  KvOpenError,
  /// KV read error.
  KvReadError,
  /// KV write error.
  KvWriteError,
  /// KV merge/compaction error.
  KvMergeError,
  /// Input contained no audio samples.
  EmptyAudio,
  /// Not enough samples to form a frame.
  NoFrames,
  /// No peaks detected after filtering/NMS.
  NoPeaks,
  /// Operation exceeded configured deadline.
  Timeout,
  /// Malformed bytes / integrity violation.
  IntegrityError,
};

/// Canonical result used across the crate.
template <class T>
using Result = tl::expected<T, Error>;

/// Convenient byte buffer alias for on-disk/value payloads.
using ByteArray = std::vector<std::uint8_t>;

/// Convenient growable array alias used throughout the API.
template <class T>
using Array = std::vector<T>;

/// Deterministic ordered map alias used in ranking & votes.
template <class K, class V>
using Map = std::map<K, V>;

/// Endianness selector used for on-disk key encoding.
enum class Endian {
  /// Big-endian byte order.
  Big,
  /// Little-endian byte order.
  Little,
};

/// Opaque key container; layout defines used bits (32/48/64 packed into `u128`).
/// Represented as 16 raw bytes (opaque).
struct Key {
  std::array<std::uint8_t, 16> bytes;
};

/// Unit OK marker for functions that succeed without returning data.
struct OK {
};

/// Generic row-major matrix used for spectrograms and transforms.
template <class T>
struct Matrix {
  /// Flat row-major storage of length `rows * cols`.
  std::vector<T> data;
  /// Row count (e.g., time frames T).
  std::uint32_t rows{};
  /// Column count (e.g., frequency bins F).
  std::uint32_t cols{};
};

/// Mono PCM buffer.
struct PCM {
  /// Samples in mono (shape [N]).
  std::vector<float> samples;
  /// Sample rate in Hz.
  std::uint32_t sr{};
};

/// STFT magnitude (or reassigned magnitude) spectrogram.
struct STFTSpec {
  /// Magnitude spectrogram [T, K] in linear units.
  Matrix<float> mag;
  /// Sample rate (Hz).
  std::uint32_t sr{};
  /// Frame size (FFT size) in samples.
  std::uint32_t fft{};
  /// Hop size in samples.
  std::uint32_t hop{};
  /// Magnitude unit prior to scaling.
  const char* unit{};
};

/// Scaled, band-limited spectrogram.
struct ScaledSpec {
  /// Values [T, F'] after band-limit and scaling.
  Matrix<float> val;
  /// Sample rate (Hz).
  std::uint32_t sr{};
  /// Frame size (samples).
  std::uint32_t fft{};
  /// Hop size (samples).
  std::uint32_t hop{};
  /// Inclusive start bin in original K.
  std::uint16_t f0_bin{};
  /// Number of bins after cropping (F').
  std::uint16_t fprime{};
  /// Scale unit ("log-dB" or "pcen_log_db").
  const char* unit{};
};

/// Peak detected in the scaled spectrogram.
struct Peak {
  /// Frame index (time).
  std::uint32_t t{};
  /// Band-limited bin in [0..F'-1].
  std::uint16_t f{};
  /// Peak strength in same units as `ScaledSpec.val`.
  float strength{};
};

/// Pairing configuration (Δt window & budgets).
struct PairingCfg {
  /// Minimum Δt in frames.
  std::uint16_t dt_min_frames{};
  /// Maximum Δt in frames.
  std::uint16_t dt_max_frames{};
  /// Δt quantization step in frames.
  std::uint16_t delta_bin_frames{};
  /// Max targets per anchor.
  std::uint8_t max_targets_per_anchor{};
};

/// Bit-level definition of packed key integer.
struct KeyLayout {
  /// Total bits (32, 40..48, or 64).
  std::uint8_t total_bits{};
  /// Bits for anchor frequency bin.
  std::uint8_t bits_fa{};
  /// Bits for target frequency bin.
  std::uint8_t bits_ft{};
  /// Bits for Δt bin.
  std::uint8_t bits_dt{};
  /// Optional shard prefix bits (64-bit layout).
  std::uint8_t bits_shard{};
  /// Optional version nibble (64-bit layout).
  std::uint8_t bits_ver{};
  /// Canonical on-disk endianness.
  Endian endian{Endian::Little};
};

/// Key paired with its anchor time (frame index).
struct KeyWithTime {
  /// Packed key.
  Key key;
  /// Anchor time in frames.
  std::uint32_t t_anchor{};
};

/// One (key, track) block packed as bytes.
struct PostingBlock {
  /// Track identifier.
  std::uint32_t track_id{};
  /// Anchor count.
  std::uint32_t n{};
  /// First absolute anchor time.
  std::uint32_t t0{};
  /// Delta-encoded subsequent times (n-1).
  std::vector<std::uint32_t> dt;
};

/// Per-track metadata stored in KV.
struct TrackMeta {
  /// Track identifier.
  std::uint32_t track_id{};
  /// Sample rate (Hz).
  std::uint32_t sr{};
  /// FFT size (frame size) in samples.
  std::uint16_t fft{};
  /// Hop size (samples).
  std::uint16_t hop{};
  /// Total frames in the media.
  std::uint32_t frames{};
  /// CRC64 over audio bytes or canonical form.
  std::uint64_t audio_crc64{};
  /// Key layout version tag.
  std::uint8_t key_layout_version{};
};

/// Feature extraction configuration.
struct FeatureCfg {
  /// Target sample rate (Hz).
  std::uint32_t target_sr{};
  /// Frame size (samples).
  std::uint32_t frame_size{};
  /// Hop size (samples).
  std::uint32_t hop_size{};
  /// Use PCEN before log mapping.
  bool use_pcen{};
  /// Use DoG enhancement in frequency.
  bool use_dog{};
  /// Use reassigned magnitude for STFT.
  bool use_reassignment{};
  /// Minimum band edge (Hz).
  float band_min_hz{};
  /// Maximum band edge (Hz).
  float band_max_hz{};
  /// Lower percentile clip.
  float clip_low_pct{};
  /// Upper percentile clip.
  float clip_high_pct{};
  /// Min frequency separation (bins) for per-frame NMS.
  std::uint8_t nms_min_freq_sep_bins{};
  /// Neighborhood half-width in time (for maxima).
  std::uint8_t neigh_dt{};
  /// Neighborhood half-width in freq (for maxima).
  std::uint8_t neigh_df{};
  /// Upper cap per frame.
  std::uint8_t max_peaks_per_frame{};
  /// Lower bound per frame (backfill).
  std::uint8_t min_peaks_per_frame{};
};

/// Identification configuration (query-time).
struct IdentifyCfg {
  /// Pairing configuration.
  PairingCfg pairing;
  /// Feature extraction configuration.
  FeatureCfg feature;
  /// Key packing layout.
  KeyLayout key_layout;
  /// Minimum coverage threshold [0,1].
  float min_coverage{};
  /// Maximum entropy threshold (bits).
  float max_entropy{};
};

/// Build (index-time) configuration.
struct BuildCfg {
  /// Pairing configuration.
  PairingCfg pairing;
  /// Feature extraction configuration.
  FeatureCfg feature;
  /// Key packing layout.
  KeyLayout key_layout;
  /// Shard fan-out bits.
  std::uint8_t shard_bits{};
  /// Value compression indicator.
  const char* value_compression{};
};

/// Build report summarizing ingestion.
struct BuildReport {
  /// Tracks successfully processed.
  std::uint32_t tracks_ingested{};
  /// Total keys emitted across tracks.
  std::uint64_t keys_total{};
  /// Unique (Key,track) pairs written.
  std::uint64_t unique_keys{};
  /// Histogram of postings lengths for hot keys.
  std::vector<std::uint32_t> hotkey_histogram;
  /// Non-fatal warnings encountered.
  std::vector<std::string> warnings;
};

/// Mid/Side container used by audio helpers.
struct MidSide {
  /// Mono mid channel.
  PCM mid;
  /// Optional side channel.
  std::optional<PCM> side_opt;
};

/// DoG output pair {Det, Base}.
struct DogOutput {
  /// Detection spectrogram (possibly G1-G2).
  ScaledSpec det;
  /// Base spectrogram for confirmation.
  ScaledSpec base;
};

/// Percentile clip bounds.
struct ClipBounds {
  /// Lower bound.
  float lo{};
  /// Upper bound.
  float hi{};
};

/// Anchor emitted by postings parser (track_id, t_anchor).
struct Anchor {
  /// Track identifier.
  std::uint32_t track_id{};
  /// Absolute anchor time (frame index).
  std::uint32_t t_anchor{};
};

/// Identification match object.
struct Match {
  /// Winning track identifier.
  std::uint32_t track_id{};
  /// Estimated offset (seconds).
  double offset_seconds{};
  /// Confidence score [0,1].
  float score{};
};

/// Identify result (match or explicit no-match).
struct IdentifyResultMatch {
  Match value;
};

/// Identify result (no match) with reason.
struct IdentifyResultNoMatch {
  std::string reason;
};

/// Tagged union for identify result.
using IdentifyResult = std::variant<IdentifyResultMatch, IdentifyResultNoMatch>;
} // namespace afp
