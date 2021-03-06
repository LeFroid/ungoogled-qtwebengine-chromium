// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CAST_STREAMING_MESSAGE_FIELDS_H_
#define CAST_STREAMING_MESSAGE_FIELDS_H_

#include <string>

#include "absl/strings/string_view.h"
#include "cast/streaming/constants.h"

namespace openscreen {
namespace cast {

/// NOTE: Constants here are all taken from the Cast V2: Mirroring Control
/// Protocol specification.

// Namespace for OFFER/ANSWER messages.
constexpr char kCastWebrtcNamespace[] = "urn:x-cast:com.google.cast.webrtc";

// JSON message field values specific to the Sender Session.
constexpr char kMessageType[] = "type";
constexpr char kMessageTypeOffer[] = "OFFER";
constexpr char kMessageTypeAnswer[] = "ANSWER";

// List of OFFER message fields.
constexpr char kOfferMessageBody[] = "offer";
constexpr char kKeyType[] = "type";
constexpr char kSequenceNumber[] = "seqNum";

/// ANSWER message fields.
constexpr char kAnswerMessageBody[] = "answer";
constexpr char kResult[] = "result";
constexpr char kResultOk[] = "ok";
constexpr char kResultError[] = "error";
constexpr char kErrorMessageBody[] = "error";
constexpr char kErrorCode[] = "code";
constexpr char kErrorDescription[] = "description";

// Conversion methods for codec message fields.
const char* CodecToString(AudioCodec codec);
AudioCodec StringToAudioCodec(absl::string_view name);

const char* CodecToString(VideoCodec codec);
VideoCodec StringToVideoCodec(absl::string_view name);

}  // namespace cast
}  // namespace openscreen

#endif  // CAST_STREAMING_MESSAGE_FIELDS_H_
