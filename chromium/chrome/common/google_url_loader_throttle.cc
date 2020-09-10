// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/google_url_loader_throttle.h"

#include "build/build_config.h"
#include "chrome/common/net/safe_search_util.h"
#include "components/google/core/common/google_util.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

namespace {

#if defined(OS_ANDROID)
const char kClientDataHeader[] = "X-CCT-Client-Data";
#endif

}  // namespace

GoogleURLLoaderThrottle::GoogleURLLoaderThrottle(
#if defined(OS_ANDROID)
    const std::string& client_data_header,
#endif
    chrome::mojom::DynamicParams dynamic_params)
    :
#if defined(OS_ANDROID)
      client_data_header_(client_data_header),
#endif
      dynamic_params_(std::move(dynamic_params)) {
}

GoogleURLLoaderThrottle::~GoogleURLLoaderThrottle() = default;

void GoogleURLLoaderThrottle::DetachFromCurrentSequence() {}

void GoogleURLLoaderThrottle::WillStartRequest(
    network::ResourceRequest* request,
    bool* defer) {
}

void GoogleURLLoaderThrottle::WillRedirectRequest(
    net::RedirectInfo* redirect_info,
    const network::mojom::URLResponseHead& response_head,
    bool* /* defer */,
    std::vector<std::string>* to_be_removed_headers,
    net::HttpRequestHeaders* modified_headers) {
}

#if BUILDFLAG(ENABLE_EXTENSIONS)
void GoogleURLLoaderThrottle::WillProcessResponse(
    const GURL& response_url,
    network::mojom::URLResponseHead* response_head,
    bool* defer) {
}
#endif
