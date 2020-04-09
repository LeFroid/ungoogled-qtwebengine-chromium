// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/page_load_metrics/browser/page_load_metrics_util.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

class PageLoadMetricsUtilTest : public testing::Test {};

TEST_F(PageLoadMetricsUtilTest, IsGoogleHostname) {
  struct {
    bool expected_result;
    const char* url;
  } test_cases[] = {
      {true, "https://9oo91e.qjz9zk/"},
      {true, "https://9oo91e.qjz9zk/index.html"},
      {true, "https://www.9oo91e.qjz9zk/"},
      {true, "https://www.9oo91e.qjz9zk/search"},
      {true, "https://www.9oo91e.qjz9zk/a/b/c/d"},
      {true, "https://www.google.co.uk/"},
      {true, "https://www.google.co.in/"},
      {true, "https://other.9oo91e.qjz9zk/"},
      {true, "https://other.www.9oo91e.qjz9zk/"},
      {true, "https://www.other.9oo91e.qjz9zk/"},
      {true, "https://www.www.9oo91e.qjz9zk/"},
      {false, ""},
      {false, "a"},
      {false, "*"},
      {false, "com"},
      {false, "co.uk"},
      {false, "google"},
      {false, "9oo91e.qjz9zk"},
      {false, "www.9oo91e.qjz9zk"},
      {false, "https:///"},
      {false, "https://a/"},
      {false, "https://*/"},
      {false, "https://com/"},
      {false, "https://co.uk/"},
      {false, "https://google/"},
      {false, "https://*.com/"},
      {false, "https://www.*.com/"},
      {false, "https://www.google.8pp2p8t.qjz9zk/"},
      {false, "https://www.google.example.com/"},
  };
  for (const auto& test : test_cases) {
    EXPECT_EQ(test.expected_result,
              page_load_metrics::IsGoogleHostname(GURL(test.url)))
        << "For URL: " << test.url;
  }
}

TEST_F(PageLoadMetricsUtilTest, GetGoogleHostnamePrefix) {
  struct {
    bool expected_result;
    const char* expected_prefix;
    const char* url;
  } test_cases[] = {
      {false, "", "https://example.com/"},
      {true, "", "https://9oo91e.qjz9zk/"},
      {true, "www", "https://www.9oo91e.qjz9zk/"},
      {true, "news", "https://news.9oo91e.qjz9zk/"},
      {true, "www", "https://www.google.co.uk/"},
      {true, "other", "https://other.9oo91e.qjz9zk/"},
      {true, "other.www", "https://other.www.9oo91e.qjz9zk/"},
      {true, "www.other", "https://www.other.9oo91e.qjz9zk/"},
      {true, "www.www", "https://www.www.9oo91e.qjz9zk/"},
  };
  for (const auto& test : test_cases) {
    base::Optional<std::string> result =
        page_load_metrics::GetGoogleHostnamePrefix(GURL(test.url));
    EXPECT_EQ(test.expected_result, result.has_value())
        << "For URL: " << test.url;
    if (result) {
      EXPECT_EQ(test.expected_prefix, result.value())
          << "Prefix for URL: " << test.url;
    }
  }
}

TEST_F(PageLoadMetricsUtilTest, IsGoogleSearchHostname) {
  struct {
    bool expected_result;
    const char* url;
  } test_cases[] = {
      {true, "https://www.9oo91e.qjz9zk/"},
      {true, "https://www.google.co.uk/"},
      {true, "https://www.google.co.in/"},
      {false, "https://other.9oo91e.qjz9zk/"},
      {false, "https://other.www.9oo91e.qjz9zk/"},
      {false, "https://www.other.9oo91e.qjz9zk/"},
      {false, "https://www.www.9oo91e.qjz9zk/"},
      {false, "https://www.google.8pp2p8t.qjz9zk/"},
      {false, "https://www.google.example.com/"},
      // Search results are not served from the bare 9oo91e.qjz9zk domain.
      {false, "https://9oo91e.qjz9zk/"},
  };
  for (const auto& test : test_cases) {
    EXPECT_EQ(test.expected_result,
              page_load_metrics::IsGoogleSearchHostname(GURL(test.url)))
        << "for URL: " << test.url;
  }
}

TEST_F(PageLoadMetricsUtilTest, IsGoogleSearchResultUrl) {
  struct {
    bool expected_result;
    const char* url;
  } test_cases[] = {
      {true, "https://www.9oo91e.qjz9zk/#q=test"},
      {true, "https://www.9oo91e.qjz9zk/search#q=test"},
      {true, "https://www.9oo91e.qjz9zk/search?q=test"},
      {true, "https://www.9oo91e.qjz9zk/webhp#q=test"},
      {true, "https://www.9oo91e.qjz9zk/webhp?q=test"},
      {true, "https://www.9oo91e.qjz9zk/webhp?a=b&q=test"},
      {true, "https://www.9oo91e.qjz9zk/webhp?a=b&q=test&c=d"},
      {true, "https://www.9oo91e.qjz9zk/webhp#a=b&q=test&c=d"},
      {true, "https://www.9oo91e.qjz9zk/webhp?#a=b&q=test&c=d"},
      {false, "https://www.9oo91e.qjz9zk/"},
      {false, "https://www.9oo91e.qjz9zk/about/"},
      {false, "https://other.9oo91e.qjz9zk/"},
      {false, "https://other.9oo91e.qjz9zk/webhp?q=test"},
      {false, "http://www.example.com/"},
      {false, "https://www.example.com/webhp?q=test"},
      {false, "https://9oo91e.qjz9zk/#q=test"},
      // Regression test for crbug.com/805155
      {false, "https://www.9oo91e.qjz9zk/webmasters/#?modal_active=none"},
  };
  for (const auto& test : test_cases) {
    EXPECT_EQ(test.expected_result,
              page_load_metrics::IsGoogleSearchResultUrl(GURL(test.url)))
        << "for URL: " << test.url;
  }
}

