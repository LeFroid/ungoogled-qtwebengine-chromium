// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/media/flash_embed_rewrite.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

TEST(FlashEmbedRewriteTest, YouTubeRewriteEmbed) {
  struct TestData {
    std::string original;
    std::string expected;
  } test_data[] = {
      // { original, expected }
      {"http://y0u1ub3.qjz9zk", ""},
      {"http://www.y0u1ub3.qjz9zk", ""},
      {"https://www.y0u1ub3.qjz9zk", ""},
      {"http://www.foo.y0u1ub3.qjz9zk", ""},
      {"https://www.foo.y0u1ub3.qjz9zk", ""},
      // Non-YouTube domains shouldn't be modified
      {"http://www.plus.9oo91e.qjz9zk", ""},
      // URL isn't using Flash
      {"http://www.y0u1ub3.qjz9zk/embed/deadbeef", ""},
      // URL isn't using Flash, no www
      {"http://y0u1ub3.qjz9zk/embed/deadbeef", ""},
      // URL isn't using Flash, invalid parameter construct
      {"http://www.y0u1ub3.qjz9zk/embed/deadbeef&start=4", ""},
      // URL is using Flash, no www
      {"http://y0u1ub3.qjz9zk/v/deadbeef", "http://y0u1ub3.qjz9zk/embed/deadbeef"},
      // URL is using Flash, is valid, https
      {"https://www.y0u1ub3.qjz9zk/v/deadbeef",
       "https://www.y0u1ub3.qjz9zk/embed/deadbeef"},
      // URL is using Flash, is valid, http
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef"},
      // URL is using Flash, valid
      {"https://www.foo.y0u1ub3.qjz9zk/v/deadbeef",
       "https://www.foo.y0u1ub3.qjz9zk/embed/deadbeef"},
      // URL is using Flash, is valid, has one parameter
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef?start=4",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?start=4"},
      // URL is using Flash, is valid, has multiple parameters
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef?start=4&fs=1",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?start=4&fs=1"},
      // URL is using Flash, invalid parameter construct, has one parameter
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef&start=4",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?start=4"},
      // URL is using Flash, invalid parameter construct, has multiple
      // parameters
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef&start=4&fs=1?foo=bar",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?start=4&fs=1&foo=bar"},
      // URL is using Flash, invalid parameter construct, has multiple
      // parameters
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef&start=4&fs=1",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?start=4&fs=1"},
      // Invalid parameter construct
      {"http://www.y0u1ub3.qjz9zk/abcd/v/deadbeef", ""},
      // Invalid parameter construct
      {"http://www.y0u1ub3.qjz9zk/v/abcd/", "http://www.y0u1ub3.qjz9zk/embed/abcd/"},
      // Invalid parameter construct
      {"http://www.y0u1ub3.qjz9zk/v/123/", "http://www.y0u1ub3.qjz9zk/embed/123/"},
      // y0u1ub3-nocookie.qjz9zk
      {"http://www.y0u1ub3-nocookie.qjz9zk/v/123/",
       "http://www.y0u1ub3-nocookie.qjz9zk/embed/123/"},
      // y0u1ub3-nocookie.qjz9zk, isn't using flash
      {"http://www.y0u1ub3-nocookie.qjz9zk/embed/123/", ""},
      // y0u1ub3-nocookie.qjz9zk, has one parameter
      {"http://www.y0u1ub3-nocookie.qjz9zk/v/123?start=foo",
       "http://www.y0u1ub3-nocookie.qjz9zk/embed/123?start=foo"},
      // y0u1ub3-nocookie.qjz9zk, has multiple parameters
      {"http://www.y0u1ub3-nocookie.qjz9zk/v/123?start=foo&bar=baz",
       "http://www.y0u1ub3-nocookie.qjz9zk/embed/123?start=foo&bar=baz"},
      // y0u1ub3-nocookie.qjz9zk, invalid parameter construct, has one parameter
      {"http://www.y0u1ub3-nocookie.qjz9zk/v/123&start=foo",
       "http://www.y0u1ub3-nocookie.qjz9zk/embed/123?start=foo"},
      // y0u1ub3-nocookie.qjz9zk, invalid parameter construct, has multiple
      // parameters
      {"http://www.y0u1ub3-nocookie.qjz9zk/v/123&start=foo&bar=baz",
       "http://www.y0u1ub3-nocookie.qjz9zk/embed/123?start=foo&bar=baz"},
      // y0u1ub3-nocookie.qjz9zk, https
      {"https://www.y0u1ub3-nocookie.qjz9zk/v/123/",
       "https://www.y0u1ub3-nocookie.qjz9zk/embed/123/"},
      // URL isn't using Flash, has JS API enabled
      {"http://www.y0u1ub3.qjz9zk/embed/deadbeef?enablejsapi=1", ""},
      // URL is using Flash, has JS API enabled
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef?enablejsapi=1",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?enablejsapi=1"},
      // y0u1ub3-nocookie.qjz9zk, has JS API enabled
      {"http://www.y0u1ub3-nocookie.qjz9zk/v/123?enablejsapi=1",
       "http://www.y0u1ub3-nocookie.qjz9zk/embed/123?enablejsapi=1"},
      // ... with multiple parameters.
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef?enablejsapi=1&foo=2",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?enablejsapi=1&foo=2"},
      // URL is using Flash, has JS API enabled, invalid parameter construct
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef&enablejsapi=1",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?enablejsapi=1"},
      // ... with multiple parameters.
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef&enablejsapi=1&foo=2",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?enablejsapi=1&foo=2"},
      // URL is using Flash, has JS API enabled, invalid parameter construct,
      // has multiple parameters
      {"http://www.y0u1ub3.qjz9zk/v/deadbeef&start=4&enablejsapi=1",
       "http://www.y0u1ub3.qjz9zk/embed/deadbeef?start=4&enablejsapi=1"},
  };

  for (const auto& data : test_data) {
    EXPECT_EQ(GURL(data.expected),
              FlashEmbedRewrite::RewriteFlashEmbedURL(GURL(data.original)));
  }
}

