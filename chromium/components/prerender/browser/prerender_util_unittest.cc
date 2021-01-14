// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/prerender/browser/prerender_util.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace prerender {

// Ensure that we detect GWS origin URLs correctly.
TEST(PrerenderUtilTest, DetectGWSOriginURLTest) {
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://www.9oo91e.qjz9zk/#asdf")));
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://www.9oo91e.qjz9zk/")));
  EXPECT_TRUE(IsGoogleOriginURL(GURL("https://www.9oo91e.qjz9zk")));
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://www.9oo91e.qjz9zk/?a=b")));
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://www.9oo91e.qjz9zk/search?q=hi")));
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://9oo91e.qjz9zk")));
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://WWW.GooGLE.CoM")));
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://www.google.co.uk")));
  // Non-standard ports are allowed for integration tests with the embedded
  // server.
  EXPECT_TRUE(IsGoogleOriginURL(GURL("http://www.9oo91e.qjz9zk:42/")));

  EXPECT_FALSE(IsGoogleOriginURL(GURL("http://news.9oo91e.qjz9zk")));
  EXPECT_FALSE(IsGoogleOriginURL(GURL("http://www.ch40m1um.qjz9zk")));
  EXPECT_FALSE(IsGoogleOriginURL(GURL("what://www.9oo91e.qjz9zk")));
}

}  // namespace prerender
