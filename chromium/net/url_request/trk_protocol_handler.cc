// Copyright (c) 2018 The ungoogled-chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/url_request/trk_protocol_handler.h"

#include "base/logging.h"
#include "net/base/net_errors.h"
#include "net/url_request/url_request_error_job.h"

namespace net {

TrkProtocolHandler::TrkProtocolHandler() = default;

URLRequestJob* TrkProtocolHandler::MaybeCreateJob(
    URLRequest* request, NetworkDelegate* network_delegate) const {
  LOG(ERROR) << "Blocked URL in TrkProtocolHandler: " << request->original_url();
  return new URLRequestErrorJob(
      request, network_delegate, ERR_BLOCKED_BY_CLIENT);
}

bool TrkProtocolHandler::IsSafeRedirectTarget(const GURL& location) const {
  return true;
}

}  // namespace net
