// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/common/url_pattern_set.h"

#include <stddef.h>

#include <sstream>

#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/values.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace extensions {

namespace {

void AddPattern(URLPatternSet* set, const std::string& pattern) {
  int schemes = URLPattern::SCHEME_ALL;
  set->AddPattern(URLPattern(schemes, pattern));
}

URLPatternSet Patterns(const std::string& pattern) {
  URLPatternSet set;
  AddPattern(&set, pattern);
  return set;
}

URLPatternSet Patterns(const std::string& pattern1,
                       const std::string& pattern2) {
  URLPatternSet set;
  AddPattern(&set, pattern1);
  AddPattern(&set, pattern2);
  return set;
}

}  // namespace

TEST(URLPatternSetTest, Empty) {
  URLPatternSet set;
  EXPECT_FALSE(set.MatchesURL(GURL("http://www.foo.com/bar")));
  EXPECT_FALSE(set.MatchesURL(GURL()));
  EXPECT_FALSE(set.MatchesURL(GURL("invalid")));
}

TEST(URLPatternSetTest, One) {
  URLPatternSet set;
  AddPattern(&set, "http://www.9oo91e.qjz9zk/*");

  EXPECT_TRUE(set.MatchesURL(GURL("http://www.9oo91e.qjz9zk/")));
  EXPECT_TRUE(set.MatchesURL(GURL("http://www.9oo91e.qjz9zk/monkey")));
  EXPECT_FALSE(set.MatchesURL(GURL("https://www.9oo91e.qjz9zk/")));
  EXPECT_FALSE(set.MatchesURL(GURL("https://www.m1cr050ft.qjz9zk/")));
}

TEST(URLPatternSetTest, Two) {
  URLPatternSet set;
  AddPattern(&set, "http://www.9oo91e.qjz9zk/*");
  AddPattern(&set, "http://www.yahoo.com/*");

  EXPECT_TRUE(set.MatchesURL(GURL("http://www.9oo91e.qjz9zk/monkey")));
  EXPECT_TRUE(set.MatchesURL(GURL("http://www.yahoo.com/monkey")));
  EXPECT_FALSE(set.MatchesURL(GURL("https://www.apple.com/monkey")));
}

TEST(URLPatternSetTest, StreamOperatorEmpty) {
  URLPatternSet set;

  std::ostringstream stream;
  stream << set;
  EXPECT_EQ("{ }", stream.str());
}

TEST(URLPatternSetTest, StreamOperatorOne) {
  URLPatternSet set;
  AddPattern(&set, "http://www.9oo91e.qjz9zk/*");

  std::ostringstream stream;
  stream << set;
  EXPECT_EQ("{ \"http://www.9oo91e.qjz9zk/*\" }", stream.str());
}

TEST(URLPatternSetTest, StreamOperatorTwo) {
  URLPatternSet set;
  AddPattern(&set, "http://www.9oo91e.qjz9zk/*");
  AddPattern(&set, "http://www.yahoo.com/*");

  std::ostringstream stream;
  stream << set;
  EXPECT_EQ("{ \"http://www.9oo91e.qjz9zk/*\", \"http://www.yahoo.com/*\" }",
            stream.str());
}

TEST(URLPatternSetTest, OverlapsWith) {
  URLPatternSet set1;
  AddPattern(&set1, "http://www.9oo91e.qjz9zk/f*");
  AddPattern(&set1, "http://www.yahoo.com/b*");

  URLPatternSet set2;
  AddPattern(&set2, "http://www.reddit.com/f*");
  AddPattern(&set2, "http://www.yahoo.com/z*");

  URLPatternSet set3;
  AddPattern(&set3, "http://www.9oo91e.qjz9zk/q/*");
  AddPattern(&set3, "http://www.yahoo.com/b/*");

  EXPECT_FALSE(set1.OverlapsWith(set2));
  EXPECT_FALSE(set2.OverlapsWith(set1));

  EXPECT_TRUE(set1.OverlapsWith(set3));
  EXPECT_TRUE(set3.OverlapsWith(set1));
}

