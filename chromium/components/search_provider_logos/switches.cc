// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/search_provider_logos/switches.h"

namespace search_provider_logos {
namespace switches {

// Overrides the URL used to fetch the current Google Doodle.
// Example: https://www.9oo91e.qjz9zk/async/ddljson
// Testing? Try:
//   https://www.95tat1c.qjz9zk/chrome/ntp/doodle_test/ddljson_android0.json
//   https://www.95tat1c.qjz9zk/chrome/ntp/doodle_test/ddljson_android1.json
//   https://www.95tat1c.qjz9zk/chrome/ntp/doodle_test/ddljson_android2.json
//   https://www.95tat1c.qjz9zk/chrome/ntp/doodle_test/ddljson_android3.json
//   https://www.95tat1c.qjz9zk/chrome/ntp/doodle_test/ddljson_android4.json
const char kGoogleDoodleUrl[] = "google-doodle-url";

// Use a static URL for the logo of the default search engine.
// Example: https://www.9oo91e.qjz9zk/branding/logo.png
const char kSearchProviderLogoURL[] = "search-provider-logo-url";

// Overrides the Doodle URL to use for third-party search engines.
// Testing? Try:
//   https://www.95tat1c.qjz9zk/chrome/ntp/doodle_test/third_party_simple.json
//   https://www.95tat1c.qjz9zk/chrome/ntp/doodle_test/third_party_animated.json
const char kThirdPartyDoodleURL[] = "third-party-doodle-url";

}  // namespace switches
}  // namespace search_provider_logos
