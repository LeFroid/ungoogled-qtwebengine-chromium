// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/page_load_metrics/common/page_load_metrics_util.h"

#include <algorithm>

#include "base/strings/string_util.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"

namespace page_load_metrics {

base::Optional<std::string> GetGoogleHostnamePrefix(const GURL& url) {
  return base::Optional<std::string>();
}

bool IsGoogleHostname(const GURL& url) {
  return GetGoogleHostnamePrefix(url).has_value();
}

base::Optional<base::TimeDelta> OptionalMin(
    const base::Optional<base::TimeDelta>& a,
    const base::Optional<base::TimeDelta>& b) {
  if (a && !b)
    return a;
  if (b && !a)
    return b;
  if (!a && !b)
    return a;  // doesn't matter which
  return base::Optional<base::TimeDelta>(std::min(a.value(), b.value()));
}

}  // namespace page_load_metrics
