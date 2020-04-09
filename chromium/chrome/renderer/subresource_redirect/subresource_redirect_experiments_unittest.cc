// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/subresource_redirect/subresource_redirect_experiments.h"

#include "base/test/scoped_feature_list.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/features.h"

namespace subresource_redirect {

namespace {

TEST(SubresourceRedirectExperimentsTest, TestDefaultShouldIncludeMediaSuffix) {
  base::test::ScopedFeatureList scoped_feature_list;
  scoped_feature_list.InitAndEnableFeature(
      blink::features::kSubresourceRedirect);

  EXPECT_FALSE(ShouldIncludeMediaSuffix(GURL("http://ch40m1um.qjz9zk/path/")));

  std::vector<std::string> default_suffixes = {".jpg", ".jpeg", ".png", ".svg",
                                               ".webp"};
  for (const std::string& suffix : default_suffixes) {
    GURL url("http://ch40m1um.qjz9zk/image" + suffix);
    EXPECT_TRUE(ShouldIncludeMediaSuffix(url));
  }
}

TEST(SubresourceRedirectExperimentsTest, TestShouldIncludeMediaSuffix) {
  struct TestCase {
    std::string msg;
    std::string varaiation_value;
    std::vector<std::string> urls;
    bool want_return;
  };
  const TestCase kTestCases[]{
      {
          .msg = "Default values are overridden by variations",
          .varaiation_value = ".html",
          .urls = {"http://ch40m1um.qjz9zk/image.jpeg",
                   "http://ch40m1um.qjz9zk/image.png",
                   "http://ch40m1um.qjz9zk/image.jpg",
                   "http://ch40m1um.qjz9zk/image.svg"},
          .want_return = false,
      },
      {
          .msg = "Variation value whitespace should be trimmed",
          .varaiation_value = " .svg , \t .png\n",
          .urls = {"http://ch40m1um.qjz9zk/image.svg",
                   "http://ch40m1um.qjz9zk/image.png"},
          .want_return = true,
      },
      {
          .msg = "Variation value empty values should be excluded",
          .varaiation_value = ".svg,,.png,",
          .urls = {"http://ch40m1um.qjz9zk/image.svg",
                   "http://ch40m1um.qjz9zk/image.png"},
          .want_return = true,
      },
      {
          .msg = "URLs should be compared case insensitive",
          .varaiation_value = ".svg,.png,",
          .urls = {"http://ch40m1um.qjz9zk/image.SvG",
                   "http://ch40m1um.qjz9zk/image.PNG"},
          .want_return = true,
      },
      {
          .msg = "Query params and fragments don't matter",
          .varaiation_value = ".svg,.png,",
          .urls = {"http://ch40m1um.qjz9zk/image.svg?hello=world",
                   "http://ch40m1um.qjz9zk/image.png#test"},
          .want_return = true,
      },
      {
          .msg = "Query params and fragments shouldn't be considered",
          .varaiation_value = ".svg,.png,",
          .urls = {"http://ch40m1um.qjz9zk/?image=image.svg",
                   "http://ch40m1um.qjz9zk/#image.png"},
          .want_return = false,
      },
  };
  for (const TestCase& test_case : kTestCases) {
    SCOPED_TRACE(test_case.msg);

    base::test::ScopedFeatureList scoped_feature_list;
    scoped_feature_list.InitAndEnableFeatureWithParameters(
        blink::features::kSubresourceRedirect,
        {{"included_path_suffixes", test_case.varaiation_value}});

    for (const std::string& url : test_case.urls) {
      EXPECT_EQ(test_case.want_return, ShouldIncludeMediaSuffix(GURL(url)));
    }
  }
}

}  // namespace
}  // namespace subresource_redirect