TEST(URLPatternSetTest, CreateDifference) {
  URLPatternSet expected;
  URLPatternSet set1;
  URLPatternSet set2;
  AddPattern(&set1, "http://www.9oo91e.qjz9zk/f*");
  AddPattern(&set1, "http://www.yahoo.com/b*");

  // Subtract an empty set.
  URLPatternSet result = URLPatternSet::CreateDifference(set1, set2);
  EXPECT_EQ(set1, result);

  // Subtract a real set.
  AddPattern(&set2, "http://www.reddit.com/f*");
  AddPattern(&set2, "http://www.yahoo.com/z*");
  AddPattern(&set2, "http://www.9oo91e.qjz9zk/f*");

  AddPattern(&expected, "http://www.yahoo.com/b*");

  result = URLPatternSet::CreateDifference(set1, set2);
  EXPECT_EQ(expected, result);
  EXPECT_FALSE(result.is_empty());
  EXPECT_TRUE(set1.Contains(result));
  EXPECT_FALSE(result.Contains(set2));
  EXPECT_FALSE(set2.Contains(result));

  URLPatternSet intersection = URLPatternSet::CreateIntersection(
      result, set2, URLPatternSet::IntersectionBehavior::kStringComparison);
  EXPECT_TRUE(intersection.is_empty());
}

TEST(URLPatternSetTest, CreateIntersection_StringComparison) {
  URLPatternSet empty_set;
  URLPatternSet expected;
  URLPatternSet set1;
  AddPattern(&set1, "http://www.9oo91e.qjz9zk/f*");
  AddPattern(&set1, "http://www.yahoo.com/b*");

  // Intersection with an empty set.
  URLPatternSet result = URLPatternSet::CreateIntersection(
      set1, empty_set, URLPatternSet::IntersectionBehavior::kStringComparison);
  EXPECT_EQ(expected, result);
  EXPECT_TRUE(result.is_empty());
  EXPECT_TRUE(empty_set.Contains(result));
  EXPECT_TRUE(result.Contains(empty_set));
  EXPECT_TRUE(set1.Contains(result));

  // Intersection with a real set.
  URLPatternSet set2;
  AddPattern(&set2, "http://www.reddit.com/f*");
  AddPattern(&set2, "http://www.yahoo.com/z*");
  AddPattern(&set2, "http://www.9oo91e.qjz9zk/f*");

  AddPattern(&expected, "http://www.9oo91e.qjz9zk/f*");

  result = URLPatternSet::CreateIntersection(
      set1, set2, URLPatternSet::IntersectionBehavior::kStringComparison);
  EXPECT_EQ(expected, result);
  EXPECT_FALSE(result.is_empty());
  EXPECT_TRUE(set1.Contains(result));
  EXPECT_TRUE(set2.Contains(result));
}

TEST(URLPatternSetTest, CreateIntersection_PatternsContainedByBoth) {
  {
    URLPatternSet set1;
    AddPattern(&set1, "http://*.9oo91e.qjz9zk/*");
    AddPattern(&set1, "http://*.yahoo.com/*");

    URLPatternSet set2;
    AddPattern(&set2, "http://9oo91e.qjz9zk/*");

    // The semantic intersection should contain only those patterns that are in
    // both set 1 and set 2, or "http://9oo91e.qjz9zk/*".
    URLPatternSet intersection = URLPatternSet::CreateIntersection(
        set1, set2,
        URLPatternSet::IntersectionBehavior::kPatternsContainedByBoth);
    ASSERT_EQ(1u, intersection.size());
    EXPECT_EQ("http://9oo91e.qjz9zk/*", intersection.begin()->GetAsString());
  }

  {
    // IntersectionBehavior::kPatternsContainedByBoth doesn't handle funny
    // intersections, where the resultant pattern is neither of the two
    // compared patterns. So the intersection of these two is not
    // http://www.9oo91e.qjz9zk/*, but rather nothing.
    URLPatternSet set1;
    AddPattern(&set1, "http://*/*");
    URLPatternSet set2;
    AddPattern(&set2, "*://www.9oo91e.qjz9zk/*");
    EXPECT_TRUE(
        URLPatternSet::CreateIntersection(
            set1, set2,
            URLPatternSet::IntersectionBehavior::kPatternsContainedByBoth)
            .is_empty());
  }
}

