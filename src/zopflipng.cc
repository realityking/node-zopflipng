#include <string.h>
#include <napi.h>
#include "zopflipng/zopflipng_lib.h"

using namespace Napi;

Napi::Buffer<unsigned char> OptimzeZopfliPNGSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    Napi::TypeError::New(env, "input must be a buffer").ThrowAsJavaScriptException();
  }

  Napi::Buffer<unsigned char> inputBuffer = info[0].As<Napi::Buffer<unsigned char>>();
  size_t inputBufferSize = inputBuffer.Length();
  const unsigned char * inputBufferData = inputBuffer.Data();
  const std::vector<unsigned char> inputputPng(inputBufferData, inputBufferData + inputBufferSize);

  std::vector<unsigned char> outputPng;
  unsigned char* outputData = 0;
  size_t outputSize = 0;

  ZopfliPNGOptions png_options;
  bool verbose = false;

  int ret = ZopfliPNGOptimize(inputputPng, png_options, verbose, &outputPng);

  outputSize = outputPng.size();
  outputData = (unsigned char*) malloc(outputSize);
  if (!outputData) {
    Napi::TypeError::New(env, "can't allocate memory for output png").ThrowAsJavaScriptException();
  }

  memcpy(outputData,
         reinterpret_cast<unsigned char*>(&outputPng[0]),
         outputSize);

  return Napi::Buffer<unsigned char>::New(info.Env(), outputData, outputSize);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "optimizeZopfliPngSync"),
              Napi::Function::New(env, OptimzeZopfliPNGSync));
  return exports;
}

NODE_API_MODULE(addon, Init)
