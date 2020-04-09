// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/google_url_loader_throttle.h"

#include "chrome/common/net/safe_search_util.h"
#include "components/variations/net/variations_http_headers.h"
#include "services/network/public/cpp/resource_response.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

GoogleURLLoaderThrottle::GoogleURLLoaderThrottle(
    bool is_off_the_record,
    chrome::mojom::DynamicParams dynamic_params)
    : is_off_the_record_(is_off_the_record),
      dynamic_params_(std::move(dynamic_params)) {}

GoogleURLLoaderThrottle::~GoogleURLLoaderThrottle() {}

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
