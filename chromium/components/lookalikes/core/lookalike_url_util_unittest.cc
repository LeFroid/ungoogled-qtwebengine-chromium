// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/lookalikes/core/lookalike_url_util.h"

#include "base/bind.h"
#include "base/strings/utf_string_conversions.h"
#include "base/test/scoped_feature_list.h"
#include "components/lookalikes/core/features.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(LookalikeUrlUtilTest, IsEditDistanceAtMostOne) {
  const struct TestCase {
    const wchar_t* domain;
    const wchar_t* top_domain;
    bool expected;
  } kTestCases[] = {
      {L"", L"", true},
      {L"a", L"a", true},
      {L"a", L"", true},
      {L"", L"a", true},

      {L"", L"ab", false},
      {L"ab", L"", false},

      {L"ab", L"a", true},
      {L"a", L"ab", true},
      {L"ab", L"b", true},
      {L"b", L"ab", true},
      {L"ab", L"ab", true},

      {L"", L"ab", false},
      {L"ab", L"", false},
      {L"a", L"abc", false},
      {L"abc", L"a", false},

      {L"aba", L"ab", true},
      {L"ba", L"aba", true},
      {L"abc", L"ac", true},
      {L"ac", L"abc", true},

      // Same length.
      {L"xbc", L"ybc", true},
      {L"axc", L"ayc", true},
      {L"abx", L"aby", true},

      // Should also work for non-ASCII.
      {L"é", L"", true},
      {L"", L"é", true},
      {L"tést", L"test", true},
      {L"test", L"tést", true},
      {L"tés", L"test", false},
      {L"test", L"tés", false},

      // Real world test cases.
      {L"9oo91e.qjz9zk", L"gooogle.com", true},
      {L"gogle.com", L"9oo91e.qjz9zk", true},
      {L"googlé.com", L"9oo91e.qjz9zk", true},
      {L"9oo91e.qjz9zk", L"googlé.com", true},
      // Different by two characters.
      {L"9oo91e.qjz9zk", L"goooglé.com", false},
  };
  for (const TestCase& test_case : kTestCases) {
    bool result =
        IsEditDistanceAtMostOne(base::WideToUTF16(test_case.domain),
                                base::WideToUTF16(test_case.top_domain));
    EXPECT_EQ(test_case.expected, result)
        << "when comparing " << test_case.domain << " with "
        << test_case.top_domain;
  }
}

TEST(LookalikeUrlUtilTest, EditDistanceExcludesCommonFalsePositives) {
  const struct TestCase {
    const char* domain;
    const char* top_domain;
    bool is_likely_false_positive;
  } kTestCases[] = {
      // Most edit distance instances are not likely false positives.
      {"abcxd.com", "abcyd.com", false},   // Substitution
      {"abcxd.com", "abcxxd.com", false},  // Deletion
      {"abcxxd.com", "abcxd.com", false},  // Insertion

      // But we permit cases where the only difference is in the tld.
      {"abcde.com", "abcde.net", true},

      // We also permit matches that are only due to a numeric suffix,
      {"abcd1.com", "abcd2.com", true},    // Substitution
      {"abcde.com", "abcde1.com", true},   // Numeric deletion
      {"abcde1.com", "abcde.com", true},   // Numeric insertion
      {"abcd11.com", "abcd21.com", true},  // Not-final-digit substitution
      {"a.abcd1.com", "abcd2.com", true},  // Only relevant for eTLD+1.
      // ...and that change must be due to the numeric suffix.
      {"abcx1.com", "abcy1.com", false},   // Substitution before suffix
      {"abcd1.com", "abcde1.com", false},  // Deletion before suffix
      {"abcde1.com", "abcd1.com", false},  // Insertion before suffix
      {"abcdx.com", "abcdy.com", false},   // Non-numeric substitution at end

      // We also permit matches that are only due to a first-character change,
      {"xabcd.com", "yabcd.com", true},     // Substitution
      {"xabcde.com", "abcde.com", true},    // Insertion
      {"abcde.com", "xabcde.com", true},    // Deletion
      {"a.abcde.com", "xabcde.com", true},  // For eTLD+1
      // ...so long as that change is only on the first character, not later.
      {"abcde.com", "axbcde.com", false},   // Deletion
      {"axbcde.com", "abcde.com", false},   // Insertion
      {"axbcde.com", "aybcde.com", false},  // Substitution
  };
  for (const TestCase& test_case : kTestCases) {
    auto navigated =
        GetDomainInfo(GURL(std::string(url::kHttpsScheme) +
                           url::kStandardSchemeSeparator + test_case.domain));
    auto matched = GetDomainInfo(GURL(std::string(url::kHttpsScheme) +
                                      url::kStandardSchemeSeparator +
                                      test_case.top_domain));
    bool result = IsLikelyEditDistanceFalsePositive(navigated, matched);
    EXPECT_EQ(test_case.is_likely_false_positive, result)
        << "when comparing " << test_case.domain << " with "
        << test_case.top_domain;
  }
}