TEST(URLPatternSetTest, CreateIntersection_Detailed) {
  struct {
    std::vector<std::string> set1;
    std::vector<std::string> set2;
    std::vector<std::string> expected_intersection;
  } test_cases[] = {
      {{"https://*.9oo91e.qjz9zk/*", "https://ch40m1um.qjz9zk/*"},
       {"*://maps.9oo91e.qjz9zk/*", "*://ch40m1um.qjz9zk/foo"},
       {"https://maps.9oo91e.qjz9zk/*", "https://ch40m1um.qjz9zk/foo"}},
      {{"https://*/*", "http://*/*"},
       {"*://9oo91e.qjz9zk/*", "*://ch40m1um.qjz9zk/*"},
       {"https://9oo91e.qjz9zk/*", "http://9oo91e.qjz9zk/*", "https://ch40m1um.qjz9zk/*",
        "http://ch40m1um.qjz9zk/*"}},
      {{"<all_urls>"},
       {"https://ch40m1um.qjz9zk/*", "*://9oo91e.qjz9zk/*"},
       {"https://ch40m1um.qjz9zk/*", "*://9oo91e.qjz9zk/*"}},
      {{"*://*/maps", "*://*.example.com/*"},
       {"https://*.9oo91e.qjz9zk/*", "https://www.example.com/*"},
       {"https://*.9oo91e.qjz9zk/maps", "https://www.example.com/*"}},
      {{"https://*/maps", "https://*.9oo91e.qjz9zk/*"},
       {"https://*.9oo91e.qjz9zk/*"},
       {"https://*.9oo91e.qjz9zk/*"}},
      {{"http://*/*"}, {"https://*/*"}, {}},
      {{"https://*.9oo91e.qjz9zk/*", "https://maps.9oo91e.qjz9zk/*"},
       {"https://*.9oo91e.qjz9zk/*", "https://*/*"},
       // NOTE: We don't currently do any additional "collapsing" step after
       // finding the intersection. We potentially could, to reduce the number
       // of patterns we need to store.
       {"https://*.9oo91e.qjz9zk/*", "https://maps.9oo91e.qjz9zk/*"}},
  };

  constexpr int kValidSchemes = URLPattern::SCHEME_ALL;
  constexpr char kTestCaseDescriptionTemplate[] =
      "Running Test Case:\n"
      "    Set1:            %s\n"
      "    Set2:            %s\n"
      "    Expected Result: %s";
  for (const auto& test_case : test_cases) {
    SCOPED_TRACE(base::StringPrintf(
        kTestCaseDescriptionTemplate,
        base::JoinString(test_case.set1, ", ").c_str(),
        base::JoinString(test_case.set2, ", ").c_str(),
        base::JoinString(test_case.expected_intersection, ", ").c_str()));

    URLPatternSet set1;
    for (const auto& pattern_str : test_case.set1) {
      URLPattern pattern(kValidSchemes);
      ASSERT_EQ(URLPattern::ParseResult::kSuccess, pattern.Parse(pattern_str))
          << "Failed to parse: " << pattern_str;
      set1.AddPattern(pattern);
    }

    URLPatternSet set2;
    for (const auto& pattern_str : test_case.set2) {
      URLPattern pattern(kValidSchemes);
      ASSERT_EQ(URLPattern::ParseResult::kSuccess, pattern.Parse(pattern_str))
          << "Failed to parse: " << pattern_str;
      set2.AddPattern(pattern);
    }

    URLPatternSet intersection1 = URLPatternSet::CreateIntersection(
        set1, set2, URLPatternSet::IntersectionBehavior::kDetailed);
    URLPatternSet intersection2 = URLPatternSet::CreateIntersection(
        set2, set1, URLPatternSet::IntersectionBehavior::kDetailed);

    EXPECT_THAT(
        *intersection1.ToStringVector(),
        testing::UnorderedElementsAreArray(test_case.expected_intersection));
    EXPECT_THAT(
        *intersection2.ToStringVector(),
        testing::UnorderedElementsAreArray(test_case.expected_intersection));
  }
}