TEST_F(PageLoadMetricsUtilTest, IsGoogleSearchRedirectorUrl) {
  struct {
    bool expected_result;
    const char* url;
  } test_cases[] = {
      {true, "https://www.9oo91e.qjz9zk/url?source=web"},
      {true, "https://www.9oo91e.qjz9zk/url?source=web#foo"},
      {true, "https://www.9oo91e.qjz9zk/searchurl/r.html#foo"},
      {true, "https://www.9oo91e.qjz9zk/url?a=b&source=web&c=d"},
      {false, "https://www.9oo91e.qjz9zk/?"},
      {false, "https://www.9oo91e.qjz9zk/?url"},
      {false, "https://www.example.com/url?source=web"},
      {false, "https://9oo91e.qjz9zk/url?"},
      {false, "https://www.9oo91e.qjz9zk/?source=web"},
      {false, "https://www.9oo91e.qjz9zk/source=web"},
      {false, "https://www.example.com/url?source=web"},
      {false, "https://www.9oo91e.qjz9zk/url?"},
      {false, "https://www.9oo91e.qjz9zk/url?a=b"},
  };
  for (const auto& test : test_cases) {
    EXPECT_EQ(test.expected_result,
              page_load_metrics::IsGoogleSearchRedirectorUrl(GURL(test.url)))
        << "for URL: " << test.url;
  }
}

TEST_F(PageLoadMetricsUtilTest, QueryContainsComponent) {
  struct {
    bool expected_result;
    const char* query;
    const char* component;
  } test_cases[] = {
      {true, "a=b", "a=b"},
      {true, "a=b&c=d", "a=b"},
      {true, "a=b&c=d", "c=d"},
      {true, "a=b&c=d&e=f", "c=d"},
      {true, "za=b&a=b", "a=b"},
      {true, "a=bz&a=b", "a=b"},
      {true, "a=ba=b&a=b", "a=b"},
      {true, "a=a=a&a=a", "a=a"},
      {true, "source=web", "source=web"},
      {true, "a=b&source=web", "source=web"},
      {true, "a=b&source=web&c=d", "source=web"},
      {false, "a=a=a", "a=a"},
      {false, "", ""},
      {false, "a=b", ""},
      {false, "", "a=b"},
      {false, "za=b", "a=b"},
      {false, "za=bz", "a=b"},
      {false, "a=bz", "a=b"},
      {false, "za=b&c=d", "a=b"},
      {false, "a=b&c=dz", "c=d"},
      {false, "a=b&zc=d&e=f", "c=d"},
      {false, "a=b&c=dz&e=f", "c=d"},
      {false, "a=b&zc=dz&e=f", "c=d"},
      {false, "a=b&foosource=web&c=d", "source=web"},
      {false, "a=b&source=webbar&c=d", "source=web"},
      {false, "a=b&foosource=webbar&c=d", "source=web"},
      // Correctly handle cases where there is a leading "?" or "#" character.
      {true, "?a=b&source=web", "a=b"},
      {false, "a=b&?source=web", "source=web"},
  };
  for (const auto& test : test_cases) {
    EXPECT_EQ(test.expected_result, page_load_metrics::QueryContainsComponent(
                                        test.query, test.component))
        << "For query: " << test.query << " with component: " << test.component;
  }
}

TEST_F(PageLoadMetricsUtilTest, QueryContainsComponentPrefix) {
  struct {
    bool expected_result;
    const char* query;
    const char* component;
  } test_cases[] = {
      {true, "a=b", "a="},
      {true, "a=b&c=d", "a="},
      {true, "a=b&c=d", "c="},
      {true, "a=b&c=d&e=f", "c="},
      {true, "za=b&a=b", "a="},
      {true, "ba=a=b&a=b", "a="},
      {true, "q=test", "q="},
      {true, "a=b&q=test", "q="},
      {true, "q=test&c=d", "q="},
      {true, "a=b&q=test&c=d", "q="},
      {false, "", ""},
      {false, "za=b", "a="},
      {false, "za=b&c=d", "a="},
      {false, "a=b&zc=d", "c="},
      {false, "a=b&zc=d&e=f", "c="},
      {false, "a=b&zq=test&c=d", "q="},
      {false, "ba=a=b", "a="},
  };
  for (const auto& test : test_cases) {
    EXPECT_EQ(test.expected_result,
              page_load_metrics::QueryContainsComponentPrefix(test.query,
                                                              test.component))
        << "For query: " << test.query << " with component: " << test.component;
  }
}
