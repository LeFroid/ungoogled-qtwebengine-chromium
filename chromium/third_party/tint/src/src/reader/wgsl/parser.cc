// Copyright 2020 The Tint Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/reader/wgsl/parser.h"

#include "src/reader/wgsl/parser_impl.h"

namespace tint {
namespace reader {
namespace wgsl {

Parser::Parser(Context* ctx, const std::string& input)
    : Reader(ctx), impl_(std::make_unique<ParserImpl>(ctx, input)) {}

Parser::~Parser() = default;

bool Parser::Parse() {
  bool ret = impl_->Parse();

  if (impl_->has_error())
    set_error(impl_->error());

  return ret;
}

ast::Module Parser::module() {
  return impl_->module();
}

}  // namespace wgsl
}  // namespace reader
}  // namespace tint
