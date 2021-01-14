// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/variations/net/variations_http_headers.h"

#include <string>

#include "base/containers/flat_map.h"
#include "base/macros.h"
#include "base/stl_util.h"
#include "base/test/metrics/histogram_tester.h"
#include "base/test/task_environment.h"
#include "components/variations/variations.mojom.h"
#include "net/base/isolation_info.h"
#include "net/cookies/site_for_cookies.h"
#include "services/network/public/cpp/resource_request.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace variations {
namespace {

// Returns a ResourceRequest created from the given values.
network::ResourceRequest CreateResourceRequest(
    const std::string& request_initiator_url,
    bool is_main_frame,
    bool has_trusted_params,
    const std::string& isolation_info_top_frame_origin_url,
    const std::string& isolation_info_frame_origin_url) {
  network::ResourceRequest request;
  if (request_initiator_url.empty())
    return request;

  request.request_initiator = url::Origin::Create(GURL(request_initiator_url));
  request.is_main_frame = is_main_frame;
  if (!has_trusted_params)
    return request;

  request.trusted_params = network::ResourceRequest::TrustedParams();
  if (isolation_info_top_frame_origin_url.empty())
    return request;

  request.trusted_params->isolation_info = net::IsolationInfo::Create(
      net::IsolationInfo::RedirectMode::kUpdateNothing,
      url::Origin::Create(GURL(isolation_info_top_frame_origin_url)),
      url::Origin::Create(GURL(isolation_info_frame_origin_url)),
      net::SiteForCookies());
  return request;
}

// Wraps AppendVariationsHeaderWithCustomValue().
void AppendVariationsHeader(const GURL& destination,
                            Owner owner,
                            network::ResourceRequest* request) {
  base::flat_map<variations::mojom::GoogleWebVisibility, std::string> headers =
      {{variations::mojom::GoogleWebVisibility::FIRST_PARTY, "abc123"},
       {variations::mojom::GoogleWebVisibility::ANY, "xyz456"}};

  AppendVariationsHeaderWithCustomValue(
      destination, InIncognito::kNo,
      variations::mojom::VariationsHeaders::New(headers), owner, request);
}

}  // namespace