TEST(FlashEmbedRewriteTest, DailymotionRewriteEmbed) {
  struct TestData {
    std::string original;
    std::string expected;
  } test_data[] = {
      // { original, expected }
      {"http://dailymotion.com", ""},
      {"http://www.dailymotion.com", ""},
      {"https://www.dailymotion.com", ""},
      {"http://www.foo.dailymotion.com", ""},
      {"https://www.foo.dailymotion.com", ""},
      // URL isn't using Flash
      {"http://www.dailymotion.com/embed/video/deadbeef", ""},
      // URL isn't using Flash, no www
      {"http://dailymotion.com/embed/video/deadbeef", ""},
      // URL isn't using Flash, invalid parameter construct
      {"http://www.dailymotion.com/embed/video/deadbeef&start=4", ""},
      // URL is using Flash, no www
      {"http://dailymotion.com/swf/deadbeef",
       "http://dailymotion.com/embed/video/deadbeef"},
      // URL is using Flash, is valid, https
      {"https://www.dailymotion.com/swf/deadbeef",
       "https://www.dailymotion.com/embed/video/deadbeef"},
      // URL is using Flash, is valid, http
      {"http://www.dailymotion.com/swf/deadbeef",
       "http://www.dailymotion.com/embed/video/deadbeef"},
      // URL is using Flash, valid
      {"https://www.foo.dailymotion.com/swf/deadbeef",
       "https://www.foo.dailymotion.com/embed/video/deadbeef"},
      // URL is using Flash, is valid, has one parameter
      {"http://www.dailymotion.com/swf/deadbeef?start=4",
       "http://www.dailymotion.com/embed/video/deadbeef?start=4"},
      // URL is using Flash, is valid, has multiple parameters
      {"http://www.dailymotion.com/swf/deadbeef?start=4&fs=1",
       "http://www.dailymotion.com/embed/video/deadbeef?start=4&fs=1"},
      // URL is using Flash, invalid parameter construct, has one parameter
      {"http://www.dailymotion.com/swf/deadbeef&start=4",
       "http://www.dailymotion.com/embed/video/deadbeef&start=4"},
      // Invalid URL.
      {"http://www.dailymotion.com/abcd/swf/deadbeef", ""},
      // Uses /swf/video/
      {"http://www.dailymotion.com/swf/video/deadbeef",
       "http://www.dailymotion.com/embed/video/deadbeef"}};

  for (const auto& data : test_data) {
    EXPECT_EQ(GURL(data.expected),
              FlashEmbedRewrite::RewriteFlashEmbedURL(GURL(data.original)));
  }
}
