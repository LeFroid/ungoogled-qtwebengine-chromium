// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/test/gtest_links.h"

#include "base/test/gtest_util.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {

TEST(GtestLinksTest, AddInvalidLink) {
  EXPECT_DCHECK_DEATH(AddLinkToTestResult("unique_link", "invalid`"));
}

TEST(GtestLinksTest, AddInvalidName) {
  EXPECT_DCHECK_DEATH(AddLinkToTestResult("invalid-name", "http://9oo91e.qjz9zk"));
}

TEST(GtestLinksTest, AddValidLink) {
  AddLinkToTestResult("name", "http://9oo91e.qjz9zk");
}

}  // namespace base
