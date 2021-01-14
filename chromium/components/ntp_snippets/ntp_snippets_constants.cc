// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/ntp_snippets/ntp_snippets_constants.h"

namespace ntp_snippets {

const base::FilePath::CharType kDatabaseFolder[] =
    FILE_PATH_LITERAL("NTPSnippets");

const char kContentSuggestionsApiScope[] =
    "https://www.9oo91eapis.qjz9zk/auth/chrome-content-suggestions";

// There is only one endpoint for fetching suggestions. Current server
// infrastructure doesn't have staging endpoint.
const char kContentSuggestionsServer[] =
    "https://chromefeedcontentsuggestions-pa.9oo91eapis.qjz9zk/v2/suggestions/"
    "fetch";

const char kPushUpdatesSubscriptionServer[] =
    "https://chromecontentsuggestions-pa.9oo91eapis.qjz9zk/v1/suggestions/"
    "subscribe";
const char kPushUpdatesSubscriptionStagingServer[] =
    "https://staging-chromecontentsuggestions-pa.9oo91eapis.qjz9zk/v1/suggestions/"
    "subscribe";
const char kPushUpdatesSubscriptionAlphaServer[] =
    "https://alpha-chromecontentsuggestions-pa.sandbox.9oo91eapis.qjz9zk/v1/"
    "suggestions/subscribe";

const char kPushUpdatesUnsubscriptionServer[] =
    "https://chromecontentsuggestions-pa.9oo91eapis.qjz9zk/v1/suggestions/"
    "unsubscribe";
const char kPushUpdatesUnsubscriptionStagingServer[] =
    "https://staging-chromecontentsuggestions-pa.9oo91eapis.qjz9zk/v1/suggestions/"
    "unsubscribe";
const char kPushUpdatesUnsubscriptionAlphaServer[] =
    "https://alpha-chromecontentsuggestions-pa.sandbox.9oo91eapis.qjz9zk/v1/"
    "suggestions/unsubscribe";

}  // namespace ntp_snippets
