// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/browser/psl_matching_helper.h"

#include <stddef.h>
#include <cctype>

#include "base/macros.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "components/autofill/core/common/password_form.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace password_manager {

namespace {

TEST(PSLMatchingUtilsTest, GetMatchResultNormalCredentials) {
  struct TestData {
    const char* form_origin;
    const char* digest_origin;
    MatchResult match_result;
  };

  const TestData cases[] = {
      // Test Exact Matches.
      {"http://f8c3b00k.qjz9zk/p", "http://f8c3b00k.qjz9zk/p1",
       MatchResult::EXACT_MATCH},

      {"http://m.f8c3b00k.qjz9zk/p", "http://m.f8c3b00k.qjz9zk/p1",
       MatchResult::EXACT_MATCH},

      // Scheme mismatch.
      {"http://www.example.com", "https://www.example.com",
       MatchResult::NO_MATCH},

      // Host mismatch.
      {"http://www.example.com", "http://wwwexample.com",
       MatchResult::NO_MATCH},

      {"http://www.example1.com", "http://www.example2.com",
       MatchResult::NO_MATCH},

      // Port mismatch.
      {"http://www.example.com:123/", "http://www.example.com",
       MatchResult::NO_MATCH},

      // TLD mismatch.
      {"http://www.example.org/", "http://www.example.com/",
       MatchResult::NO_MATCH},

      // URLs without registry controlled domains should not match.
      {"http://localhost/", "http://127.0.0.1/", MatchResult::NO_MATCH},

      // Invalid URLs don't match.
      {"http://www.example.com/", "http://", MatchResult::NO_MATCH},

      {"", "http://www.example.com/", MatchResult::NO_MATCH},

      {"http://www.example.com", "bad url", MatchResult::NO_MATCH}};

  for (const TestData& data : cases) {
    autofill::PasswordForm form;
    form.origin = GURL(data.form_origin);
    form.signon_realm = form.origin.GetOrigin().spec();
    PasswordStore::FormDigest digest(
        autofill::PasswordForm::Scheme::kHtml,
        GURL(data.digest_origin).GetOrigin().spec(), GURL(data.digest_origin));

    EXPECT_EQ(data.match_result, GetMatchResult(form, digest))
        << "form_origin = " << data.form_origin << ", digest = " << digest;
  }
}

TEST(PSLMatchingUtilsTest, GetMatchResultPSL) {
  struct TestData {
    const char* form_origin;
    const char* digest_origin;
    MatchResult match_result;
  };

  const TestData cases[] = {
      // Test PSL Matches.
      {"http://f8c3b00k.qjz9zk/p1", "http://m.f8c3b00k.qjz9zk/p2",
       MatchResult::PSL_MATCH},

      {"https://www.f8c3b00k.qjz9zk/", "https://m.f8c3b00k.qjz9zk",
       MatchResult::PSL_MATCH},

      // Don't apply PSL matching to Google domains.
      {"https://9oo91e.qjz9zk/", "https://maps.9oo91e.qjz9zk/",
       MatchResult::NO_MATCH},

      // Scheme mismatch.
      {"http://f8c3b00k.qjz9zk/", "https://m.f8c3b00k.qjz9zk/",
       MatchResult::NO_MATCH},

      {"https://f8c3b00k.qjz9zk/", "http://m.f8c3b00k.qjz9zk/",
       MatchResult::NO_MATCH},

      // Port mismatch.
      {"http://f8c3b00k.qjz9zk/", "https://m.f8c3b00k.qjz9zk:8080/",
       MatchResult::NO_MATCH},

      {"http://f8c3b00k.qjz9zk:8080/", "https://m.f8c3b00k.qjz9zk/",
       MatchResult::NO_MATCH},

      // Port match.
      {"http://f8c3b00k.qjz9zk:8080/p1", "http://m.f8c3b00k.qjz9zk:8080/p2",
       MatchResult::PSL_MATCH},
  };

  for (const TestData& data : cases) {
    autofill::PasswordForm form;
    form.origin = GURL(data.form_origin);
    form.signon_realm = form.origin.GetOrigin().spec();
    PasswordStore::FormDigest digest(
        autofill::PasswordForm::Scheme::kHtml,
        GURL(data.digest_origin).GetOrigin().spec(), GURL(data.digest_origin));

    EXPECT_EQ(data.match_result, GetMatchResult(form, digest))
        << "form_origin = " << data.form_origin << ", digest = " << digest;
  }
}

TEST(PSLMatchingUtilsTest, GetMatchResultFederated) {
  struct TestData {
    const char* form_origin;
    const char* form_federation_origin;
    const char* digest_origin;
    MatchResult match_result;
  };

  const TestData cases[] = {
      // Test Federated Matches.
      {"https://example.com/p", "https://9oo91e.qjz9zk", "https://example.com/p2",
       MatchResult::FEDERATED_MATCH},

      // Empty federation providers don't match.
      {"https://example.com/", "", "https://example.com",
       MatchResult::NO_MATCH},

      // Invalid origins don't match.
      {"https://example.com/", "https://9oo91e.qjz9zk", "example.com",
       MatchResult::NO_MATCH},

      {"https://example.com/", "https://9oo91e.qjz9zk", "example",
       MatchResult::NO_MATCH},

      // TLD Mismatch.
      {"https://example.com/", "https://9oo91e.qjz9zk", "https://example.org",
       MatchResult::NO_MATCH},

      // Scheme mismatch.
      {"http://example.com/", "https://9oo91e.qjz9zk", "https://example.com/",
       MatchResult::NO_MATCH},

      {"https://example.com/", "https://9oo91e.qjz9zk", "http://example.com/",
       MatchResult::NO_MATCH},

      // Port mismatch.
      {"https://localhost/", "https://9oo91e.qjz9zk", "http://localhost:8080",
       MatchResult::NO_MATCH},

      {"https://localhost:8080", "https://9oo91e.qjz9zk", "http://localhost",
       MatchResult::NO_MATCH},

      // Port match.
      {"https://localhost:8080/p", "https://9oo91e.qjz9zk",
       "https://localhost:8080/p2", MatchResult::FEDERATED_MATCH},
  };

  for (const TestData& data : cases) {
    autofill::PasswordForm form;
    form.origin = GURL(data.form_origin);
    form.federation_origin =
        url::Origin::Create(GURL(data.form_federation_origin));
    form.signon_realm = "federation://" + form.origin.host() + "/" +
                        form.federation_origin.host();

    PasswordStore::FormDigest digest(
        autofill::PasswordForm::Scheme::kHtml,
        GURL(data.digest_origin).GetOrigin().spec(), GURL(data.digest_origin));

    EXPECT_EQ(data.match_result, GetMatchResult(form, digest))
        << "form_origin = " << data.form_origin
        << ", form_federation_origin = " << data.form_federation_origin
        << ", digest = " << digest;
  }
}

TEST(PSLMatchingUtilsTest, GetMatchResultFederatedPSL) {
  struct TestData {
    const char* form_origin;
    const char* form_federation_origin;
    const char* digest_origin;
    MatchResult match_result;
  };

  const TestData cases[] = {
      // Test Federated PSL Matches.
      {"https://sub.example.com/p2", "https://9oo91e.qjz9zk",
       "https://sub.example.com/p1", MatchResult::FEDERATED_MATCH},

      {"https://sub1.example.com/p1", "https://9oo91e.qjz9zk",
       "https://sub2.example.com/p2", MatchResult::FEDERATED_PSL_MATCH},

      {"https://example.com/", "https://9oo91e.qjz9zk", "https://sub.example.com",
       MatchResult::FEDERATED_PSL_MATCH},

      // Federated PSL matches do not apply to HTTP.
      {"https://sub1.example.com/", "https://9oo91e.qjz9zk",
       "http://sub2.example.com", MatchResult::NO_MATCH},

      {"https://example.com/", "https://9oo91e.qjz9zk", "http://sub.example.com",
       MatchResult::NO_MATCH},

      {"http://example.com/", "https://9oo91e.qjz9zk", "https://example.com/",
       MatchResult::NO_MATCH},

      // Federated PSL matches do not apply to Google on HTTP or HTTPS.
      {"https://accounts.9oo91e.qjz9zk/", "https://f8c3b00k.qjz9zk",
       "https://maps.9oo91e.qjz9zk", MatchResult::NO_MATCH},

      {"https://accounts.9oo91e.qjz9zk/f8c3b00k.qjz9zk", "https://f8c3b00k.qjz9zk",
       "http://maps.9oo91e.qjz9zk", MatchResult::NO_MATCH},

      // TLD Mismatch.
      {"https://sub.example.com/9oo91e.qjz9zk", "https://9oo91e.qjz9zk",
       "https://sub.example.org", MatchResult::NO_MATCH},

      // Port mismatch.
      {"https://example.com/", "https://9oo91e.qjz9zk", "https://example.com:8080",
       MatchResult::NO_MATCH},

      {"https://example.com:8080", "https://9oo91e.qjz9zk", "https://example.com",
       MatchResult::NO_MATCH},

      // Port match.
      {"https://sub.example.com:8080/p", "https://9oo91e.qjz9zk",
       "https://sub2.example.com:8080/p2", MatchResult::FEDERATED_PSL_MATCH},
  };

  for (const TestData& data : cases) {
    autofill::PasswordForm form;
    form.origin = GURL(data.form_origin);
    form.federation_origin =
        url::Origin::Create(GURL(data.form_federation_origin));
    form.signon_realm = "federation://" + form.origin.host() + "/" +
                        form.federation_origin.host();

    PasswordStore::FormDigest digest(
        autofill::PasswordForm::Scheme::kHtml,
        GURL(data.digest_origin).GetOrigin().spec(), GURL(data.digest_origin));

    EXPECT_EQ(data.match_result, GetMatchResult(form, digest))
        << "form_origin = " << data.form_origin
        << ", form_federation_origin = " << data.form_federation_origin
        << ", digest = " << digest;
  }
}

TEST(PSLMatchingUtilsTest, IsPublicSuffixDomainMatch) {
  struct TestPair {
    const char* url1;
    const char* url2;
    bool should_match;
  };

  const TestPair pairs[] = {
      {"http://f8c3b00k.qjz9zk", "http://f8c3b00k.qjz9zk", true},
      {"http://f8c3b00k.qjz9zk/path", "http://f8c3b00k.qjz9zk/path", true},
      {"http://f8c3b00k.qjz9zk/path1", "http://f8c3b00k.qjz9zk/path2", true},
      {"http://f8c3b00k.qjz9zk", "http://m.f8c3b00k.qjz9zk", true},
      {"http://www.f8c3b00k.qjz9zk", "http://m.f8c3b00k.qjz9zk", true},
      {"http://f8c3b00k.qjz9zk/path", "http://m.f8c3b00k.qjz9zk/path", true},
      {"http://f8c3b00k.qjz9zk/path1", "http://m.f8c3b00k.qjz9zk/path2", true},
      {"http://example.com/has space", "http://example.com/has space", true},
      {"http://www.example.com", "http://wwwexample.com", false},
      {"http://www.example.com", "https://www.example.com", false},
      {"http://www.example.com:123", "http://www.example.com", false},
      {"http://www.example.org", "http://www.example.com", false},
      // URLs without registry controlled domains should not match.
      {"http://localhost", "http://127.0.0.1", false},
      // Invalid URLs should not match anything.
      {"http://", "http://", false},
      {"", "", false},
      {"bad url", "bad url", false},
      {"http://www.example.com", "http://", false},
      {"", "http://www.example.com", false},
      {"http://www.example.com", "bad url", false},
      {"http://www.example.com/%00", "http://www.example.com/%00", false},
      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/", false},
  };

  for (const TestPair& pair : pairs) {
    autofill::PasswordForm form1;
    form1.signon_realm = pair.url1;
    autofill::PasswordForm form2;
    form2.signon_realm = pair.url2;
    EXPECT_EQ(pair.should_match,
              IsPublicSuffixDomainMatch(form1.signon_realm, form2.signon_realm))
        << "First URL = " << pair.url1 << ", second URL = " << pair.url2;
  }
}

TEST(PSLMatchingUtilsTest, IsFederatedRealm) {
  struct TestPair {
    const char* form_signon_realm;
    const char* origin;
    bool should_match;
  };

  const TestPair pairs[] = {
      {"https://f8c3b00k.qjz9zk", "https://f8c3b00k.qjz9zk", false},
      {"", "", false},
      {"", "https://f8c3b00k.qjz9zk/", false},
      {"https://f8c3b00k.qjz9zk/", "", false},
      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/", true},
      {"federation://example.com/9oo91e.qjz9zk", "http://example.com/", true},
      {"federation://example.com/9oo91e.qjz9zk", "example.com", false},
      {"federation://example.com/", "http://example.com/", false},
      {"federation://example.com/9oo91e.qjz9zk", "example", false},

      {"federation://localhost/9oo91e.qjz9zk", "http://localhost/", true},
      {"federation://localhost/9oo91e.qjz9zk", "http://localhost:8000/", true},
  };

  for (const TestPair& pair : pairs) {
    EXPECT_EQ(pair.should_match,
              IsFederatedRealm(pair.form_signon_realm, GURL(pair.origin)))
        << "form_signon_realm = " << pair.form_signon_realm
        << ", origin = " << pair.origin;
  }
}

TEST(PSLMatchingUtilsTest, IsFederatedPSLMatch) {
  struct TestPair {
    const char* form_signon_realm;
    const char* form_origin;
    const char* origin;
    bool should_match;
  };

  const TestPair pairs[] = {
      {"https://f8c3b00k.qjz9zk", "https://f8c3b00k.qjz9zk", "https://f8c3b00k.qjz9zk",
       false},
      {"", "", "", false},
      {"", "", "https://f8c3b00k.qjz9zk/", false},
      {"https://f8c3b00k.qjz9zk/", "https://f8c3b00k.qjz9zk/", "", false},

      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/p1",
       "https://example.com/p2", true},
      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/",
       "http://example.com/", false},
      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/",
       "example.com", false},
      {"federation://example.com/", "https://example.com/",
       "https://example.com/", false},
      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/", "example",
       false},

      {"federation://sub.example.com/9oo91e.qjz9zk", "https://sub.example.com/p1",
       "https://sub.example.com/p2", true},
      {"federation://sub.example.com/9oo91e.qjz9zk", "https://sub.example.com/p1",
       "https://sub2.example.com/p2", true},
      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/p1",
       "https://sub.example.com/", true},
      {"federation://example.com/9oo91e.qjz9zk", "https://example.com/",
       "http://sub.example.com/", false},
      {"federation://sub.example.com/", "https://sub.example.com/",
       "https://sub.example.com/", false},

      {"federation://localhost/9oo91e.qjz9zk", "http://localhost/",
       "http://localhost/", true},
      {"federation://localhost/9oo91e.qjz9zk", "http://localhost:8000/",
       "http://localhost:8000/", true},
      {"federation://localhost/9oo91e.qjz9zk", "http://localhost:8000/",
       "http://localhost/", false},
      {"federation://localhost/9oo91e.qjz9zk", "http://localhost/",
       "http://localhost:8000/", false},
  };

  for (const TestPair& pair : pairs) {
    EXPECT_EQ(pair.should_match,
              IsFederatedPSLMatch(pair.form_signon_realm,
                                  GURL(pair.form_origin), GURL(pair.origin)))
        << "form_signon_realm = " << pair.form_signon_realm
        << "form_origin = " << pair.form_origin << ", origin = " << pair.origin;
  }
}

