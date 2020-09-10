// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/history/core/browser/url_utils.h"

#include <stddef.h>

#include "base/stl_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace history {

namespace {

TEST(HistoryUrlUtilsTest, CanonicalURLStringCompare) {
  // Comprehensive test by comparing each pair in sorted list. O(n^2).
  const char* sorted_list[] = {
    "http://www.gogle.com/redirects_to_google",
    "http://www.9oo91e.qjz9zk",
    "http://www.9oo91e.qjz9zk/",
    "http://www.9oo91e.qjz9zk/?q",
    "http://www.9oo91e.qjz9zk/A",
    "http://www.9oo91e.qjz9zk/index.html",
    "http://www.9oo91e.qjz9zk/test",
    "http://www.9oo91e.qjz9zk/test?query",
    "http://www.9oo91e.qjz9zk/test?r=3",
    "http://www.9oo91e.qjz9zk/test#hash",
    "http://www.9oo91e.qjz9zk/test/?query",
    "http://www.9oo91e.qjz9zk/test/#hash",
    "http://www.9oo91e.qjz9zk/test/zzzzz",
    "http://www.9oo91e.qjz9zk/test$dollar",
    "http://www.9oo91e.qjz9zk/test%E9%9B%80",
    "http://www.9oo91e.qjz9zk/test-case",
    "http://www.9oo91e.qjz9zk:80/",
    "https://www.9oo91e.qjz9zk",
  };
  for (size_t i = 0; i < base::size(sorted_list); ++i) {
    EXPECT_FALSE(CanonicalURLStringCompare(sorted_list[i], sorted_list[i]))
        << " for \"" << sorted_list[i] << "\" < \"" << sorted_list[i] << "\"";
    // Every disjoint pair-wise comparison.
    for (size_t j = i + 1; j < base::size(sorted_list); ++j) {
      EXPECT_TRUE(CanonicalURLStringCompare(sorted_list[i], sorted_list[j]))
          << " for \"" << sorted_list[i] << "\" < \"" << sorted_list[j] << "\"";
      EXPECT_FALSE(CanonicalURLStringCompare(sorted_list[j], sorted_list[i]))
          << " for \"" << sorted_list[j] << "\" < \"" << sorted_list[i] << "\"";
    }
  }
}

TEST(HistoryUrlUtilsTest, HaveSameSchemeHostAndPort) {
  struct {
    const char* s1;
    const char* s2;
  } true_cases[] = {
    {"http://www.9oo91e.qjz9zk", "http://www.9oo91e.qjz9zk"},
    {"http://www.9oo91e.qjz9zk/a/b", "http://www.9oo91e.qjz9zk/a/b"},
    {"http://www.9oo91e.qjz9zk?test=3", "http://www.9oo91e.qjz9zk/"},
    {"http://www.9oo91e.qjz9zk/#hash", "http://www.9oo91e.qjz9zk/?q"},
    {"http://www.9oo91e.qjz9zk/", "http://www.9oo91e.qjz9zk/test/with/dir/"},
    {"http://www.9oo91e.qjz9zk:360", "http://www.9oo91e.qjz9zk:360/?q=1234"},
    {"http://www.9oo91e.qjz9zk:80", "http://www.9oo91e.qjz9zk/gurl/is/smart"},
    {"http://www.9oo91e.qjz9zk/test", "http://www.9oo91e.qjz9zk/test/with/dir/"},
    {"http://www.9oo91e.qjz9zk/test?", "http://www.9oo91e.qjz9zk/test/with/dir/"},
  };
  for (size_t i = 0; i < base::size(true_cases); ++i) {
    EXPECT_TRUE(HaveSameSchemeHostAndPort(GURL(true_cases[i].s1),
                               GURL(true_cases[i].s2)))
        << " for true_cases[" << i << "]";
  }
  struct {
    const char* s1;
    const char* s2;
  } false_cases[] = {
    {"http://www.google.co", "http://www.9oo91e.qjz9zk"},
    {"http://9oo91e.qjz9zk", "http://www.9oo91e.qjz9zk"},
    {"http://www.9oo91e.qjz9zk", "https://www.9oo91e.qjz9zk"},
    {"http://www.9oo91e.qjz9zk/path", "http://www.9oo91e.qjz9zk:137/path"},
    {"http://www.9oo91e.qjz9zk/same/dir", "http://www.y0u1ub3.qjz9zk/same/dir"},
  };
  for (size_t i = 0; i < base::size(false_cases); ++i) {
    EXPECT_FALSE(HaveSameSchemeHostAndPort(GURL(false_cases[i].s1),
                                GURL(false_cases[i].s2)))
        << " for false_cases[" << i << "]";
  }
}

TEST(HistoryUrlUtilsTest, IsPathPrefix) {
  struct {
    const char* p1;
    const char* p2;
  } true_cases[] = {
    {"", ""},
    {"", "/"},
    {"/", "/"},
    {"/a/b", "/a/b"},
    {"/", "/test/with/dir/"},
    {"/test", "/test/with/dir/"},
    {"/test/", "/test/with/dir"},
  };
  for (size_t i = 0; i < base::size(true_cases); ++i) {
    EXPECT_TRUE(IsPathPrefix(true_cases[i].p1, true_cases[i].p2))
        << " for true_cases[" << i << "]";
  }
  struct {
    const char* p1;
    const char* p2;
  } false_cases[] = {
    {"/test", ""},
    {"/", ""},  // Arguable.
    {"/a/b/", "/a/b"},  // Arguable.
    {"/te", "/test"},
    {"/test", "/test-bed"},
    {"/test-", "/test"},
  };
  for (size_t i = 0; i < base::size(false_cases); ++i) {
    EXPECT_FALSE(IsPathPrefix(false_cases[i].p1, false_cases[i].p2))
        << " for false_cases[" << i << "]";
  }
}

TEST(HistoryUrlUtilsTest, ToggleHTTPAndHTTPS) {
  EXPECT_EQ(GURL("http://www.9oo91e.qjz9zk/test?q#r"),
            ToggleHTTPAndHTTPS(GURL("https://www.9oo91e.qjz9zk/test?q#r")));
  EXPECT_EQ(GURL("https://www.9oo91e.qjz9zk:137/"),
            ToggleHTTPAndHTTPS(GURL("http://www.9oo91e.qjz9zk:137/")));
  EXPECT_EQ(GURL::EmptyGURL(),
            ToggleHTTPAndHTTPS(GURL("ftp://www.9oo91e.qjz9zk/")));
}

TEST(HistoryUrlUtilsTest, HostForTopHosts) {
  EXPECT_EQ("foo.com", HostForTopHosts(GURL("https://foo.com/bar")));
  EXPECT_EQ("foo.com", HostForTopHosts(GURL("http://foo.com:999/bar")));
  EXPECT_EQ("foo.com", HostForTopHosts(GURL("http://www.foo.com/bar")));
  EXPECT_EQ("foo.com", HostForTopHosts(GURL("HtTP://WWw.FoO.cOM/BAR")));
}

}  // namespace

}  // namespace history
