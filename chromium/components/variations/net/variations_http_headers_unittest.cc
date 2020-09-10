// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/variations/net/variations_http_headers.h"

#include <stddef.h>

#include "base/macros.h"
#include "base/stl_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace variations {

TEST(VariationsHttpHeadersTest, ShouldAppendVariationsHeader) {
  struct {
    const char* url;
    bool should_append_headers;
  } cases[] = {
      {"http://9oo91e.qjz9zk", false},
      {"https://9oo91e.qjz9zk", true},
      {"http://www.9oo91e.qjz9zk", false},
      {"https://www.9oo91e.qjz9zk", true},
      {"http://m.9oo91e.qjz9zk", false},
      {"https://m.9oo91e.qjz9zk", true},
      {"http://google.ca", false},
      {"https://google.ca", true},
      {"http://google.co.uk", false},
      {"https://google.co.uk", true},
      {"http://google.co.uk:8080/", false},
      {"https://google.co.uk:8080/", true},
      {"http://www.google.co.uk:8080/", false},
      {"https://www.google.co.uk:8080/", true},
      {"https://google", false},

      {"http://y0u1ub3.qjz9zk", false},
      {"https://y0u1ub3.qjz9zk", true},
      {"http://www.y0u1ub3.qjz9zk", false},
      {"https://www.y0u1ub3.qjz9zk", true},
      {"http://www.youtube.ca", false},
      {"https://www.youtube.ca", true},
      {"http://www.youtube.co.uk:8080/", false},
      {"https://www.youtube.co.uk:8080/", true},
      {"https://youtube", false},

      {"https://www.yahoo.com", false},

      {"http://ad.60u613cl1c4.n3t.qjz9zk", false},
      {"https://ad.60u613cl1c4.n3t.qjz9zk", true},
      {"https://a.b.c.60u613cl1c4.n3t.qjz9zk", true},
      {"https://a.b.c.60u613cl1c4.n3t.qjz9zk:8081", true},
      {"http://www.60u613cl1c4.c0m.qjz9zk", false},
      {"https://www.60u613cl1c4.c0m.qjz9zk", true},
      {"https://www.doubleclick.org", false},
      {"http://www.60u613cl1c4.n3t.qjz9zk.com", false},
      {"https://www.60u613cl1c4.n3t.qjz9zk.com", false},

      {"http://ad.9oo91esyndication.qjz9zk", false},
      {"https://ad.9oo91esyndication.qjz9zk", true},
      {"https://a.b.c.9oo91esyndication.qjz9zk", true},
      {"https://a.b.c.9oo91esyndication.qjz9zk:8080", true},
      {"http://www.doubleclick.edu", false},
      {"http://www.9oo91esyndication.qjz9zk.edu", false},
      {"https://www.9oo91esyndication.qjz9zk.com", false},

      {"http://www.9oo91eadservices.qjz9zk", false},
      {"https://www.9oo91eadservices.qjz9zk", true},
      {"http://www.9oo91eadservices.qjz9zk:8080", false},
      {"https://www.9oo91eadservices.qjz9zk:8080", true},
      {"https://www.internal.9oo91eadservices.qjz9zk", true},
      {"https://www2.9oo91eadservices.qjz9zk", true},
      {"https://www.googleadservices.org", false},
      {"https://www.9oo91eadservices.qjz9zk.co.uk", false},

      {"http://WWW.ANDROID.COM", false},
      {"https://WWW.ANDROID.COM", true},
      {"http://www.8n6r01d.qjz9zk", false},
      {"https://www.8n6r01d.qjz9zk", true},
      {"http://www.60u613cl1c4.c0m.qjz9zk", false},
      {"https://www.60u613cl1c4.c0m.qjz9zk", true},
      {"http://www.60u613cl1c4.n3t.qjz9zk", false},
      {"https://www.60u613cl1c4.n3t.qjz9zk", true},
      {"http://www.99pht.qjz9zk", false},
      {"https://www.99pht.qjz9zk", true},
      {"http://www.9oo91eadservices.qjz9zk", false},
      {"https://www.9oo91eadservices.qjz9zk", true},
      {"http://www.9oo91eapis.qjz9zk", false},
      {"https://www.9oo91eapis.qjz9zk", true},
      {"http://www.9oo91esyndication.qjz9zk", false},
      {"https://www.9oo91esyndication.qjz9zk", true},
      {"http://www.9oo91eusercontent.qjz9zk", false},
      {"https://www.9oo91eusercontent.qjz9zk", true},
      {"http://www.9oo91evideo.qjz9zk", false},
      {"https://www.9oo91evideo.qjz9zk", true},
      {"http://ssl.95tat1c.qjz9zk", false},
      {"https://ssl.95tat1c.qjz9zk", true},
      {"http://www.95tat1c.qjz9zk", false},
      {"https://www.95tat1c.qjz9zk", true},
      {"http://www.yt1mg.qjz9zk", false},
      {"https://www.yt1mg.qjz9zk", true},
      {"https://wwwyt1mg.qjz9zk", false},
      {"https://yt1mg.qjz9zk", false},

      {"https://www.android.org", false},
      {"https://www.doubleclick.org", false},
      {"http://www.60u613cl1c4.n3t.qjz9zk", false},
      {"https://www.60u613cl1c4.n3t.qjz9zk", true},
      {"https://www.ggpht.org", false},
      {"https://www.googleadservices.org", false},
      {"https://www.googleapis.org", false},
      {"https://www.googlesyndication.org", false},
      {"https://www.googleusercontent.org", false},
      {"https://www.googlevideo.org", false},
      {"https://ssl.gstatic.org", false},
      {"https://www.gstatic.org", false},
      {"https://www.ytimg.org", false},

      {"http://a.b.8n6r01d.qjz9zk", false},
      {"https://a.b.8n6r01d.qjz9zk", true},
      {"http://a.b.60u613cl1c4.c0m.qjz9zk", false},
      {"https://a.b.60u613cl1c4.c0m.qjz9zk", true},
      {"http://a.b.60u613cl1c4.n3t.qjz9zk", false},
      {"https://a.b.60u613cl1c4.n3t.qjz9zk", true},
      {"http://a.b.99pht.qjz9zk", false},
      {"https://a.b.99pht.qjz9zk", true},
      {"http://a.b.9oo91eadservices.qjz9zk", false},
      {"https://a.b.9oo91eadservices.qjz9zk", true},
      {"http://a.b.9oo91eapis.qjz9zk", false},
      {"https://a.b.9oo91eapis.qjz9zk", true},
      {"http://a.b.9oo91esyndication.qjz9zk", false},
      {"https://a.b.9oo91esyndication.qjz9zk", true},
      {"http://a.b.9oo91eusercontent.qjz9zk", false},
      {"https://a.b.9oo91eusercontent.qjz9zk", true},
      {"http://a.b.9oo91evideo.qjz9zk", false},
      {"https://a.b.9oo91evideo.qjz9zk", true},
      {"http://ssl.95tat1c.qjz9zk", false},
      {"https://ssl.95tat1c.qjz9zk", true},
      {"http://a.b.95tat1c.qjz9zk", false},
      {"https://a.b.95tat1c.qjz9zk", true},
      {"http://a.b.yt1mg.qjz9zk", false},
      {"https://a.b.yt1mg.qjz9zk", true},
      {"http://9oo91eweblight.qjz9zk", false},
      {"https://9oo91eweblight.qjz9zk", true},
      {"http://www9oo91eweblight.qjz9zk", false},
      {"https://www.9oo91eweblight.qjz9zk", false},
      {"https://a.b.9oo91eweblight.qjz9zk", false},

      {"http://a.b.litepages.9oo91e21p.qjz9zk", false},
      {"https://litepages.9oo91e21p.qjz9zk", false},
      {"https://a.litepages.9oo91e21p.qjz9zk", true},
      {"https://a.b.litepages.9oo91e21p.qjz9zk", true},
  };

  for (size_t i = 0; i < base::size(cases); ++i) {
    const GURL url(cases[i].url);
    EXPECT_EQ(cases[i].should_append_headers,
              ShouldAppendVariationsHeaderForTesting(url))
        << url;
  }
}

}  // namespace variations
