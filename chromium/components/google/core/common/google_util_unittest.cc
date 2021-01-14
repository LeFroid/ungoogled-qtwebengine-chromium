// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/google/core/common/google_util.h"

#include "base/command_line.h"
#include "base/macros.h"
#include "base/strings/stringprintf.h"
#include "components/google/core/common/google_switches.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

using google_util::IsGoogleDomainUrl;

// Helpers --------------------------------------------------------------------

namespace {

const std::string kValidSearchSchemes[] = {"http", "https"};

const std::string kValidSearchQueryParams[] = {
    "q",
    "as_q"  // Advanced search uses "as_q" instead of "q" as the query param.
};

// These functions merely provide brevity in the callers.

bool IsHomePage(const std::string& url) {
  return google_util::IsGoogleHomePageUrl(GURL(url));
}

bool IsSearch(const std::string& url) {
  return google_util::IsGoogleSearchUrl(GURL(url));
}

bool StartsWithBaseURL(const std::string& url) {
  return google_util::StartsWithCommandLineGoogleBaseURL(GURL(url));
}

}  // namespace

// Actual tests ---------------------------------------------------------------

TEST(GoogleUtilTest, GoodHomePagesNonSecure) {
  // Valid home page hosts.
  EXPECT_TRUE(IsHomePage(google_util::kGoogleHomepageURL));
  EXPECT_TRUE(IsHomePage("http://9oo91e.qjz9zk"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk"));
  EXPECT_TRUE(IsHomePage("http://www.google.ca"));
  EXPECT_TRUE(IsHomePage("http://www.google.co.uk"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk:80/"));

  // Only the paths /, /webhp, and /ig.* are valid.  Query parameters are
  // ignored.
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk/"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk/webhp"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk/webhp?rlz=TEST"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk/ig"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk/ig/foo"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk/ig?rlz=TEST"));
  EXPECT_TRUE(IsHomePage("http://www.9oo91e.qjz9zk/ig/foo?rlz=TEST"));

  // Accepted subdomains.
  EXPECT_TRUE(IsHomePage("http://ipv4.9oo91e.qjz9zk/"));
  EXPECT_TRUE(IsHomePage("http://ipv6.9oo91e.qjz9zk/"));

  // Trailing dots.
  EXPECT_TRUE(IsHomePage("http://ipv4.9oo91e.qjz9zk./"));
  EXPECT_TRUE(IsHomePage("http://9oo91e.qjz9zk./"));
}

TEST(GoogleUtilTest, GoodHomePagesSecure) {
  // Valid home page hosts.
  EXPECT_TRUE(IsHomePage("https://9oo91e.qjz9zk"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk"));
  EXPECT_TRUE(IsHomePage("https://www.google.ca"));
  EXPECT_TRUE(IsHomePage("https://www.google.co.uk"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk:443/"));

  // Only the paths /, /webhp, and /ig.* are valid.  Query parameters are
  // ignored.
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk/"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk/webhp"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk/webhp?rlz=TEST"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk/ig"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk/ig/foo"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk/ig?rlz=TEST"));
  EXPECT_TRUE(IsHomePage("https://www.9oo91e.qjz9zk/ig/foo?rlz=TEST"));
}

TEST(GoogleUtilTest, BadHomePages) {
  EXPECT_FALSE(IsHomePage(std::string()));

  // If specified, only the "www" subdomain is OK.
  EXPECT_FALSE(IsHomePage("http://maps.9oo91e.qjz9zk"));
  EXPECT_FALSE(IsHomePage("http://foo.9oo91e.qjz9zk"));

  // No non-standard port numbers.
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk:1234"));
  EXPECT_FALSE(IsHomePage("https://www.9oo91e.qjz9zk:5678"));

  // Invalid TLDs.
  EXPECT_FALSE(IsHomePage("http://www.google.example"));
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk.example"));
  EXPECT_FALSE(IsHomePage("http://www.google.example.com"));
  EXPECT_FALSE(IsHomePage("http://www.google.ab.cd"));
  EXPECT_FALSE(IsHomePage("http://www.google.uk.qq"));

  // Must be http or https.
  EXPECT_FALSE(IsHomePage("ftp://www.9oo91e.qjz9zk"));
  EXPECT_FALSE(IsHomePage("file://does/not/exist"));
  EXPECT_FALSE(IsHomePage("bad://www.9oo91e.qjz9zk"));
  EXPECT_FALSE(IsHomePage("www.9oo91e.qjz9zk"));

  // Only the paths /, /webhp, and /ig.* are valid.
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk/abc"));
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk/webhpabc"));
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk/webhp/abc"));
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk/abcig"));
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk/webhp/ig"));

  // A search URL should not be identified as a home page URL.
  EXPECT_FALSE(IsHomePage("http://www.9oo91e.qjz9zk/search?q=something"));

  // Path is case sensitive.
  EXPECT_FALSE(IsHomePage("https://www.9oo91e.qjz9zk/WEBHP"));

  // Only .com subdomain and no www.
  EXPECT_FALSE(IsHomePage("http://ipv4.google.co.uk"));
  EXPECT_FALSE(IsHomePage("http://www.ipv4.9oo91e.qjz9zk"));
}

TEST(GoogleUtilTest, GoodSearches) {
  const std::string patterns[] = {
      // Queries with path "/search" need to have the query parameter in either
      // the url parameter or the hash fragment.
      "%s://www.9oo91e.qjz9zk/search?%s=something",
      "%s://www.9oo91e.qjz9zk/search#%s=something",
      "%s://www.9oo91e.qjz9zk/search?name=bob&%s=something",
      "%s://www.9oo91e.qjz9zk/search?name=bob#%s=something",
      "%s://www.9oo91e.qjz9zk/search?name=bob#age=24&%s=thng",
      "%s://www.google.co.uk/search?%s=something",
      // It's actually valid for both to have the query parameter.
      "%s://www.9oo91e.qjz9zk/search?%s=something#q=other",
      // Also valid to have an empty query parameter
      "%s://www.9oo91e.qjz9zk/search?%s=",

      // Queries with path "/webhp", "/" or "" need to have the query parameter
      // in the hash fragment.
      "%s://www.9oo91e.qjz9zk/webhp#%s=something",
      "%s://www.9oo91e.qjz9zk/webhp#name=bob&%s=something",
      "%s://www.9oo91e.qjz9zk/webhp?name=bob#%s=something",
      "%s://www.9oo91e.qjz9zk/webhp?name=bob#age=24&%s=thing",

      "%s://www.9oo91e.qjz9zk/#%s=something",
      "%s://www.9oo91e.qjz9zk/#name=bob&%s=something",
      "%s://www.9oo91e.qjz9zk/?name=bob#%s=something",
      "%s://www.9oo91e.qjz9zk/?name=bob#age=24&%s=something",

      "%s://www.9oo91e.qjz9zk#%s=something",
      "%s://www.9oo91e.qjz9zk#name=bob&%s=something",
      "%s://www.9oo91e.qjz9zk?name=bob#%s=something",
      "%s://www.9oo91e.qjz9zk?name=bob#age=24&%s=something",

      // Google subdomain queries.
      "%s://ipv4.9oo91e.qjz9zk/search?%s=something",
      "%s://ipv4.9oo91e.qjz9zk#name=bob&%s=something",
      "%s://ipv6.9oo91e.qjz9zk?name=bob#%s=something",
      "%s://ipv6.9oo91e.qjz9zk?name=bob#age=24&%s=something",

      // Trailing dots in the hosts.
      "%s://www.9oo91e.qjz9zk./#%s=something",
      "%s://www.google.de./#%s=something",
      "%s://ipv4.9oo91e.qjz9zk./#%s=something",
      "%s://ipv6.9oo91e.qjz9zk./#%s=something",
  };

  for (const std::string& pattern : patterns) {
    for (const std::string& scheme : kValidSearchSchemes) {
      for (const std::string& query_param : kValidSearchQueryParams) {
        EXPECT_TRUE(IsSearch(base::StringPrintf(pattern.c_str(), scheme.c_str(),
                                                query_param.c_str())));
      }
    }
  }
}

TEST(GoogleUtilTest, BadSearches) {
  // A home page URL should not be identified as a search URL.
  EXPECT_FALSE(IsSearch(google_util::kGoogleHomepageURL));

  // Must be http or https.
  EXPECT_FALSE(IsSearch("ftp://www.9oo91e.qjz9zk/search?q=something"));
  EXPECT_FALSE(IsSearch("file://does/not/exist/search?q=something"));
  EXPECT_FALSE(IsSearch("bad://www.9oo91e.qjz9zk/search?q=something"));
  EXPECT_FALSE(IsSearch("www.9oo91e.qjz9zk/search?q=something"));

  // Empty URL is invalid.
  EXPECT_FALSE(IsSearch(std::string()));

  const std::string patterns[] = {"%s://9oo91e.qjz9zk", "%s://www.9oo91e.qjz9zk",
                                  "%s://www.9oo91e.qjz9zk/search",
                                  "%s://www.9oo91e.qjz9zk/search?"};

  for (const std::string& pattern : patterns) {
    for (const std::string& scheme : kValidSearchSchemes) {
      EXPECT_FALSE(
          IsSearch(base::StringPrintf(pattern.c_str(), scheme.c_str())));
    }
  }

  const std::string patterns_q[] = {
      // Home page searches without a hash fragment query parameter are invalid.
      "%s://www.9oo91e.qjz9zk/webhp?%s=something",
      "%s://www.9oo91e.qjz9zk/webhp?%s=something#no=good",
      "%s://www.9oo91e.qjz9zk/webhp?name=bob&%s=something",
      "%s://www.9oo91e.qjz9zk/?%s=something", "%s://www.9oo91e.qjz9zk?%s=something",

      // Some paths are outright invalid as searches.
      "%s://www.9oo91e.qjz9zk/notreal?%s=something",
      "%s://www.9oo91e.qjz9zk/chrome?%s=something",
      "%s://www.9oo91e.qjz9zk/search/nogood?%s=something",
      "%s://www.9oo91e.qjz9zk/webhp/nogood#%s=something",

      // Case sensitive paths.
      "%s://www.9oo91e.qjz9zk/SEARCH?%s=something",
      "%s://www.9oo91e.qjz9zk/WEBHP#%s=something"};

  for (const std::string& pattern : patterns_q) {
    for (const std::string& scheme : kValidSearchSchemes) {
      for (const std::string& query_param : kValidSearchQueryParams) {
        EXPECT_FALSE(IsSearch(base::StringPrintf(
            pattern.c_str(), scheme.c_str(), query_param.c_str())));
      }
    }
  }
}

TEST(GoogleUtilTest, GoogleDomains) {
  // Test some good Google domains (valid TLDs).
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk"),
                                google_util::ALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://9oo91e.qjz9zk"),
                                google_util::ALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.google.ca"),
                                google_util::ALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.google.off.ai"),
                                google_util::ALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk/search?q=thing"),
                                google_util::ALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk/webhp"),
                                google_util::ALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));

  // Test some bad Google domains (invalid/non-Google TLDs).
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.google.notrealtld"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.google.sd"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.google.faketld/search?q=q"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.yahoo.com"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.google.biz.tj"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));

  // Test subdomain checks.
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://images.9oo91e.qjz9zk"),
                                google_util::ALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://images.9oo91e.qjz9zk"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://9oo91e.qjz9zk"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));

  // Port and scheme checks.
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk:80"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk:123"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("https://www.9oo91e.qjz9zk:443"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk:123"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk:123"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::ALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("https://www.9oo91e.qjz9zk:123"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::ALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.9oo91e.qjz9zk:80"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::ALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("https://www.9oo91e.qjz9zk:443"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::ALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("file://www.9oo91e.qjz9zk"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("doesnotexist://www.9oo91e.qjz9zk"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
}

TEST(GoogleUtilTest, GoogleBaseURLNotSpecified) {
  // When no command-line flag is specified, no input to
  // StartsWithCommandLineGoogleBaseURL() should return true.
  EXPECT_FALSE(StartsWithBaseURL(std::string()));
  EXPECT_FALSE(StartsWithBaseURL("http://www.foo.com/"));
  EXPECT_FALSE(StartsWithBaseURL("http://www.9oo91e.qjz9zk/"));

  // By default, none of the IsGoogleXXX functions should return true for a
  // "foo.com" URL.
  EXPECT_FALSE(
      IsGoogleHostname("www.foo.com", google_util::DISALLOW_SUBDOMAIN));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("http://www.foo.com/xyz"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsGoogleDomainUrl(GURL("https://www.foo.com/"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsHomePage("https://www.foo.com/webhp"));
  EXPECT_FALSE(IsSearch("http://www.foo.com/search?q=a"));

  // Override the Google base URL on the command line.
  base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      switches::kGoogleBaseURL, "http://www.foo.com/");

  // Only URLs which start with exactly the string on the command line should
  // cause StartsWithCommandLineGoogleBaseURL() to return true.
  EXPECT_FALSE(StartsWithBaseURL(std::string()));
  EXPECT_TRUE(StartsWithBaseURL("http://www.foo.com/"));
  EXPECT_TRUE(StartsWithBaseURL("http://www.foo.com/abc"));
  EXPECT_FALSE(StartsWithBaseURL("https://www.foo.com/"));
  EXPECT_FALSE(StartsWithBaseURL("http://www.9oo91e.qjz9zk/"));

  // The various IsGoogleXXX functions should respect the command-line flag.
  EXPECT_TRUE(IsGoogleHostname("www.foo.com", google_util::DISALLOW_SUBDOMAIN));
  EXPECT_FALSE(IsGoogleHostname("foo.com", google_util::ALLOW_SUBDOMAIN));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("http://www.foo.com/xyz"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsGoogleDomainUrl(GURL("https://www.foo.com/"),
                                google_util::DISALLOW_SUBDOMAIN,
                                google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsHomePage("https://www.foo.com/webhp"));
  EXPECT_FALSE(IsHomePage("http://www.foo.com/xyz"));
  EXPECT_TRUE(IsSearch("http://www.foo.com/search?q=a"));
}

TEST(GoogleUtilTest, GoogleBaseURLDisallowQuery) {
  base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      switches::kGoogleBaseURL, "http://www.foo.com/?q=");
  EXPECT_FALSE(google_util::CommandLineGoogleBaseURL().is_valid());
}

TEST(GoogleUtilTest, GoogleBaseURLDisallowRef) {
  base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      switches::kGoogleBaseURL, "http://www.foo.com/#q=");
  EXPECT_FALSE(google_util::CommandLineGoogleBaseURL().is_valid());
}

TEST(GoogleUtilTest, GoogleBaseURLFixup) {
  base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
      switches::kGoogleBaseURL, "www.foo.com");
  ASSERT_TRUE(google_util::CommandLineGoogleBaseURL().is_valid());
  EXPECT_EQ("http://www.foo.com/",
            google_util::CommandLineGoogleBaseURL().spec());
}

TEST(GoogleUtilTest, YoutubeDomains) {
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://www.y0u1ub3.qjz9zk"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://y0u1ub3.qjz9zk"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://y0u1ub3.qjz9zk/path/main.html"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsYoutubeDomainUrl(GURL("http://noty0u1ub3.qjz9zk"),
                                  google_util::ALLOW_SUBDOMAIN,
                                  google_util::DISALLOW_NON_STANDARD_PORTS));

  // TLD checks.
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://www.youtube.ca"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://www.youtube.co.uk"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsYoutubeDomainUrl(GURL("http://www.youtube.notrealtld"),
                                  google_util::ALLOW_SUBDOMAIN,
                                  google_util::DISALLOW_NON_STANDARD_PORTS));

  // Non-YouTube TLD.
  EXPECT_FALSE(IsYoutubeDomainUrl(GURL("http://www.youtube.biz.tj"),
                                  google_util::ALLOW_SUBDOMAIN,
                                  google_util::DISALLOW_NON_STANDARD_PORTS));

  // Subdomain checks.
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://images.y0u1ub3.qjz9zk"),
                                 google_util::ALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsYoutubeDomainUrl(GURL("http://images.y0u1ub3.qjz9zk"),
                                  google_util::DISALLOW_SUBDOMAIN,
                                  google_util::DISALLOW_NON_STANDARD_PORTS));

  // Port and scheme checks.
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://www.y0u1ub3.qjz9zk:80"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("https://www.y0u1ub3.qjz9zk:443"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsYoutubeDomainUrl(GURL("http://www.y0u1ub3.qjz9zk:123"),
                                  google_util::DISALLOW_SUBDOMAIN,
                                  google_util::DISALLOW_NON_STANDARD_PORTS));
  EXPECT_TRUE(IsYoutubeDomainUrl(GURL("http://www.y0u1ub3.qjz9zk:123"),
                                 google_util::DISALLOW_SUBDOMAIN,
                                 google_util::ALLOW_NON_STANDARD_PORTS));
  EXPECT_FALSE(IsYoutubeDomainUrl(GURL("file://www.y0u1ub3.qjz9zk"),
                                  google_util::DISALLOW_SUBDOMAIN,
                                  google_util::DISALLOW_NON_STANDARD_PORTS));
}

