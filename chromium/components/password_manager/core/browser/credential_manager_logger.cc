// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/browser/credential_manager_logger.h"

#include <string>

#include "base/strings/string_number_conversions.h"
#include "components/autofill/core/browser/logging/log_manager.h"
#include "components/autofill/core/common/save_password_progress_logger.h"

using autofill::SavePasswordProgressLogger;

namespace password_manager {

CredentialManagerLogger::CredentialManagerLogger(
    const autofill::LogManager* log_manager)
    : log_manager_(log_manager) {}

CredentialManagerLogger::~CredentialManagerLogger() = default;

void CredentialManagerLogger::LogRequestCredential(
    const url::Origin& origin,
    CredentialMediationRequirement mediation,
    const std::vector<GURL>& federations) {
  std::string s("CM API get credentials: origin=" + origin.Serialize());
  s += ", mediation=";
  switch (mediation) {
    case CredentialMediationRequirement::kSilent:
      s += "silent";
      break;
    case CredentialMediationRequirement::kOptional:
      s += "optional";
      break;
    case CredentialMediationRequirement::kRequired:
      s += "required";
      break;
  }
  s += ", federations=";
  for (const GURL& federation_provider : federations)
    s += SavePasswordProgressLogger::ScrubURL(federation_provider) + ", ";

  log_manager_->LogTextMessage(s);
}

void CredentialManagerLogger::LogSendCredential(const url::Origin& origin,
                                                CredentialType type) {
  std::string s("CM API send a credential: origin=" + origin.Serialize());
  s += ", CredentialType=" + CredentialTypeToString(type);
  log_manager_->LogTextMessage(s);
}

void CredentialManagerLogger::LogStoreCredential(const url::Origin& origin,
                                                 CredentialType type) {
  std::string s("CM API save a credential: origin=" + origin.Serialize());
  s += ", CredentialType=" + CredentialTypeToString(type);
  log_manager_->LogTextMessage(s);
}

void CredentialManagerLogger::LogPreventSilentAccess(
    const url::Origin& origin) {
  std::string s("CM API sign out: origin=" + origin.Serialize());
  log_manager_->LogTextMessage(s);
}

}  // namespace password_manager
