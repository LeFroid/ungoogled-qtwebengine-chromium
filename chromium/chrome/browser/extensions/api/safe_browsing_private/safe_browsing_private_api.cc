// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/api/safe_browsing_private/safe_browsing_private_api.h"

#include <memory>
#include <utility>

#include "chrome/browser/browser_process.h"
#include "chrome/browser/extensions/api/safe_browsing_private/safe_browsing_util.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/safe_browsing/safe_browsing_navigation_observer_manager.h"
#include "chrome/browser/safe_browsing/safe_browsing_service.h"
#include "chrome/common/extensions/api/safe_browsing_private.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_function.h"

using safe_browsing::SafeBrowsingNavigationObserverManager;

namespace extensions {

namespace {

// The number of user gestures we trace back for the referrer chain.
const int kReferrerUserGestureLimit = 2;

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// SafeBrowsingPrivateGetReferrerChainFunction

SafeBrowsingPrivateGetReferrerChainFunction::
    SafeBrowsingPrivateGetReferrerChainFunction() {}

SafeBrowsingPrivateGetReferrerChainFunction::
    ~SafeBrowsingPrivateGetReferrerChainFunction() {}

ExtensionFunction::ResponseAction
SafeBrowsingPrivateGetReferrerChainFunction::Run() {
  std::unique_ptr<api::safe_browsing_private::GetReferrerChain::Params> params =
      api::safe_browsing_private::GetReferrerChain::Params::Create(*args_);
  EXTENSION_FUNCTION_VALIDATE(params.get());

  content::WebContents* contents = nullptr;

  if (!ExtensionTabUtil::GetTabById(params->tab_id, browser_context(),
                                    include_incognito_information(),
                                    &contents)) {
    return RespondNow(Error(
        base::StringPrintf("Could not find tab with id %d.", params->tab_id)));
  }

  std::vector<api::safe_browsing_private::ReferrerChainEntry> referrer_entries;
  return RespondNow(ArgumentList(
      api::safe_browsing_private::GetReferrerChain::Results::Create(
          referrer_entries)));
}

}  // namespace extensions