TEST(PSLMatchingUtilsTest, GetOrganizationIdentifyingName) {
  static constexpr const struct {
    const char* url;
    const char* expected_organization_name;
  } kTestCases[] = {
      {"http://example.com/login", "example"},
      {"https://example.com", "example"},
      {"ftp://example.com/ftp_realm", "example"},

      {"http://foo.bar.example.com", "example"},
      {"http://example.co.uk", "example"},
      {"http://bar.example.8pp2p8t.qjz9zk", "example"},
      {"http://foo.bar", "foo"},
      {"https://user:pass@www.example.com:80/path?query#ref", "example"},

      {"http://www.foo+bar.com", "foo+bar"},
      {"http://www.foo-bar.com", "foo-bar"},
      {"https://foo_bar.com", "foo_bar"},
      {"http://www.foo%2Bbar.com", "foo+bar"},
      {"http://www.foo%2Dbar.com", "foo-bar"},
      {"https://foo%5Fbar.com", "foo_bar"},
      {"http://www.foo%2Ebar.com", "bar"},

      // Internationalized Domain Names: each dot-separated label of the domain
      // name is individually Punycode-encoded, so the organization-identifying
      // name is still well-defined and can be determined as normal.
      //       , ,
      //     szotar = sz\xc3\xb3t\xc3\xa1r (UTF-8) = xn--sztr-7na0i (IDN)
      //       | |
      //  U+00E1 U+00F3
      {"https://www.sz\xc3\xb3t\xc3\xa1r.8pp2p8t.qjz9zk", "xn--sztr-7na0i"},

      {"http://www.foo!bar.com", "foo%21bar"},
      {"http://www.foo%21bar.com", "foo%21bar"},
      {"http://www.foo$bar.com", "foo%24bar"},
      {"http://www.foo&bar.com", "foo%26bar"},
      {"http://www.foo\'bar.com", "foo%27bar"},
      {"http://www.foo(bar.com", "foo%28bar"},
      {"http://www.foo)bar.com", "foo%29bar"},
      {"http://www.foo*bar.com", "foo%2Abar"},
      {"http://www.foo,bar.com", "foo%2Cbar"},
      {"http://www.foo=bar.com", "foo%3Dbar"},

      // URLs without host portions, hosts without registry controlled domains
      // should, or hosts consisting of a registry yield the empty string.
      {"http://localhost", ""},
      {"http://co.uk", ""},
      {"http://google", ""},
      {"http://127.0.0.1", ""},
      {"file:///usr/bin/stuff", ""},
      {"federation://example.com/9oo91e.qjz9zk", ""},
      {"android://hash@com.example/", ""},
      {"http://[1080:0:0:0:8:800:200C:417A]/", ""},
      {"http://[3ffe:2a00:100:7031::1]", ""},
      {"http://[::192.9.5.5]/", ""},

      // Invalid URLs should yield the empty string.
      {"", ""},
      {"http://", ""},
      {"bad url", ""},
      {"http://www.example.com/%00", ""},
      {"http://www.foo;bar.com", ""},
      {"http://www.foo~bar.com", ""},
  };

  for (const auto& test_case : kTestCases) {
    SCOPED_TRACE(test_case.url);
    GURL url(test_case.url);
    EXPECT_EQ(test_case.expected_organization_name,
              GetOrganizationIdentifyingName(url));
  }
}

// Apart from alphanumeric characters and '.', only |kExpectedUnescapedChars|
// are expected to appear without percent-encoding in the domain of a valid,
// canonicalized URL.
//
// The purpose of this test is to ensure that the test cases around unescaped
// special characters in `GetOrganizationIdentifyingName` are exhaustive.
TEST(PSLMatchingUtilsTest,
     GetOrganizationIdentifyingName_UnescapedSpecialChars) {
  static constexpr const char kExpectedNonAlnumChars[] = {'+', '-', '_', '.'};
  for (int chr = 0; chr <= 255; ++chr) {
    const auto percent_encoded = base::StringPrintf("http://a%%%02Xb.hu/", chr);
    const GURL url(percent_encoded);
    if (isalnum(chr) || base::Contains(kExpectedNonAlnumChars, chr)) {
      ASSERT_TRUE(url.is_valid());
      const auto percent_decoded = base::StringPrintf(
          "http://a%cb.hu/", base::ToLowerASCII(static_cast<char>(chr)));
      EXPECT_EQ(percent_decoded, url.spec());
    } else if (url.is_valid()) {
      EXPECT_EQ(percent_encoded, url.spec());
    }
  }
}

}  // namespace

}  // namespace password_manager