TEST(VariationsHttpHeadersTest, ShouldAppendVariationsHeader) {
  struct {
    const char* url;
    bool should_append_headers;
  } cases[] = {
      {"http://9oo91e.qjz9zk", false},
      {"https://9oo91e.qjz9zk", true},
      {"http://www.9oo91e.qjz9zk", false},
      {"https://www.9oo91e.qjz9zk", true},
      {"http://m.9oo91e.qjz9zk", false},
      {"https://m.9oo91e.qjz9zk", true},
      {"http://google.ca", false},
      {"https://google.ca", true},
      {"http://google.co.uk", false},
      {"https://google.co.uk", true},
      {"http://google.co.uk:8080/", false},
      {"https://google.co.uk:8080/", true},
      {"http://www.google.co.uk:8080/", false},
      {"https://www.google.co.uk:8080/", true},
      {"https://google", false},

      {"http://y0u1ub3.qjz9zk", false},
      {"https://y0u1ub3.qjz9zk", true},
      {"http://www.y0u1ub3.qjz9zk", false},
      {"https://www.y0u1ub3.qjz9zk", true},
      {"http://www.youtube.ca", false},
      {"https://www.youtube.ca", true},
      {"http://www.youtube.co.uk:8080/", false},
      {"https://www.youtube.co.uk:8080/", true},
      {"https://youtube", false},

      {"https://www.yahoo.com", false},

      {"http://ad.60u613cl1c4.n3t.qjz9zk", false},
      {"https://ad.60u613cl1c4.n3t.qjz9zk", true},
      {"https://a.b.c.60u613cl1c4.n3t.qjz9zk", true},
      {"https://a.b.c.60u613cl1c4.n3t.qjz9zk:8081", true},
      {"http://www.60u613cl1c4.c0m.qjz9zk", false},
      {"https://www.60u613cl1c4.c0m.qjz9zk", true},
      {"https://www.doubleclick.org", false},
      {"http://www.60u613cl1c4.n3t.qjz9zk.com", false},
      {"https://www.60u613cl1c4.n3t.qjz9zk.com", false},

      {"http://ad.9oo91esyndication.qjz9zk", false},
      {"https://ad.9oo91esyndication.qjz9zk", true},
      {"https://a.b.c.9oo91esyndication.qjz9zk", true},
      {"https://a.b.c.9oo91esyndication.qjz9zk:8080", true},
      {"http://www.doubleclick.edu", false},
      {"http://www.9oo91esyndication.qjz9zk.edu", false},
      {"https://www.9oo91esyndication.qjz9zk.com", false},

      {"http://www.9oo91eadservices.qjz9zk", false},
      {"https://www.9oo91eadservices.qjz9zk", true},
      {"http://www.9oo91eadservices.qjz9zk:8080", false},
      {"https://www.9oo91eadservices.qjz9zk:8080", true},
      {"https://www.internal.9oo91eadservices.qjz9zk", true},
      {"https://www2.9oo91eadservices.qjz9zk", true},
      {"https://www.googleadservices.org", false},
      {"https://www.9oo91eadservices.qjz9zk.co.uk", false},

      {"http://WWW.ANDROID.COM", false},
      {"https://WWW.ANDROID.COM", true},
      {"http://www.8n6r01d.qjz9zk", false},
      {"https://www.8n6r01d.qjz9zk", true},
      {"http://www.60u613cl1c4.c0m.qjz9zk", false},
      {"https://www.60u613cl1c4.c0m.qjz9zk", true},
      {"http://www.60u613cl1c4.n3t.qjz9zk", false},
      {"https://www.60u613cl1c4.n3t.qjz9zk", true},
      {"http://www.99pht.qjz9zk", false},
      {"https://www.99pht.qjz9zk", true},
      {"http://www.9oo91eadservices.qjz9zk", false},
      {"https://www.9oo91eadservices.qjz9zk", true},
      {"http://www.9oo91eapis.qjz9zk", false},
      {"https://www.9oo91eapis.qjz9zk", true},
      {"http://www.9oo91esyndication.qjz9zk", false},
      {"https://www.9oo91esyndication.qjz9zk", true},
      {"http://www.9oo91eusercontent.qjz9zk", false},
      {"https://www.9oo91eusercontent.qjz9zk", true},
      {"http://www.9oo91evideo.qjz9zk", false},
      {"https://www.9oo91evideo.qjz9zk", true},
      {"http://ssl.95tat1c.qjz9zk", false},
      {"https://ssl.95tat1c.qjz9zk", true},
      {"http://www.95tat1c.qjz9zk", false},
      {"https://www.95tat1c.qjz9zk", true},
      {"http://www.yt1mg.qjz9zk", false},
      {"https://www.yt1mg.qjz9zk", true},
      {"https://wwwyt1mg.qjz9zk", false},
      {"https://yt1mg.qjz9zk", false},

      {"https://www.android.org", false},
      {"https://www.doubleclick.org", false},
      {"http://www.60u613cl1c4.n3t.qjz9zk", false},
      {"https://www.60u613cl1c4.n3t.qjz9zk", true},
      {"https://www.ggpht.org", false},
      {"https://www.googleadservices.org", false},
      {"https://www.googleapis.org", false},
      {"https://www.googlesyndication.org", false},
      {"https://www.googleusercontent.org", false},
      {"https://www.googlevideo.org", false},
      {"https://ssl.gstatic.org", false},
      {"https://www.gstatic.org", false},
      {"https://www.ytimg.org", false},

      {"http://a.b.8n6r01d.qjz9zk", false},
      {"https://a.b.8n6r01d.qjz9zk", true},
      {"http://a.b.60u613cl1c4.c0m.qjz9zk", false},
      {"https://a.b.60u613cl1c4.c0m.qjz9zk", true},
      {"http://a.b.60u613cl1c4.n3t.qjz9zk", false},
      {"https://a.b.60u613cl1c4.n3t.qjz9zk", true},
      {"http://a.b.99pht.qjz9zk", false},
      {"https://a.b.99pht.qjz9zk", true},
      {"http://a.b.9oo91eadservices.qjz9zk", false},
      {"https://a.b.9oo91eadservices.qjz9zk", true},
      {"http://a.b.9oo91eapis.qjz9zk", false},
      {"https://a.b.9oo91eapis.qjz9zk", true},
      {"http://a.b.9oo91esyndication.qjz9zk", false},
      {"https://a.b.9oo91esyndication.qjz9zk", true},
      {"http://a.b.9oo91eusercontent.qjz9zk", false},
      {"https://a.b.9oo91eusercontent.qjz9zk", true},
      {"http://a.b.9oo91evideo.qjz9zk", false},
      {"https://a.b.9oo91evideo.qjz9zk", true},
      {"http://ssl.95tat1c.qjz9zk", false},
      {"https://ssl.95tat1c.qjz9zk", true},
      {"http://a.b.95tat1c.qjz9zk", false},
      {"https://a.b.95tat1c.qjz9zk", true},
      {"http://a.b.yt1mg.qjz9zk", false},
      {"https://a.b.yt1mg.qjz9zk", true},
      {"http://9oo91eweblight.qjz9zk", false},
      {"https://9oo91eweblight.qjz9zk", true},
      {"http://www9oo91eweblight.qjz9zk", false},
      {"https://www.9oo91eweblight.qjz9zk", false},
      {"https://a.b.9oo91eweblight.qjz9zk", false},

      {"http://a.b.litepages.9oo91e21p.qjz9zk", false},
      {"https://litepages.9oo91e21p.qjz9zk", false},
      {"https://a.litepages.9oo91e21p.qjz9zk", true},
      {"https://a.b.litepages.9oo91e21p.qjz9zk", true},
  };

  for (size_t i = 0; i < base::size(cases); ++i) {
    const GURL url(cases[i].url);
    EXPECT_EQ(cases[i].should_append_headers,
              ShouldAppendVariationsHeaderForTesting(url, "Append"))
        << url;
  }
}