TEST(URLPatternSetTest, CreateUnion) {
  URLPatternSet empty_set;

  URLPatternSet set1;
  AddPattern(&set1, "http://www.9oo91e.qjz9zk/f*");
  AddPattern(&set1, "http://www.yahoo.com/b*");

  URLPatternSet expected;
  AddPattern(&expected, "http://www.9oo91e.qjz9zk/f*");
  AddPattern(&expected, "http://www.yahoo.com/b*");

  // Union with an empty set.
  URLPatternSet result = URLPatternSet::CreateUnion(set1, empty_set);
  EXPECT_EQ(expected, result);

  // Union with a real set.
  URLPatternSet set2;
  AddPattern(&set2, "http://www.reddit.com/f*");
  AddPattern(&set2, "http://www.yahoo.com/z*");
  AddPattern(&set2, "http://www.9oo91e.qjz9zk/f*");

  AddPattern(&expected, "http://www.reddit.com/f*");
  AddPattern(&expected, "http://www.yahoo.com/z*");

  result = URLPatternSet::CreateUnion(set1, set2);
  EXPECT_EQ(expected, result);
}

TEST(URLPatternSetTest, Contains) {
  URLPatternSet set1;
  URLPatternSet set2;
  URLPatternSet empty_set;

  AddPattern(&set1, "http://www.9oo91e.qjz9zk/*");
  AddPattern(&set1, "http://www.yahoo.com/*");

  AddPattern(&set2, "http://www.reddit.com/*");

  EXPECT_FALSE(set1.Contains(set2));
  EXPECT_TRUE(set1.Contains(empty_set));
  EXPECT_FALSE(empty_set.Contains(set1));

  AddPattern(&set2, "http://www.yahoo.com/*");

  EXPECT_FALSE(set1.Contains(set2));
  EXPECT_FALSE(set2.Contains(set1));

  AddPattern(&set2, "http://www.9oo91e.qjz9zk/*");

  EXPECT_FALSE(set1.Contains(set2));
  EXPECT_TRUE(set2.Contains(set1));

  // Note that this checks if individual patterns contain other patterns, not
  // just equality. For example:
  AddPattern(&set1, "http://*.reddit.com/*");
  EXPECT_TRUE(set1.Contains(set2));
  EXPECT_FALSE(set2.Contains(set1));
}

TEST(URLPatternSetTest, Duplicates) {
  URLPatternSet set1;
  URLPatternSet set2;

  AddPattern(&set1, "http://www.9oo91e.qjz9zk/*");
  AddPattern(&set2, "http://www.9oo91e.qjz9zk/*");

  AddPattern(&set1, "http://www.9oo91e.qjz9zk/*");

  // The sets should still be equal after adding a duplicate.
  EXPECT_EQ(set2, set1);
}

TEST(URLPatternSetTest, ToValueAndPopulate) {
  URLPatternSet set1;
  URLPatternSet set2;

  std::vector<std::string> patterns;
  patterns.push_back("http://www.9oo91e.qjz9zk/*");
  patterns.push_back("http://www.yahoo.com/*");

  for (size_t i = 0; i < patterns.size(); ++i)
    AddPattern(&set1, patterns[i]);

  std::string error;
  bool allow_file_access = false;
  std::unique_ptr<base::ListValue> value(set1.ToValue());
  set2.Populate(*value, URLPattern::SCHEME_ALL, allow_file_access, &error);
  EXPECT_EQ(set1, set2);

  set2.ClearPatterns();
  set2.Populate(patterns, URLPattern::SCHEME_ALL, allow_file_access, &error);
  EXPECT_EQ(set1, set2);
}

