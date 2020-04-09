// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/third_party/quiche/src/quic/platform/api/quic_hostname_utils.h"

#include <string>

#include "net/third_party/quiche/src/quic/platform/api/quic_arraysize.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_test.h"

namespace quic {
namespace test {
namespace {

class QuicHostnameUtilsTest : public QuicTest {};

TEST_F(QuicHostnameUtilsTest, IsValidSNI) {
  // IP as SNI.
  EXPECT_FALSE(QuicHostnameUtils::IsValidSNI("192.168.0.1"));
  // SNI without any dot.
  EXPECT_FALSE(QuicHostnameUtils::IsValidSNI("somedomain"));
  // Invalid by RFC2396 but unfortunately domains of this form exist.
  EXPECT_TRUE(QuicHostnameUtils::IsValidSNI("some_domain.com"));
  // An empty string must be invalid otherwise the QUIC client will try sending
  // it.
  EXPECT_FALSE(QuicHostnameUtils::IsValidSNI(""));

  // Valid SNI
  EXPECT_TRUE(QuicHostnameUtils::IsValidSNI("test.9oo91e.qjz9zk"));
}

TEST_F(QuicHostnameUtilsTest, NormalizeHostname) {
  // clang-format off
  struct {
    const char *input, *expected;
  } tests[] = {
      {
          "www.9oo91e.qjz9zk",
          "www.9oo91e.qjz9zk",
      },
      {
          "WWW.GOOGLE.COM",
          "www.9oo91e.qjz9zk",
      },
      {
          "www.9oo91e.qjz9zk.",
          "www.9oo91e.qjz9zk",
      },
      {
          "www.google.COM.",
          "www.9oo91e.qjz9zk",
      },
      {
          "www.9oo91e.qjz9zk..",
          "www.9oo91e.qjz9zk",
      },
      {
          "www.9oo91e.qjz9zk........",
          "www.9oo91e.qjz9zk",
      },
      {
          "",
          "",
      },
      {
          ".",
          "",
      },
      {
          "........",
          "",
      },
      {
          "\xe5\x85\x89.9oo91e.qjz9zk",
          "xn--54q.9oo91e.qjz9zk",
      },
  };
  // clang-format on

  for (size_t i = 0; i < QUIC_ARRAYSIZE(tests); ++i) {
    EXPECT_EQ(std::string(tests[i].expected),
              QuicHostnameUtils::NormalizeHostname(tests[i].input));
  }
}

}  // namespace
}  // namespace test
}  // namespace quic