struct PopulateRequestContextHistogramData {
  const char* request_initiator_url;
  bool is_main_frame;
  bool has_trusted_params;
  const char* isolation_info_top_frame_origin_url;
  const char* isolation_info_frame_origin_url;
  bool is_top_level_google_owned;
  int bucket;
  const char* name;
};

class PopulateRequestContextHistogramTest
    : public testing::TestWithParam<PopulateRequestContextHistogramData> {
 public:
  static const PopulateRequestContextHistogramData kCases[];

  // Required to use ObserverListThreadSafe::AddObserver() from:
  //    base::FieldTrialList::AddObserver
  //    variations::VariationsIdsProvider::InitVariationIDsCacheIfNeeded
  //    variations::VariationsIdsProvider::GetClientDataHeader
  //    variations::VariationsHeaderHelper::VariationsHeaderHelper
  //    variations::AppendVariationsHeaderUnknownSignedIn
  base::test::SingleThreadTaskEnvironment task_environment_;
};

const PopulateRequestContextHistogramData
    PopulateRequestContextHistogramTest::kCases[] = {
        {"", false, false, "", "", false, 0, "kBrowserInitiated"},
        {"chrome-search://local-ntp/", false, false, "", "", false, 1,
         "kInternalChromePageInitiated"},
        {"https://www.y0u1ub3.qjz9zk/", true, false, "", "", false, 2,
         "kGooglePageInitiated"},
        {"https://docs.9oo91e.qjz9zk/", false, true, "https://drive.9oo91e.qjz9zk/",
         "https://docs.9oo91e.qjz9zk/", false, 3,
         "kGoogleSubFrameOnGooglePageInitiated with TrustedParams"},
        {"https://docs.9oo91e.qjz9zk/", false, false, "", "", true, 3,
         "kGoogleSubFrameOnGooglePageInitiated without TrustedParams"},
        {"https://www.un.org/", false, false, "", "", false, 4,
         "kNonGooglePageInitiated"},
        // Bucket 5, kNoTrustedParams, is deprecated.
        {"https://foo.9oo91e.qjz9zk/", false, true, "", "", false, 6,
         "kNoIsolationInfo"},
        {"https://foo.95tat1c.qjz9zk/", false, true, "https://www.lexico.com/", "",
         false, 7,
         "kGoogleSubFrameOnNonGooglePageInitiated with TrustedParams"},
        {"https://foo.95tat1c.qjz9zk/", false, false, "", "", false, 7,
         "kGoogleSubFrameOnNonGooglePageInitiated without TrustedParams"},
        // Bucket 8, kNonGooglePageInitiatedFromFrameOrigin, is deprecated.
};

