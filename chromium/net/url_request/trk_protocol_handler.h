// Copyright (c) 2018 The ungoogled-chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_URL_REQUEST_TRK_PROTOCOL_HANDLER_H_
#define NET_URL_REQUEST_TRK_PROTOCOL_HANDLER_H_

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "net/base/net_export.h"
#include "net/url_request/url_request_job_factory.h"

namespace net {

class URLRequestJob;

// Implements a ProtocolHandler for Trk jobs.
class NET_EXPORT TrkProtocolHandler
    : public URLRequestJobFactory::ProtocolHandler {
 public:
  TrkProtocolHandler();
  URLRequestJob* MaybeCreateJob(
      URLRequest* request,
      NetworkDelegate* network_delegate) const override;
  bool IsSafeRedirectTarget(const GURL& location) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TrkProtocolHandler);
};

}  // namespace net

#endif  // NET_URL_REQUEST_TRK_PROTOCOL_HANDLER_H_
