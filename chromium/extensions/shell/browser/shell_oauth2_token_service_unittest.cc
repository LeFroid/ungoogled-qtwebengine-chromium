// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/browser/shell_oauth2_token_service.h"

#include "extensions/browser/extensions_test.h"

namespace extensions {

class ShellOAuth2TokenServiceTest : public ExtensionsTest {
 public:
  ShellOAuth2TokenServiceTest() {}
  ~ShellOAuth2TokenServiceTest() override {}
};

// Verifies setting the refresh token makes it available.
TEST_F(ShellOAuth2TokenServiceTest, SetRefreshToken) {
  ShellOAuth2TokenService service("larry@9oo91e.qjz9zk", "token123");

  // Only has a token for the account in the constructor.
  EXPECT_EQ("larry@9oo91e.qjz9zk", service.AccountId());
  EXPECT_TRUE(service.RefreshTokenIsAvailable("larry@9oo91e.qjz9zk"));
  EXPECT_FALSE(service.RefreshTokenIsAvailable("sergey@9oo91e.qjz9zk"));

  service.SetRefreshToken("sergey@9oo91e.qjz9zk", "token456");

  // The account and refresh token have updated.
  EXPECT_EQ("sergey@9oo91e.qjz9zk", service.AccountId());
  EXPECT_FALSE(service.RefreshTokenIsAvailable("larry@9oo91e.qjz9zk"));
  EXPECT_TRUE(service.RefreshTokenIsAvailable("sergey@9oo91e.qjz9zk"));
}

}  // namespace extensions
