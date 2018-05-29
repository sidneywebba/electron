// Copyright (c) 2018 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/api/atom_api_net_log.h"
#include "atom/browser/atom_browser_client.h"
#include "atom/common/native_mate_converters/callback.h"
#include "atom/common/native_mate_converters/file_path_converter.h"
#include "atom/common/node_includes.h"
#include "base/callback.h"
#include "content/public/common/content_switches.h"
#include "native_mate/dictionary.h"
#include "native_mate/handle.h"

namespace atom {

namespace api {

NetLog::NetLog(v8::Isolate* isolate) {
  Init(isolate);

  net_log_ = atom::AtomBrowserClient::Get()->GetNetLog();
}

NetLog::~NetLog() {}

// static
v8::Local<v8::Value> NetLog::Create(v8::Isolate* isolate) {
  return mate::CreateHandle(isolate, new NetLog(isolate)).ToV8();
}

void NetLog::StartLogging(mate::Arguments* args) {
  if (args->Length() >= 1) {
    base::FilePath path;
    if (!args->GetNext(&path)) {
      args->ThrowError("Invalid file path");
      return;
    }

    net_log_->StartDynamicLogging(path);
    return;
  }

  args->ThrowError("File path required");
}

bool NetLog::IsLogging() {
  return net_log_->IsDynamicLogging();
}

void NetLog::StopLogging(mate::Arguments* args) {
  base::OnceClosure callback;
  args->GetNext(&callback);

  net_log_->StopDynamicLogging(std::move(callback));
}

// static
void NetLog::BuildPrototype(v8::Isolate* isolate,
                            v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(mate::StringToV8(isolate, "NetLog"));
  mate::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetProperty("currentlyLogging", &NetLog::IsLogging)
      .SetMethod("startLogging", &NetLog::StartLogging)
      .SetMethod("stopLogging", &NetLog::StopLogging);
}

}  // namespace api

}  // namespace atom

namespace {

using atom::api::NetLog;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();

  mate::Dictionary dict(isolate, exports);
  dict.Set("netLog", NetLog::Create(isolate));
  dict.Set("NetLog", NetLog::GetConstructor(isolate)->GetFunction());
}

}  // namespace

NODE_BUILTIN_MODULE_CONTEXT_AWARE(atom_browser_net_log, Initialize)
