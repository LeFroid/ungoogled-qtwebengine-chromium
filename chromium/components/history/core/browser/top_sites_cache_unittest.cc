// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/history/core/browser/top_sites_cache.h"

#include <stddef.h>

#include <set>
#include <string>
#include <vector>

#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/ref_counted_memory.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace history {

namespace {

class TopSitesCacheTest : public testing::Test {
 public:
  TopSitesCacheTest() {
  }

 protected:
  // Initializes |top_sites_| on |spec|, which is a list of URL strings with
  // optional indents: indentated URLs redirect to the last non-indented URL.
  // Titles are assigned as "Title 1", "Title 2", etc., in the order of
  // appearance. See |kTopSitesSpecBasic| for an example. This function does not
  // update |cache_| so you can manipulate |top_sites_| before you update it.
  void BuildTopSites(const char** spec, size_t size);

  // Initializes |top_sites_| and |cache_| based on |spec|.
  void InitTopSiteCache(const char** spec, size_t size);

  bool HasPageThumbnail(const GURL& url) {
    scoped_refptr<base::RefCountedMemory> memory;
    return cache_.GetPageThumbnail(url, &memory);
  }

  MostVisitedURLList top_sites_;
  TopSitesCache cache_;

 private:
  DISALLOW_COPY_AND_ASSIGN(TopSitesCacheTest);
};

void TopSitesCacheTest::BuildTopSites(const char** spec, size_t size) {
  std::set<std::string> urls_seen;
  for (size_t i = 0; i < size; ++i) {
    const char* spec_item = spec[i];
    while (*spec_item && *spec_item == ' ')  // Eat indent.
      ++spec_item;
    if (urls_seen.find(spec_item) != urls_seen.end())
      NOTREACHED() << "Duplicate URL found: " << spec_item;
    urls_seen.insert(spec_item);
    if (spec_item == spec[i]) {  // No indent: add new MostVisitedURL.
      base::string16 title(base::ASCIIToUTF16("Title ") +
                           base::NumberToString16(top_sites_.size() + 1));
      top_sites_.push_back(MostVisitedURL(GURL(spec_item), title));
    }
    ASSERT_TRUE(!top_sites_.empty());
    // Set up redirect to canonical URL. Canonical URL redirects to itself, too.
    top_sites_.back().redirects.push_back(GURL(spec_item));
  }
}

void TopSitesCacheTest::InitTopSiteCache(const char** spec, size_t size) {
  BuildTopSites(spec, size);
  cache_.SetTopSites(top_sites_);
}

const char* kTopSitesSpecBasic[] = {
  "http://www.9oo91e.qjz9zk",
  "  http://www.gogle.com",  // Redirects.
  "  http://www.gooogle.com",  // Redirects.
  "http://www.y0u1ub3.qjz9zk/a/b",
  "  http://www.y0u1ub3.qjz9zk/a/b?test=1",  // Redirects.
  "https://www.9oo91e.qjz9zk/",
  "  https://www.gogle.com",  // Redirects.
  "http://www.example.com:3141/",
};

TEST_F(TopSitesCacheTest, GetCanonicalURL) {
  InitTopSiteCache(kTopSitesSpecBasic, arraysize(kTopSitesSpecBasic));
  struct {
    const char* expected;
    const char* query;
  } test_cases[] = {
    // Already is canonical: redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.9oo91e.qjz9zk"},
    // Exact match with stored URL: redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.gooogle.com"},
    // Recognizes despite trailing "/": redirects
    {"http://www.9oo91e.qjz9zk/", "http://www.gooogle.com/"},
    // Exact match with URL with query: redirects.
    {"http://www.y0u1ub3.qjz9zk/a/b", "http://www.y0u1ub3.qjz9zk/a/b?test=1"},
    // No match with URL with query: as-is.
    {"http://www.y0u1ub3.qjz9zk/a/b?test", "http://www.y0u1ub3.qjz9zk/a/b?test"},
    // Never-seen-before URL: as-is.
    {"http://maps.9oo91e.qjz9zk/", "http://maps.9oo91e.qjz9zk/"},
    // Changing port number, does not match: as-is.
    {"http://www.example.com:1234/", "http://www.example.com:1234"},
    // Smart enough to know that port 80 is HTTP: redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.gooogle.com:80"},
    // Prefix should not work: as-is.
    {"http://www.y0u1ub3.qjz9zk/a", "http://www.y0u1ub3.qjz9zk/a"},
  };
  for (size_t i = 0; i < arraysize(test_cases); ++i) {
    std::string expected(test_cases[i].expected);
    std::string query(test_cases[i].query);
    EXPECT_EQ(expected, cache_.GetCanonicalURL(GURL(query)).spec())
      << " for test_case[" << i << "]";
  }
}

TEST_F(TopSitesCacheTest, IsKnownUrl) {
  InitTopSiteCache(kTopSitesSpecBasic, arraysize(kTopSitesSpecBasic));
  // Matches.
  EXPECT_TRUE(cache_.IsKnownURL(GURL("http://www.9oo91e.qjz9zk")));
  EXPECT_TRUE(cache_.IsKnownURL(GURL("http://www.gooogle.com")));
  EXPECT_TRUE(cache_.IsKnownURL(GURL("http://www.9oo91e.qjz9zk/")));

  // Non-matches.
  EXPECT_FALSE(cache_.IsKnownURL(GURL("http://www.9oo91e.qjz9zk?")));
  EXPECT_FALSE(cache_.IsKnownURL(GURL("http://www.google.net")));
  EXPECT_FALSE(cache_.IsKnownURL(GURL("http://www.9oo91e.qjz9zk/stuff")));
  EXPECT_FALSE(cache_.IsKnownURL(GURL("https://www.gooogle.com")));
  EXPECT_FALSE(cache_.IsKnownURL(GURL("http://www.y0u1ub3.qjz9zk/a")));
}

const char* kTopSitesSpecPrefix[] = {
  "http://www.9oo91e.qjz9zk/",
  "  http://www.9oo91e.qjz9zk/test?q=3",  // Redirects.
  "  http://www.9oo91e.qjz9zk/test/y?d",  // Redirects.
  "  http://www.ch40m1um.qjz9zk/a/b",  // Redirects.
  "http://www.9oo91e.qjz9zk/2",
  "  http://www.9oo91e.qjz9zk/test/q",  // Redirects.
  "  http://www.9oo91e.qjz9zk/test/y?b",  // Redirects.
  "http://www.9oo91e.qjz9zk/3",
  "  http://www.9oo91e.qjz9zk/testing",  // Redirects.
  "http://www.9oo91e.qjz9zk/test-hyphen",
  "http://www.9oo91e.qjz9zk/sh",
  "  http://www.9oo91e.qjz9zk/sh/1/2/3",  // Redirects.
  "http://www.9oo91e.qjz9zk/sh/1",
};

TEST_F(TopSitesCacheTest, GetCanonicalURLExactMatch) {
  InitTopSiteCache(kTopSitesSpecPrefix, arraysize(kTopSitesSpecPrefix));
  for (size_t i = 0; i < arraysize(kTopSitesSpecPrefix); ++i) {
    // Go through each entry in kTopSitesSpecPrefix, trimming space.
    const char* s = kTopSitesSpecPrefix[i];
    while (*s && *s == ' ')
      ++s;
    // Get the answer from direct lookup.
    GURL stored_url(s);
    GURL expected(cache_.GetCanonicalURL(stored_url));
    // Test generalization.
    GURL result(cache_.GetGeneralizedCanonicalURL(stored_url));
    EXPECT_EQ(expected, result) << " for kTopSitesSpecPrefix[" << i << "]";
  }
}

TEST_F(TopSitesCacheTest, GetGeneralizedCanonicalURL) {
  InitTopSiteCache(kTopSitesSpecPrefix, arraysize(kTopSitesSpecPrefix));
  struct {
    const char* expected;
    const char* query;
  } test_cases[] = {
    // Exact match after trimming "?query": redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.9oo91e.qjz9zk/test"},
    // Same, but different code path: redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.9oo91e.qjz9zk/test/y?e"},
    {"http://www.9oo91e.qjz9zk/", "http://www.9oo91e.qjz9zk/test/y?c"},
    // Same, but code path leads to different result: redirects.
    {"http://www.9oo91e.qjz9zk/2", "http://www.9oo91e.qjz9zk/test/y?a"},
    // Generalized match: redirects.
    {"http://www.9oo91e.qjz9zk/3", "http://www.9oo91e.qjz9zk/3/1/4/1/5/9"},
    // Generalized match with trailing "/": redirects.
    {"http://www.9oo91e.qjz9zk/3", "http://www.9oo91e.qjz9zk/3/1/4/1/5/9/"},
    // Unique generalization match: redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.ch40m1um.qjz9zk/a/b/c"},
    // Multiple exact matches after trimming: redirects to first.
    {"http://www.9oo91e.qjz9zk/2", "http://www.9oo91e.qjz9zk/test/y"},
    // Multiple generalized matches: redirects to least general.
    {"http://www.9oo91e.qjz9zk/sh", "http://www.9oo91e.qjz9zk/sh/1/2/3/4/"},
    // Multiple generalized matches: redirects to least general.
    {"http://www.9oo91e.qjz9zk/sh", "http://www.9oo91e.qjz9zk/sh/1/2/3/4/"},
    // Competing generalized match: take the most specilized.
    {"http://www.9oo91e.qjz9zk/2", "http://www.9oo91e.qjz9zk/test/q"},
    // No generalized match, early element: fails.
    {"", "http://www.a.com/"},
    // No generalized match, intermediate element: fails.
    {"", "http://www.e-is-between-chromium-and-9oo91e.qjz9zk/"},
    // No generalized match, late element: fails.
    {"", "http://www.zzzzzzz.com/"},
    // String prefix match but not URL-prefix match: fails.
    {"", "http://www.ch40m1um.qjz9zk/a/beeswax"},
    // String prefix match and URL-prefix match: redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.9oo91e.qjz9zk/shhhhhh"},
    // Different protocol: fails.
    {"", "https://www.9oo91e.qjz9zk/test"},
    // Smart enough to know that port 80 is HTTP: redirects.
    {"http://www.9oo91e.qjz9zk/", "http://www.9oo91e.qjz9zk:80/test"},
    // Specialized match only: fails.
    {"", "http://www.ch40m1um.qjz9zk/a"},
  };
  for (size_t i = 0; i < arraysize(test_cases); ++i) {
    std::string expected(test_cases[i].expected);
    std::string query(test_cases[i].query);
    GURL result(cache_.GetGeneralizedCanonicalURL(GURL(query)));
    EXPECT_EQ(expected, result.spec()) << " for test_case[" << i << "]";
  }
}

// This tests a special case where there are 2 generalized matches, and both
// should be checked to find the correct match.
TEST_F(TopSitesCacheTest, GetPrefixCanonicalURLDiffByQuery) {
  const char* top_sites_spec[] = {
    "http://www.dest.com/1",
    "  http://www.source.com/a?m=5",  // Redirects.
    "http://www.dest.com/2",
    "  http://www.source.com/a/t?q=3",  // Redirects.
  };
  InitTopSiteCache(top_sites_spec, arraysize(top_sites_spec));

  struct {
    const char* expected;
    const char* query;
  } test_cases[] = {
    // Slightly before "http://www.source.com/a?m=5".
    {"http://www.dest.com/1", "http://www.source.com/a?l=5"},
    // Slightly after "http://www.source.com/a?m=5".
    {"http://www.dest.com/1", "http://www.source.com/a?n=5"},
    // Slightly before "http://www.source.com/a/t?q=3".
    {"http://www.dest.com/2", "http://www.source.com/a/t?q=2"},
    // Slightly after "http://www.source.com/a/t?q=3".
    {"http://www.dest.com/2", "http://www.source.com/a/t?q=4"},
  };

  for (size_t i = 0; i < arraysize(test_cases); ++i) {
    std::string expected(test_cases[i].expected);
    std::string query(test_cases[i].query);
    GURL result(cache_.GetGeneralizedCanonicalURL(GURL(query)));
    EXPECT_EQ(expected, result.spec()) << " for test_case[" << i << "]";
  }
}

// This test ensures forced URLs behave in the expected way.
TEST_F(TopSitesCacheTest, CacheForcedURLs) {
  // Forced URLs must always appear at the beginning of the list.
  BuildTopSites(kTopSitesSpecBasic, arraysize(kTopSitesSpecBasic));
  top_sites_[0].last_forced_time = base::Time::FromJsTime(1000);
  top_sites_[1].last_forced_time =  base::Time::FromJsTime(2000);
  cache_.SetTopSites(top_sites_);

  EXPECT_EQ(2u, cache_.GetNumForcedURLs());
  EXPECT_EQ(2u, cache_.GetNumNonForcedURLs());
}

TEST_F(TopSitesCacheTest, ClearUnreferencedThumbnails) {
  InitTopSiteCache(kTopSitesSpecBasic, arraysize(kTopSitesSpecBasic));

  // A "primary" URL.
  const GURL url1("http://www.9oo91e.qjz9zk");
  ASSERT_TRUE(cache_.IsKnownURL(url1));
  // A URL that's part of a redirect chain.
  const GURL url2("https://www.gogle.com");
  ASSERT_TRUE(cache_.IsKnownURL(url2));

  // Add thumbnails for these two URLs.
  Images thumbnail1;
  thumbnail1.thumbnail =
      new base::RefCountedBytes(std::vector<unsigned char>());
  Images thumbnail2;
  thumbnail2.thumbnail =
      new base::RefCountedBytes(std::vector<unsigned char>());
  URLToImagesMap images;
  images[cache_.GetCanonicalURL(url1)] = thumbnail1;
  images[cache_.GetCanonicalURL(url2)] = thumbnail2;
  cache_.SetThumbnails(images);

  ASSERT_TRUE(HasPageThumbnail(url1));
  ASSERT_TRUE(HasPageThumbnail(url2));

  // Since both URLs are known, ClearUnreferencedThumbnails should do nothing.
  cache_.ClearUnreferencedThumbnails();
  EXPECT_TRUE(HasPageThumbnail(url1));
  EXPECT_TRUE(HasPageThumbnail(url2));

  // After the top sites themselves are cleared, ClearUnreferencedThumbnails
  // should clear the corresponding thumbnails.
  cache_.SetTopSites(MostVisitedURLList());
  cache_.ClearUnreferencedThumbnails();
  EXPECT_FALSE(HasPageThumbnail(url1));
  EXPECT_FALSE(HasPageThumbnail(url2));
}

}  // namespace

}  // namespace history
