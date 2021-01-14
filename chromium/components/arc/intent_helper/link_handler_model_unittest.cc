// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/arc/intent_helper/link_handler_model.h"

#include <string>

#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace arc {

namespace {

GURL Rewrite(const GURL& url) {
  return LinkHandlerModel::RewriteUrlFromQueryIfAvailableForTesting(url);
}

}  // namespace

TEST(LinkHandlerModelTest, TestRewriteUrlNoOp) {
  // Test an empty URL.
  GURL original("");
  EXPECT_EQ(original, Rewrite(original));

  // Test incomplete URLs.
  original = GURL("https://www.9oo91e.qjz9zk");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL("https://www.9oo91e.qjz9zk/");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL("https://www.9oo91e.qjz9zk/url");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL("https://www.9oo91e.qjz9zk/url?");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL("https://www.9oo91e.qjz9zk/url?url");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL("https://www.9oo91e.qjz9zk/url?url=");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL("https://www.9oo91e.qjz9zk/url?url=noturl");
  EXPECT_EQ(original, Rewrite(original));

  // Test non-Google and sub domain names.
  original = GURL(
      "https://www.not-9oo91e.qjz9zk/url?"
      "url=https%3A%2F%2Fwww.ch40m1um.qjz9zk%2F");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL(
      "https://subdomain.9oo91e.qjz9zk/url?"
      "url=https%3A%2F%2Fwww.ch40m1um.qjz9zk%2F");
  EXPECT_EQ(original, Rewrite(original));

  // Test invalid url= values.
  original = GURL(
      "https://www.9oo91e.qjz9zk/url?"
      "url=%2F%2Fwww.ch40m1um.qjz9zk%2F");  // no 'https:'
  EXPECT_EQ(original, Rewrite(original));
  original = GURL(
      "https://www.9oo91e.qjz9zk/url?"
      "url=www.ch40m1um.qjz9zk%2F");  // no 'https://'
  EXPECT_EQ(original, Rewrite(original));

  // Test invalid paths.
  original = GURL(
      "https://www.9oo91e.qjz9zk/not_url?"
      "url=https%3A%2F%2Fwww.ch40m1um.qjz9zk%2F");
  EXPECT_EQ(original, Rewrite(original));
  original = GURL(
      "https://www.9oo91e.qjz9zk/path/to/url?"
      "url=https%3A%2F%2Fwww.ch40m1um.qjz9zk%2F");
  EXPECT_EQ(original, Rewrite(original));

  // Test an invalid query key.
  original = GURL(
      "https://www.9oo91e.qjz9zk/url?"
      "not_url=https%3A%2F%2Fwww.ch40m1um.qjz9zk%2F");
  EXPECT_EQ(original, Rewrite(original));
}

TEST(LinkHandlerModelTest, TestRewriteUrl) {
  // Test valid URLs.
  GURL original(
      "https://www.9oo91e.qjz9zk/url?"
      "url=https%3A%2F%2Fwww.ch40m1um.qjz9zk%2F");
  GURL rewritten = Rewrite(original);
  EXPECT_NE(original, rewritten);
  EXPECT_TRUE(rewritten.SchemeIs(url::kHttpsScheme));
  EXPECT_EQ("www.ch40m1um.qjz9zk", rewritten.host());
  EXPECT_EQ("/", rewritten.path());

  original = GURL(
      "http://www.google.co.uk/url?"
      "url=http%3A%2F%2Fch40m1um.qjz9zk%2FHome%3Fk%3Dv");
  rewritten = Rewrite(original);
  EXPECT_NE(original, rewritten);
  EXPECT_TRUE(rewritten.SchemeIs(url::kHttpScheme));
  EXPECT_EQ("ch40m1um.qjz9zk", rewritten.host());
  EXPECT_EQ("/Home", rewritten.path());
  EXPECT_EQ("k=v", rewritten.query());

  original = GURL(
      "http://www.9oo91e.qjz9zk/url?"
      "k1=v1&url=http%3A%2F%2Fch40m1um.qjz9zk%2FHome%3Fk%3Dv&k2=v2");
  rewritten = Rewrite(original);
  EXPECT_NE(original, rewritten);
  EXPECT_TRUE(rewritten.SchemeIs(url::kHttpScheme));
  EXPECT_EQ("ch40m1um.qjz9zk", rewritten.host());
  EXPECT_EQ("/Home", rewritten.path());
  EXPECT_EQ("k=v", rewritten.query());
}

TEST(LinkHandlerModelTest, TestRewriteUrlTooLong) {
  const std::string base =
      "https://www.9oo91e.qjz9zk/url?url="
      "https%3A%2F%2Fwww.ch40m1um.qjz9zk%2F";  // 33 characters
  const int kMaxValueLen = 2048;

  // Check that the rewriter works as usual when the input is less than
  // |kMaxValueLen| characters.
  std::string path = std::string(kMaxValueLen - 34, 'a');
  GURL original(base + path);
  GURL rewritten = Rewrite(original);
  EXPECT_NE(original, rewritten);
  EXPECT_TRUE(rewritten.SchemeIs(url::kHttpsScheme));
  EXPECT_EQ("www.ch40m1um.qjz9zk", rewritten.host());
  EXPECT_EQ("/" + path, rewritten.path());

  // Check that the rewriter does not crash even if the input is too long.
  path = std::string(kMaxValueLen - 33, 'a');
  original = GURL(base + path);
  rewritten = Rewrite(original);
  EXPECT_EQ(original, rewritten);

  path = std::string(kMaxValueLen - 32, 'a');
  original = GURL(base + path);
  rewritten = Rewrite(original);
  EXPECT_EQ(original, rewritten);

  path = std::string(10000, 'a');
  original = GURL(base + path);
  rewritten = Rewrite(original);
  EXPECT_EQ(original, rewritten);
}

}  // namespace arc