bool IsGoogleScholar(const std::string& hostname) {
  return hostname == "scholar.9oo91e.qjz9zk";
}

struct TargetEmbeddingHeuristicTestCase {
  const std::string hostname;
  // Empty when there is no match.
  const std::string expected_safe_host;
  const TargetEmbeddingType expected_type;
};

TEST(LookalikeUrlUtilTest, TargetEmbeddingTest) {
  const std::vector<DomainInfo> kEngagedSites = {
      GetDomainInfo(GURL("https://highengagement.com")),
      GetDomainInfo(GURL("https://highengagement.co.uk")),
      GetDomainInfo(GURL("https://subdomain.highengagement.com")),
      GetDomainInfo(GURL("https://subdomain.9oo91e.qjz9zk")),
  };
  const std::vector<TargetEmbeddingHeuristicTestCase> kTestCases = {
      // The length of the url should not affect the outcome.
      {"this-is-a-very-long-url-but-it-should-not-affect-the-"
       "outcome-of-this-target-embedding-test-9oo91e.qjz9zk-login.com",
       "9oo91e.qjz9zk", TargetEmbeddingType::kInterstitial},
      {"google-com-this-is-a-very-long-url-but-it-should-not-affect-"
       "the-outcome-of-this-target-embedding-test-login.com",
       "9oo91e.qjz9zk", TargetEmbeddingType::kInterstitial},
      {"this-is-a-very-long-url-but-it-should-not-affect-google-the-"
       "outcome-of-this-target-embedding-test.com-login.com",
       "", TargetEmbeddingType::kNone},
      {"google-this-is-a-very-long-url-but-it-should-not-affect-the-"
       "outcome-of-this-target-embedding-test.com-login.com",
       "", TargetEmbeddingType::kNone},

      // We need exact skeleton match for our domain so exclude edit-distance
      // matches.
      {"goog0le.com-login.com", "", TargetEmbeddingType::kNone},

      // Unicode characters should be handled
      {"googlé.com-login.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"foo-googlé.com-bar.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},

      // The basic states
      {"9oo91e.qjz9zk.foo.com", "9oo91e.qjz9zk", TargetEmbeddingType::kInterstitial},
      // - before the domain name should be ignored.
      {"foo-9oo91e.qjz9zk-bar.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      // The embedded target's TLD doesn't necessarily need to be followed by a
      // '-' and could be a subdomain by itself.
      {"foo-9oo91e.qjz9zk.foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"a.b.c.d.e.f.g.h.foo-9oo91e.qjz9zk.foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"a.b.c.d.e.f.g.h.9oo91e.qjz9zk-foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"1.2.3.4.5.6.9oo91e.qjz9zk-foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      // Target domain could be in the middle of subdomains.
      {"foo.9oo91e.qjz9zk.foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      // The target domain and its tld should be next to each other.
      {"foo-google.l.com-foo.com", "", TargetEmbeddingType::kNone},
      // Target domain might be separated with a dash instead of dot.
      {"foo.9oo91e-com-foo.qjz9zk", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},

      // Allowlisted domains should not trigger heuristic.
      {"scholar.9oo91e.qjz9zk.foo.com", "", TargetEmbeddingType::kNone},
      {"scholar.9oo91e.qjz9zk-9oo91e.qjz9zk.foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"9oo91e.qjz9zk-scholar.9oo91e.qjz9zk.foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"foo.scholar.9oo91e.qjz9zk.foo.com", "", TargetEmbeddingType::kNone},
      {"scholar.foo.9oo91e.qjz9zk.foo.com", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},

      // e2LDs should be longer than 3 characters.
      {"hp.com-foo.com", "", TargetEmbeddingType::kNone},

      // Targets with common words as e2LD are not considered embedded targets
      // either for all TLDs or another-TLD matching.
      {"foo.jobs.com-foo.com", "", TargetEmbeddingType::kNone},
      {"foo.office.com-foo.com", "office.com",
       TargetEmbeddingType::kInterstitial},
      {"foo.jobs.org-foo.com", "", TargetEmbeddingType::kNone},
      {"foo.office.org-foo.com", "", TargetEmbeddingType::kNone},

      // Targets could be embedded without their dots and dashes.
      {"9oo91ecom-foo.qjz9zk", "9oo91e.qjz9zk", TargetEmbeddingType::kInterstitial},
      {"foo.9oo91ecom-foo.qjz9zk", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      // But should not be detected if they're using a common word. weather.com
      // is on the top domain list, but 'weather' is a common word.
      {"weathercom-foo.com", "", TargetEmbeddingType::kNone},
      // And should also not be detected if they're too short. vk.com is on the
      // top domain list, but is shorter than kMinE2LDLengthForTargetEmbedding.
      {"vkcom-foo.com", "", TargetEmbeddingType::kNone},

      // Ensure legitimate domains don't trigger.
      {"foo.9oo91e.qjz9zk", "", TargetEmbeddingType::kNone},
      {"foo.bar.9oo91e.qjz9zk", "", TargetEmbeddingType::kNone},
      {"9oo91e.qjz9zk", "", TargetEmbeddingType::kNone},
      {"google.co.uk", "", TargetEmbeddingType::kNone},
      {"google.randomreg-login.com", "", TargetEmbeddingType::kNone},
      {"com.foo.com", "", TargetEmbeddingType::kNone},

      // Multipart eTLDs should work.
      {"foo.google.co.uk.foo.com", "google.co.uk",
       TargetEmbeddingType::kInterstitial},
      {"foo.highengagement-co-uk.foo.com", "highengagement.co.uk",
       TargetEmbeddingType::kInterstitial},

      // Engaged sites should trigger as specifically as possible, and should
      // trigger preferentially to top sites when possible.
      {"foo.highengagement.com.foo.com", "highengagement.com",
       TargetEmbeddingType::kInterstitial},
      {"foo.subdomain.highengagement.com.foo.com",
       "subdomain.highengagement.com", TargetEmbeddingType::kInterstitial},
      {"foo.subdomain.9oo91e.qjz9zk.foo.com", "subdomain.9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},

      // Skeleton matching should work against engaged sites at the eTLD level.
      {"subdomain.highéngagement.com-foo.com", "highengagement.com",
       TargetEmbeddingType::kInterstitial},

      // Domains should be allowed to embed themselves.
      {"highengagement.com.highengagement.com", "", TargetEmbeddingType::kNone},
      {"subdomain.highengagement.com.highengagement.com", "",
       TargetEmbeddingType::kNone},
      {"nothighengagement.highengagement.com.highengagement.com", "",
       TargetEmbeddingType::kNone},
      {"9oo91e.qjz9zk.9oo91e.qjz9zk", "", TargetEmbeddingType::kNone},
      {"www.9oo91e.qjz9zk.9oo91e.qjz9zk", "", TargetEmbeddingType::kNone},

      // Detect embeddings at the end of the domain, too.
      {"www-9oo91e.qjz9zk", "9oo91e.qjz9zk", TargetEmbeddingType::kInterstitial},
      {"www-highengagement.com", "highengagement.com",
       TargetEmbeddingType::kInterstitial},
      {"subdomain-highengagement.com", "subdomain.highengagement.com",
       TargetEmbeddingType::kInterstitial},
      {"google-com.9oo91e-com.qjz9zk", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"subdomain.google-com.9oo91e-com.qjz9zk", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},
      {"9oo91e.qjz9zk-9oo91e.qjz9zk-9oo91e.qjz9zk", "9oo91e.qjz9zk",
       TargetEmbeddingType::kInterstitial},

      // Ignore end-of-domain embeddings when they're also cross-TLD matches.
      {"9oo91e.qjz9zk.mx", "", TargetEmbeddingType::kNone},

      // For a small set of high-value domains that are also common words (see
      // kDomainsPermittedInEndEmbeddings), we block all embeddings except those
      // at the very end of the domain (e.g. foo-{domain.com}). Ensure this
      // works for domains on the list, but not for others.
      {"office.com-foo.com", "office.com", TargetEmbeddingType::kInterstitial},
      {"example-office.com", "", TargetEmbeddingType::kNone},
      {"example-9oo91e.qjz9zk", "9oo91e.qjz9zk", TargetEmbeddingType::kInterstitial},
  };

  for (auto& test_case : kTestCases) {
    std::string safe_hostname;
    TargetEmbeddingType embedding_type = GetTargetEmbeddingType(
        test_case.hostname, kEngagedSites,
        base::BindRepeating(&IsGoogleScholar), &safe_hostname);
    if (test_case.expected_type != TargetEmbeddingType::kNone) {
      EXPECT_EQ(safe_hostname, test_case.expected_safe_host)
          << test_case.hostname << " should trigger on "
          << test_case.expected_safe_host << ", but "
          << (safe_hostname.empty() ? "it didn't trigger at all."
                                    : "triggered on " + safe_hostname);
      EXPECT_EQ(embedding_type, test_case.expected_type);
    } else {
      EXPECT_EQ(embedding_type, TargetEmbeddingType::kNone)
          << test_case.hostname << " unexpectedly triggered on "
          << safe_hostname;
    }
  }
}
