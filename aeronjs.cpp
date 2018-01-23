#include <nan.h>
#include <uv.h>
#include <mutex>
#include <queue>
#include <iostream>
#include "aeron.h"

struct AeronData {
  char* buf;
  int len;
};

uv_async_t g_uv_async;
std::mutex g_queue_mutex;
std::vector<AeronData> g_queue;
v8::Persistent<v8::Function> g_on_data;

int poll_callback(char* buf, int len) {
  {
    std::lock_guard<std::mutex> guard(g_queue_mutex);
    g_queue.push_back(AeronData {buf, len});
  }
  uv_async_send(&g_uv_async);
  // std::cout << "poll_callback: " << g_queue.size() << std::endl;

  return 0;
}

// NOTE: we actually don't use uv's async to pass around callback/queue
// since we know we only have 1 callback and 1 queue, globals are much simpler
void uv_async_callback(uv_async_t*) {
  std::vector<AeronData> queue;
  {
    std::lock_guard<std::mutex> guard(g_queue_mutex);
    g_queue.swap(queue);
  }
  // std::cout << "uv_async_callback: " << queue.size() << std::endl;

  using namespace v8;
  auto* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  auto globalCtx = isolate->GetCurrentContext()->Global();
  Local<Function> cb = Local<Function>::New(Isolate::GetCurrent(), g_on_data);
  for (AeronData const& data : queue) {
    auto buffer = Nan::NewBuffer(data.buf, data.len, [](char *data, void *hint) {}, nullptr);
    Local<Value> argv[] = { buffer.ToLocalChecked() };
    Nan::MakeCallback(globalCtx, cb, 1, argv);
  }
}

void aeron_init(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() != 4) {
    Nan::ThrowTypeError("Must call with 4 arguments");
    return;
  }
  Nan::Utf8String dir(info[0]->ToString());
  Nan::Utf8String channel(info[1]->ToString());
  int stream_id = info[2]->NumberValue();
  g_on_data.Reset(v8::Isolate::GetCurrent(), info[3].As<v8::Function>());

  int res = aeron_initialize(*dir);
  if (res < 0) {
    Nan::ThrowTypeError("Failed to init Aeron");
    return;
  }
  int sub_idx = aeron_add_subscription(*channel, stream_id);
  if (sub_idx < 0) {
    Nan::ThrowTypeError("Failed to add Aeron subscription");
    return;
  }

  uv_async_init(uv_default_loop(), &g_uv_async, uv_async_callback);
  aeron_poll(sub_idx, &poll_callback);
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::SetMethod(module, "exports", aeron_init);
}

NODE_MODULE(aeronjs, Init)
