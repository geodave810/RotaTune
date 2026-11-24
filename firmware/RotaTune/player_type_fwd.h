#pragma once
// Arduino IDE generates function prototypes before processing the sketch body.
// These declarations make the custom types visible to those prototypes.
struct TrackMetadata;
struct BrowserEntry;
enum PlaybackAction : uint8_t;
struct IndexRecord;   // v0.16.7d: PSRAM library index record
enum BrowserView : uint8_t;   // v0.18.2
enum LetterMode : uint8_t;    // v0.18.2: what the A-Z letter grid is browsing