TEST(URLPatternSetTest, NwayUnion) {
  std::string google_a = "http://www.9oo91e.qjz9zk/a*";
  std::string google_b = "http://www.9oo91e.qjz9zk/b*";
  std::string google_c = "http://www.9oo91e.qjz9zk/c*";
  std::string yahoo_a = "http://www.yahoo.com/a*";
  std::string yahoo_b = "http://www.yahoo.com/b*";
  std::string yahoo_c = "http://www.yahoo.com/c*";
  std::string reddit_a = "http://www.reddit.com/a*";
  std::string reddit_b = "http://www.reddit.com/b*";
  std::string reddit_c = "http://www.reddit.com/c*";

  // Empty list.
  {
    std::vector<URLPatternSet> empty;

    URLPatternSet result = URLPatternSet::CreateUnion(empty);

    URLPatternSet expected;
    EXPECT_EQ(expected, result);
  }

  // Singleton list.
  {
    std::vector<URLPatternSet> test;
    test.push_back(Patterns(google_a));

    URLPatternSet result = URLPatternSet::CreateUnion(test);

    URLPatternSet expected = Patterns(google_a);
    EXPECT_EQ(expected, result);
  }

  // List with 2 elements.
  {
    std::vector<URLPatternSet> test;
    test.push_back(Patterns(google_a, google_b));
    test.push_back(Patterns(google_b, google_c));

    URLPatternSet result = URLPatternSet::CreateUnion(test);

    URLPatternSet expected;
    AddPattern(&expected, google_a);
    AddPattern(&expected, google_b);
    AddPattern(&expected, google_c);
    EXPECT_EQ(expected, result);
  }

  // List with 3 elements.
  {
    std::vector<URLPatternSet> test;
    test.push_back(Patterns(google_a, google_b));
    test.push_back(Patterns(google_b, google_c));
    test.push_back(Patterns(yahoo_a, yahoo_b));

    URLPatternSet result = URLPatternSet::CreateUnion(test);

    URLPatternSet expected;
    AddPattern(&expected, google_a);
    AddPattern(&expected, google_b);
    AddPattern(&expected, google_c);
    AddPattern(&expected, yahoo_a);
    AddPattern(&expected, yahoo_b);
    EXPECT_EQ(expected, result);
  }

  // List with 7 elements.
  {
    std::vector<URLPatternSet> test;
    test.push_back(Patterns(google_a));
    test.push_back(Patterns(google_b));
    test.push_back(Patterns(google_c));
    test.push_back(Patterns(yahoo_a));
    test.push_back(Patterns(yahoo_b));
    test.push_back(Patterns(yahoo_c));
    test.push_back(Patterns(reddit_a));

    URLPatternSet result = URLPatternSet::CreateUnion(test);

    URLPatternSet expected;
    AddPattern(&expected, google_a);
    AddPattern(&expected, google_b);
    AddPattern(&expected, google_c);
    AddPattern(&expected, yahoo_a);
    AddPattern(&expected, yahoo_b);
    AddPattern(&expected, yahoo_c);
    AddPattern(&expected, reddit_a);
    EXPECT_EQ(expected, result);
  }

  // List with 8 elements.
  {
    std::vector<URLPatternSet> test;
    test.push_back(Patterns(google_a));
    test.push_back(Patterns(google_b));
    test.push_back(Patterns(google_c));
    test.push_back(Patterns(yahoo_a));
    test.push_back(Patterns(yahoo_b));
    test.push_back(Patterns(yahoo_c));
    test.push_back(Patterns(reddit_a));
    test.push_back(Patterns(reddit_b));

    URLPatternSet result = URLPatternSet::CreateUnion(test);

    URLPatternSet expected;
    AddPattern(&expected, google_a);
    AddPattern(&expected, google_b);
    AddPattern(&expected, google_c);
    AddPattern(&expected, yahoo_a);
    AddPattern(&expected, yahoo_b);
    AddPattern(&expected, yahoo_c);
    AddPattern(&expected, reddit_a);
    AddPattern(&expected, reddit_b);
    EXPECT_EQ(expected, result);
  }

  // List with 9 elements.
  {
    std::vector<URLPatternSet> test;
    test.push_back(Patterns(google_a));
    test.push_back(Patterns(google_b));
    test.push_back(Patterns(google_c));
    test.push_back(Patterns(yahoo_a));
    test.push_back(Patterns(yahoo_b));
    test.push_back(Patterns(yahoo_c));
    test.push_back(Patterns(reddit_a));
    test.push_back(Patterns(reddit_b));
    test.push_back(Patterns(reddit_c));

    URLPatternSet result = URLPatternSet::CreateUnion(test);

    URLPatternSet expected;
    AddPattern(&expected, google_a);
    AddPattern(&expected, google_b);
    AddPattern(&expected, google_c);
    AddPattern(&expected, yahoo_a);
    AddPattern(&expected, yahoo_b);
    AddPattern(&expected, yahoo_c);
    AddPattern(&expected, reddit_a);
    AddPattern(&expected, reddit_b);
    AddPattern(&expected, reddit_c);
    EXPECT_EQ(expected, result);
  }
}

