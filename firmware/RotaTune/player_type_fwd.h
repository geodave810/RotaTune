#pragma once
// Arduino IDE generates function prototypes before processing the sketch body.
// These declarations make the custom types visible to those prototypes.
struct TrackMetadata;
struct BrowserEntry;
enum PlaybackAction : uint8_t;
struct IndexRecord;   // v0.16.7d: PSRAM library index record
