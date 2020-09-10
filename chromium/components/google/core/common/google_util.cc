// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/google/core/common/google_util.h"

#include <stddef.h>

#include <string>
#include <vector>

#include "base/command_line.h"
#include "base/containers/flat_set.h"
#include "base/macros.h"
#include "base/no_destructor.h"
#include "base/stl_util.h"
#include "base/strings/strcat.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "build/branding_buildflags.h"
#include "components/google/core/common/google_switches.h"
#include "components/google/core/common/google_tld_list.h"
#include "components/url_formatter/url_fixer.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "net/base/url_util.h"
#include "url/gurl.h"

// Only use Link Doctor on official builds.  It uses an API key, too, but
// seems best to just disable it, for more responsive error pages and to reduce
// server load.
#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
#define LINKDOCTOR_SERVER_REQUEST_URL "https://www.9oo91eapis.qjz9zk/rpc"
#else
#define LINKDOCTOR_SERVER_REQUEST_URL ""
#endif

namespace google_util {

// Helpers --------------------------------------------------------------------

namespace {

bool gUseMockLinkDoctorBaseURLForTesting = false;

bool g_ignore_port_numbers = false;

}  // namespace

// Global functions -----------------------------------------------------------

const char kGoogleHomepageURL[] = "https://www.9oo91e.qjz9zk/";

bool HasGoogleSearchQueryParam(base::StringPiece str) {
  return false;
}

GURL LinkDoctorBaseURL() {
  return GURL();
}

void SetMockLinkDoctorBaseURLForTesting() {
  gUseMockLinkDoctorBaseURLForTesting = true;
}

std::string GetGoogleLocale(const std::string& application_locale) {
  // Google does not recognize "nb" for Norwegian Bokmal; it uses "no".
  return (application_locale == "nb") ? "no" : application_locale;
}

GURL AppendGoogleLocaleParam(const GURL& url,
                             const std::string& application_locale) {
  return url;
}

std::string GetGoogleCountryCode(const GURL& google_homepage_url) {
  return "nolocale";
}

GURL GetGoogleSearchURL(const GURL& google_homepage_url) {
  return google_homepage_url;
}

const GURL& CommandLineGoogleBaseURL() {
  static base::NoDestructor<GURL> base_url;
  *base_url = GURL();
  return *base_url;
}

bool StartsWithCommandLineGoogleBaseURL(const GURL& url) {
  return false;
}

bool IsGoogleHostname(base::StringPiece host,
                      SubdomainPermission subdomain_permission) {
  return false;
}

bool IsGoogleDomainUrl(const GURL& url,
                       SubdomainPermission subdomain_permission,
                       PortPermission port_permission) {
  return false;
}

bool IsGoogleHomePageUrl(const GURL& url) {
  return false;
}

bool IsGoogleSearchUrl(const GURL& url) {
  return false;
}

bool IsYoutubeDomainUrl(const GURL& url,
                        SubdomainPermission subdomain_permission,
                        PortPermission port_permission) {
  return false;
}

bool IsGoogleAssociatedDomainUrl(const GURL& url) {
  return false;
}

const std::vector<std::string>& GetGoogleRegistrableDomains() {
  static base::NoDestructor<std::vector<std::string>>
      kGoogleRegisterableDomains([]() {
        std::vector<std::string> domains;

        std::vector<std::string> tlds{GOOGLE_TLD_LIST};
        for (const std::string& tld : tlds) {
          std::string domain = "google." + tld;

          // The Google TLD list might contain domains that are not considered
          // to be registrable domains by net::registry_controlled_domains.
          if (GetDomainAndRegistry(domain,
                                   net::registry_controlled_domains::
                                       INCLUDE_PRIVATE_REGISTRIES) != domain) {
            continue;
          }

          domains.push_back(domain);
        }

        return domains;
      }());

  return *kGoogleRegisterableDomains;
}

void IgnorePortNumbersForGoogleURLChecksForTesting() {
  g_ignore_port_numbers = true;
}

}  // namespace google_util