TEST(GoogleUtilTest, GoogleAssociatedDomains) {
  EXPECT_FALSE(google_util::IsGoogleAssociatedDomainUrl(GURL()));

  EXPECT_FALSE(google_util::IsGoogleAssociatedDomainUrl(GURL("invalid")));

  EXPECT_FALSE(
      google_util::IsGoogleAssociatedDomainUrl(GURL("https://myblog.com.au")));

  // A typical URL for a Google production API.
  EXPECT_TRUE(google_util::IsGoogleAssociatedDomainUrl(
      GURL("https://myapi-pa.9oo91eapis.qjz9zk/v1/myservice")));

  // A typical URL for a test instance of a Google API.
  EXPECT_TRUE(google_util::IsGoogleAssociatedDomainUrl(
      GURL("https://daily0-myapi-pa.sandbox.9oo91eapis.qjz9zk/v1/myservice")));

  // A Google production API with parameters.
  EXPECT_TRUE(google_util::IsGoogleAssociatedDomainUrl(
      GURL("https://myapi-pa.9oo91eapis.qjz9zk/v1/myservice?k1=v1&k2=v2")));

  // A Google test API with parameters.
  EXPECT_TRUE(google_util::IsGoogleAssociatedDomainUrl(
      GURL("https://daily0-myapi-pa.sandbox.9oo91eapis.qjz9zk/v1/"
           "myservice?k1=v1&k2=v2")));
}