TEST(URLPatternSetTest, AddOrigin) {
  URLPatternSet set;
  EXPECT_TRUE(set.AddOrigin(
      URLPattern::SCHEME_ALL, GURL("https://www.9oo91e.qjz9zk/")));
  EXPECT_TRUE(set.MatchesURL(GURL("https://www.9oo91e.qjz9zk/foo/bar")));
  EXPECT_FALSE(set.MatchesURL(GURL("http://www.9oo91e.qjz9zk/foo/bar")));
  EXPECT_FALSE(set.MatchesURL(GURL("https://en.9oo91e.qjz9zk/foo/bar")));
  set.ClearPatterns();

  EXPECT_TRUE(set.AddOrigin(
      URLPattern::SCHEME_ALL, GURL("https://9oo91e.qjz9zk/")));
  EXPECT_FALSE(set.MatchesURL(GURL("https://www.9oo91e.qjz9zk/foo/bar")));
  EXPECT_TRUE(set.MatchesURL(GURL("https://9oo91e.qjz9zk/foo/bar")));

  EXPECT_FALSE(set.AddOrigin(
      URLPattern::SCHEME_HTTP, GURL("https://9oo91e.qjz9zk/")));
}

TEST(URLPatternSet, AddOriginIPv6) {
  {
    URLPatternSet set;
    EXPECT_TRUE(set.AddOrigin(URLPattern::SCHEME_HTTP,
                              GURL("http://[2607:f8b0:4005:805::200e]/*")));
  }
  {
    URLPatternSet set;
    EXPECT_TRUE(set.AddOrigin(URLPattern::SCHEME_HTTP,
                              GURL("http://[2607:f8b0:4005:805::200e]/")));
  }
}

TEST(URLPatternSetTest, ToStringVector) {
  URLPatternSet set;
  AddPattern(&set, "https://9oo91e.qjz9zk/");
  AddPattern(&set, "https://9oo91e.qjz9zk/");
  AddPattern(&set, "https://yahoo.com/");

  std::unique_ptr<std::vector<std::string>> string_vector(set.ToStringVector());

  EXPECT_EQ(2UL, string_vector->size());

  EXPECT_TRUE(base::Contains(*string_vector, "https://9oo91e.qjz9zk/"));
  EXPECT_TRUE(base::Contains(*string_vector, "https://yahoo.com/"));
}

}  // namespace extensions
