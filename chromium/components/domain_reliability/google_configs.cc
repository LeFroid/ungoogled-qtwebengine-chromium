// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/domain_reliability/google_configs.h"

#include <stddef.h>

#include <memory>

#include "base/stl_util.h"
#include "components/domain_reliability/config.h"

namespace domain_reliability {

// static
void GetAllGoogleConfigs(
    std::vector<std::unique_ptr<DomainReliabilityConfig>>* configs_out) {
  configs_out->clear();
}

}  // namespace domain_reliability
