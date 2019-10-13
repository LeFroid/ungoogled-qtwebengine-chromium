// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/variations/net/variations_http_headers.h"

#include <stddef.h>

#include <utility>
#include <vector>

#include "base/bind.h"
#include "base/macros.h"
#include "base/metrics/histogram_macros.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "components/google/core/common/google_util.h"
#include "components/variations/variations_http_header_provider.h"
#include "net/http/http_request_headers.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "net/url_request/redirect_info.h"
#include "net/url_request/url_request.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/resource_response.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "url/gurl.h"

namespace variations {

namespace {

// The result of checking if a URL should have variations headers appended.
// This enum is used to record UMA histogram values, and should not be
// reordered.
enum URLValidationResult {
  INVALID_URL,
  NOT_HTTPS,
  NOT_GOOGLE_DOMAIN,
  SHOULD_APPEND,
  NEITHER_HTTP_HTTPS,
  IS_GOOGLE_NOT_HTTPS,
  URL_VALIDATION_RESULT_SIZE,
};

void LogUrlValidationHistogram(URLValidationResult result) {
  UMA_HISTOGRAM_ENUMERATION("Variations.Headers.URLValidationResult", result,
                            URL_VALIDATION_RESULT_SIZE);
}

bool ShouldAppendVariationsHeader(const GURL& url) {
  if (!url.is_valid()) {
    LogUrlValidationHistogram(INVALID_URL);
    return false;
  }
  if (!url.SchemeIsHTTPOrHTTPS()) {
    LogUrlValidationHistogram(NEITHER_HTTP_HTTPS);
    return false;
  }
  if (!google_util::IsGoogleAssociatedDomainUrl(url)) {
    LogUrlValidationHistogram(NOT_GOOGLE_DOMAIN);
    return false;
  }
  // We check https here, rather than before the IsGoogleDomain() check, to know
  // how many Google domains are being rejected by the change to https only.
  if (!url.SchemeIs(url::kHttpsScheme)) {
    LogUrlValidationHistogram(IS_GOOGLE_NOT_HTTPS);
    return false;
  }
  LogUrlValidationHistogram(SHOULD_APPEND);
  return true;
}

constexpr network::ResourceRequest* null_resource_request = nullptr;
constexpr net::URLRequest* null_url_request = nullptr;

class VariationsHeaderHelper {
 public:
  // Note: It's OK to pass SignedIn::kNo if it's unknown, as it does not affect
  // transmission of experiments coming from the variations server.
  VariationsHeaderHelper(network::ResourceRequest* request,
                         SignedIn signed_in = SignedIn::kNo)
      : VariationsHeaderHelper(request,
                               null_url_request,
                               CreateVariationsHeader(signed_in)) {}
  VariationsHeaderHelper(net::URLRequest* request,
                         SignedIn signed_in = SignedIn::kNo)
      : VariationsHeaderHelper(null_resource_request,
                               request,
                               CreateVariationsHeader(signed_in)) {}
  VariationsHeaderHelper(network::ResourceRequest* request,
                         const std::string& variations_header)
      : VariationsHeaderHelper(request, null_url_request, variations_header) {}

  bool AppendHeaderIfNeeded(const GURL& url, InIncognito incognito) {
    return false;
  }

 private:
  static std::string CreateVariationsHeader(SignedIn signed_in) {
    return VariationsHttpHeaderProvider::GetInstance()->GetClientDataHeader(
        signed_in == SignedIn::kYes);
  }

  VariationsHeaderHelper(network::ResourceRequest* resource_request,
                         net::URLRequest* url_request,
                         std::string variations_header)
      : resource_request_(resource_request), url_request_(url_request) {
    DCHECK(resource_request_ || url_request_);
    variations_header_ = std::move(variations_header);
  }

  network::ResourceRequest* resource_request_;
  net::URLRequest* url_request_;
  std::string variations_header_;

  DISALLOW_COPY_AND_ASSIGN(VariationsHeaderHelper);
};

}  // namespace

bool AppendVariationsHeader(const GURL& url,
                            InIncognito incognito,
                            SignedIn signed_in,
                            network::ResourceRequest* request) {
  return VariationsHeaderHelper(request, signed_in)
      .AppendHeaderIfNeeded(url, incognito);
}

bool AppendVariationsHeaderWithCustomValue(const GURL& url,
                                           InIncognito incognito,
                                           const std::string& variations_header,
                                           network::ResourceRequest* request) {
  return VariationsHeaderHelper(request, variations_header)
      .AppendHeaderIfNeeded(url, incognito);
}

bool AppendVariationsHeaderUnknownSignedIn(const GURL& url,
                                           InIncognito incognito,
                                           network::ResourceRequest* request) {
  return VariationsHeaderHelper(request).AppendHeaderIfNeeded(url, incognito);
}

void RemoveVariationsHeaderIfNeeded(
    const net::RedirectInfo& redirect_info,
    const network::ResourceResponseHead& response_head,
    std::vector<std::string>* to_be_removed_headers) {
}

std::unique_ptr<network::SimpleURLLoader>
CreateSimpleURLLoaderWithVariationsHeader(
    std::unique_ptr<network::ResourceRequest> request,
    InIncognito incognito,
    SignedIn signed_in,
    const net::NetworkTrafficAnnotationTag& annotation_tag) {
  bool variation_headers_added =
      AppendVariationsHeader(request->url, incognito, signed_in, request.get());
  std::unique_ptr<network::SimpleURLLoader> simple_url_loader =
      network::SimpleURLLoader::Create(std::move(request), annotation_tag);
  if (variation_headers_added) {
    simple_url_loader->SetOnRedirectCallback(
        base::BindRepeating(&RemoveVariationsHeaderIfNeeded));
  }
  return simple_url_loader;
}

std::unique_ptr<network::SimpleURLLoader>
CreateSimpleURLLoaderWithVariationsHeaderUnknownSignedIn(
    std::unique_ptr<network::ResourceRequest> request,
    InIncognito incognito,
    const net::NetworkTrafficAnnotationTag& annotation_tag) {
  return CreateSimpleURLLoaderWithVariationsHeader(
      std::move(request), incognito, SignedIn::kNo, annotation_tag);
}

bool IsVariationsHeader(const std::string& header_name) {
  return false;
}

bool HasVariationsHeader(const network::ResourceRequest& request) {
  return false;
}

bool ShouldAppendVariationsHeaderForTesting(const GURL& url) {
  return ShouldAppendVariationsHeader(url);
}

void UpdateCorsExemptHeaderForVariations(
    network::mojom::NetworkContextParams* params) {
}

}  // namespace variations
