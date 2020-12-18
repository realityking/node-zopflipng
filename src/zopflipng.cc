#include <napi.h>
#include "zopflipng/zopflipng_lib.h"

using namespace Napi;

Napi::String OptimzeZopfliPNGSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "world");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "optimizeZopfliPngSync"),
              Napi::Function::New(env, OptimzeZopfliPNGSync));
  return exports;
}

NODE_API_MODULE(addon, Init)
