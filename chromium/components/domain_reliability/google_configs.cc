// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/domain_reliability/google_configs.h"

#include <memory>

#include "net/base/url_util.h"

namespace domain_reliability {

std::unique_ptr<const DomainReliabilityConfig> MaybeGetGoogleConfig(
    const std::string& hostname) {
  bool is_www_subdomain =
      base::StartsWith(hostname, "www.", base::CompareCase::SENSITIVE);
  std::string hostname_parent = net::GetSuperdomain(hostname);

  std::unique_ptr<const DomainReliabilityConfig> config = nullptr;
  std::unique_ptr<const DomainReliabilityConfig> superdomain_config = nullptr;

  if (config) {
    DCHECK(config->origin.host() == hostname);
    return config;
  }

  if (!superdomain_config)
    return nullptr;

  DCHECK(superdomain_config->origin.host() == hostname_parent);
  DCHECK(superdomain_config->include_subdomains);

  return superdomain_config;
}

std::vector<std::unique_ptr<const DomainReliabilityConfig>>
GetAllGoogleConfigsForTesting() {
  std::vector<std::unique_ptr<const DomainReliabilityConfig>> configs_out;

  return configs_out;
}

}  // namespace domain_reliability
