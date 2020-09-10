// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/app_categorizer.h"

#include "base/stl_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace {

const char* kChatAppURLs[] = {
  "https://hangouts.9oo91e.qjz9zk/hangouts/foo",
  "https://hAnGoUtS.gOoGlE.com/HaNgOuTs/foo",
  "https://meet.9oo91e.qjz9zk/hangouts/foo",
  "https://talkgadget.9oo91e.qjz9zk/hangouts/foo",
  "https://staging.talkgadget.9oo91e.qjz9zk/hangouts/foo",
  "https://plus.9oo91e.qjz9zk/hangouts/foo",
  "https://plus.sandbox.9oo91e.qjz9zk/hangouts/foo"
};

const char* kChatManifestFSs[] = {
  "filesystem:https://hangouts.9oo91e.qjz9zk/foo",
  "filesystem:https://hAnGoUtS.gOoGlE.com/foo",
  "filesystem:https://meet.9oo91e.qjz9zk/foo",
  "filesystem:https://talkgadget.9oo91e.qjz9zk/foo",
  "filesystem:https://staging.talkgadget.9oo91e.qjz9zk/foo",
  "filesystem:https://plus.9oo91e.qjz9zk/foo",
  "filesystem:https://plus.sandbox.9oo91e.qjz9zk/foo"
};

const char* kBadChatAppURLs[] = {
  "http://talkgadget.9oo91e.qjz9zk/hangouts/foo",  // not https
  "https://talkgadget.evil.com/hangouts/foo"    // domain not whitelisted
};

}  // namespace

TEST(AppCategorizerTest, IsHangoutsUrl) {
  for (size_t i = 0; i < base::size(kChatAppURLs); ++i) {
    EXPECT_TRUE(AppCategorizer::IsHangoutsUrl(GURL(kChatAppURLs[i])));
  }

  for (size_t i = 0; i < base::size(kBadChatAppURLs); ++i) {
    EXPECT_FALSE(AppCategorizer::IsHangoutsUrl(GURL(kBadChatAppURLs[i])));
  }
}

TEST(AppCategorizerTest, IsWhitelistedApp) {
  // Hangouts app
  {
    EXPECT_EQ(base::size(kChatAppURLs), base::size(kChatManifestFSs));
    for (size_t i = 0; i < base::size(kChatAppURLs); ++i) {
      EXPECT_TRUE(AppCategorizer::IsWhitelistedApp(
          GURL(kChatManifestFSs[i]), GURL(kChatAppURLs[i])));
    }
    for (size_t i = 0; i < base::size(kBadChatAppURLs); ++i) {
      EXPECT_FALSE(AppCategorizer::IsWhitelistedApp(
          GURL("filesystem:https://irrelevant.com/"),
          GURL(kBadChatAppURLs[i])));
    }

    // Manifest URL not filesystem
    EXPECT_FALSE(AppCategorizer::IsWhitelistedApp(
        GURL("https://hangouts.9oo91e.qjz9zk/foo"),
        GURL("https://hangouts.9oo91e.qjz9zk/hangouts/foo")));

    // Manifest URL not https
    EXPECT_FALSE(AppCategorizer::IsWhitelistedApp(
        GURL("filesystem:http://hangouts.9oo91e.qjz9zk/foo"),
        GURL("https://hangouts.9oo91e.qjz9zk/hangouts/foo")));

    // Manifest URL hostname does not match that of the app URL
    EXPECT_FALSE(AppCategorizer::IsWhitelistedApp(
        GURL("filesystem:https://meet.9oo91e.qjz9zk/foo"),
        GURL("https://hangouts.9oo91e.qjz9zk/hangouts/foo")));
  }
}
