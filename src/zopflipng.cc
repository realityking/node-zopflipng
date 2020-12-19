#include <sstream>
#include <napi.h>
#include "lodepng/lodepng.h"
#include "zopflipng/zopflipng_lib.h"

using namespace Napi;

// Based on node-zopfli Pierre Inglebert. Licensed under MIT.
void parseOptions(const Napi::Object& options, ZopfliPNGOptions& png_options) {
  Napi::Value option_value;
  const Napi::Env env = options.Env();

  if(options.IsEmpty()) {
    return;
  }

  // Allow altering hidden colors of fully transparent pixels
  if (options.Has("lossyTransparent")) {
    option_value = options.Get("lossyTransparent");
    if(!option_value.IsBoolean()) {
      Napi::TypeError::New(env, "Wrong type for option 'lossyTransparent'").ThrowAsJavaScriptException();

    }
    png_options.lossy_transparent = option_value.As<Napi::Boolean>().Value();
  }

  // Convert 16-bit per channel images to 8-bit per channel
  if (options.Has("lossy8bit")) {
    option_value = options.Get("lossy8bit");
    if(!option_value.IsBoolean()) {
      Napi::TypeError::New(env, "Wrong type for option 'lossy8bit'").ThrowAsJavaScriptException();
    }
    png_options.lossy_8bit = option_value.As<Napi::Boolean>().Value();
  }

  // Zopfli number of iterations
  if (options.Has("iterations")) {
    option_value = options.Get("iterations");
    if(!option_value.IsNumber()) {
      Napi::TypeError::New(env, "Wrong type for option 'iterations'").ThrowAsJavaScriptException();
    }
    png_options.num_iterations = option_value.As<Napi::Number>().Int32Value();
  }

  // Zopfli number of iterations on images > 200 KiB
  if (options.Has("iterationsLarge")) {
    option_value = options.Get("iterationsLarge");
    if(!option_value.IsNumber()) {
      Napi::TypeError::New(env, "Wrong type for option 'iterationsLarge'").ThrowAsJavaScriptException();
    }
    png_options.num_iterations_large = option_value.As<Napi::Number>().Int32Value();
  }
}

Napi::Buffer<unsigned char> OptimzeZopfliPNGSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    Napi::TypeError::New(env, "input must be a buffer").ThrowAsJavaScriptException();
  }

  ZopfliPNGOptions png_options;
  if(info.Length() >= 2) {
    if (!info[1].IsObject()) {
      Napi::TypeError::New(env, "options much be an object").ThrowAsJavaScriptException();
    }
    Napi::Object options = info[1].ToObject();
    parseOptions(options, png_options);
  }

  Napi::Buffer<unsigned char> inputBuffer = info[0].As<Napi::Buffer<unsigned char>>();
  size_t inputBufferSize = inputBuffer.Length();
  const unsigned char * inputBufferData = inputBuffer.Data();
  const std::vector<unsigned char> inputputPng(inputBufferData, inputBufferData + inputBufferSize);

  std::vector<unsigned char> outputPng;
  unsigned char* outputData = 0;
  size_t outputSize = 0;

  bool verbose = false;

  int error = ZopfliPNGOptimize(inputputPng, png_options, verbose, &outputPng);
  if (error) {
    if (error == 1) {
      Napi::Error::New(env, "Decoding error").ThrowAsJavaScriptException();
    } else {
      std::ostringstream errstr;
      errstr << "Decoding error " << error << ": " << lodepng_error_text(error);
      Napi::Error::New(env, errstr.str()).ThrowAsJavaScriptException();
    }
  }

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
