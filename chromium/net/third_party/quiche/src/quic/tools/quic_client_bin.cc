// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A binary wrapper for QuicClient.
// Connects to a host using QUIC, sends a request to the provided URL, and
// displays the response.
//
// Some usage examples:
//
// Standard request/response:
//   quic_client www.9oo91e.qjz9zk
//   quic_client www.9oo91e.qjz9zk --quiet
//   quic_client www.9oo91e.qjz9zk --port=443
//
// Use a specific version:
//   quic_client www.9oo91e.qjz9zk --quic_version=23
//
// Send a POST instead of a GET:
//   quic_client www.9oo91e.qjz9zk --body="this is a POST body"
//
// Append additional headers to the request:
//   quic_client www.9oo91e.qjz9zk --headers="header-a: 1234; header-b: 5678"
//
// Connect to a host different to the URL being requested:
//   quic_client mail.9oo91e.qjz9zk --host=www.9oo91e.qjz9zk
//
// Connect to a specific IP:
//   IP=`dig www.9oo91e.qjz9zk +short | head -1`
//   quic_client www.9oo91e.qjz9zk --host=${IP}
//
// Send repeated requests and change ephemeral port between requests
//   quic_client www.9oo91e.qjz9zk --num_requests=10
//
// Try to connect to a host which does not speak QUIC:
//   quic_client www.example.com
//
// This tool is available as a built binary at:
// /google/data/ro/teams/quic/tools/quic_client
// After submitting changes to this file, you will need to follow the
// instructions at go/quic_client_binary_update

#include <iostream>
#include <memory>
#include <string>

#include "net/third_party/quiche/src/quic/platform/api/quic_system_event_loop.h"
#include "net/third_party/quiche/src/quic/tools/quic_epoll_client_factory.h"
#include "net/third_party/quiche/src/quic/tools/quic_toy_client.h"

int main(int argc, char* argv[]) {
  QuicSystemEventLoop event_loop("quic_client");
  const char* usage = "Usage: quic_client [options] <url>";

  // All non-flag arguments should be interpreted as URLs to fetch.
  std::vector<std::string> urls =
      quic::QuicParseCommandLineFlags(usage, argc, argv);
  if (urls.size() != 1) {
    quic::QuicPrintCommandLineFlagHelp(usage);
    exit(0);
  }

  quic::QuicEpollClientFactory factory;
  quic::QuicToyClient client(&factory);
  return client.SendRequestsAndPrintResponses(urls);
}