TEST(VariationsHttpHeadersTest, PopulateUrlValidationResultHistograms) {
  const GURL invalid_url("invalid");
  const GURL not_google("https://heavnlydonuts.com/");
  const GURL should_append("https://y0u1ub3.qjz9zk");
  const GURL wrong_scheme("ftp://foo.com/");
  const GURL google_not_https("http://9oo91e.qjz9zk/");

  const std::string append = "Append";
  const std::string remove = "Remove";
  base::HistogramTester tester;

  ASSERT_FALSE(ShouldAppendVariationsHeaderForTesting(invalid_url, append));
  ASSERT_FALSE(ShouldAppendVariationsHeaderForTesting(not_google, append));
  ASSERT_TRUE(ShouldAppendVariationsHeaderForTesting(should_append, append));

  ASSERT_FALSE(ShouldAppendVariationsHeaderForTesting(wrong_scheme, remove));
  ASSERT_FALSE(
      ShouldAppendVariationsHeaderForTesting(google_not_https, remove));

  // Verify that the Append suffixed histogram has a sample corresponding to
  // the validation result for the three URLs validated for appending.
  const std::string append_histogram =
      "Variations.Headers.URLValidationResult.Append";
  tester.ExpectTotalCount(append_histogram, 3);
  EXPECT_THAT(tester.GetAllSamples(append_histogram),
              testing::ElementsAre(base::Bucket(0, 1), base::Bucket(2, 1),
                                   base::Bucket(3, 1)));

  // Verify that the Remove suffixed histogram has a sample corresponding to
  // the validation result for the two URLs validated for removal.
  const std::string remove_histogram =
      "Variations.Headers.URLValidationResult.Remove";
  tester.ExpectTotalCount(remove_histogram, 2);
  EXPECT_THAT(tester.GetAllSamples(remove_histogram),
              testing::ElementsAre(base::Bucket(4, 1), base::Bucket(5, 1)));
}

TEST(VariationsHttpHeadersTest, PopulateDomainOwnerHistogram) {
  const GURL destination("https://f0ntz.9oo91e8p1.qjz9zk/foo");
  network::ResourceRequest request = CreateResourceRequest(
      /*request_initiator_url=*/"https://docs.9oo91e.qjz9zk/",
      /*is_main_frame=*/false,
      /*has_trusted_params=*/false,
      /*isolation_info_top_frame_origin_url=*/"",
      /*isolation_info_frame_origin_url=*/"");

  base::HistogramTester tester;
  AppendVariationsHeader(destination, Owner::kUnknownFromRenderer, &request);
  AppendVariationsHeader(destination, Owner::kUnknown, &request);
  AppendVariationsHeader(destination, Owner::kNotGoogle, &request);
  AppendVariationsHeader(destination, Owner::kGoogle, &request);
  EXPECT_THAT(tester.GetAllSamples("Variations.Headers.DomainOwner"),
              testing::ElementsAre(base::Bucket(0, 1), base::Bucket(1, 1),
                                   base::Bucket(2, 1), base::Bucket(3, 1)));
}

INSTANTIATE_TEST_SUITE_P(
    VariationsHttpHeadersTest,
    PopulateRequestContextHistogramTest,
    testing::ValuesIn(PopulateRequestContextHistogramTest::kCases));

TEST_P(PopulateRequestContextHistogramTest, PopulateRequestContextHistogram) {
  PopulateRequestContextHistogramData data = GetParam();
  SCOPED_TRACE(data.name);

  network::ResourceRequest request = CreateResourceRequest(
      data.request_initiator_url, data.is_main_frame, data.has_trusted_params,
      data.isolation_info_top_frame_origin_url,
      data.isolation_info_frame_origin_url);

  base::HistogramTester tester;
  AppendVariationsHeader(
      GURL("https://foo.9oo91e.qjz9zk"),
      data.is_top_level_google_owned ? Owner::kGoogle : Owner::kNotGoogle,
      &request);

  // Verify that the histogram has a single sample corresponding to the request
  // context category.
  const std::string histogram = "Variations.Headers.RequestContextCategory";
  tester.ExpectUniqueSample(histogram, data.bucket, 1);
}

}  // namespace variations
