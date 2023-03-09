#include <napi.h>

#include <sstream>

#define LODEPNG_NO_COMPILE_DISK
#include "lodepng/lodepng.h"
#include "zopflipng/zopflipng_lib.h"

using namespace Napi;

// Based on node-zopfli Pierre Inglebert. Licensed under MIT.
int parseOptions(const Napi::Object& options, ZopfliPNGOptions& png_options,
                 Napi::Env env, Napi::Error& error) {
  Napi::Value option_value;

  if (options.IsEmpty()) {
    return 0;
  }

  // Allow altering hidden colors of fully transparent pixels
  if (options.Has("lossyTransparent")) {
    option_value = options.Get("lossyTransparent");
    if (!option_value.IsBoolean()) {
      error =
          Napi::TypeError::New(env, "Wrong type for option 'lossyTransparent'");
      return 1;
    }
    png_options.lossy_transparent = option_value.As<Napi::Boolean>().Value();
  }

  // Convert 16-bit per channel images to 8-bit per channel
  if (options.Has("lossy8bit")) {
    option_value = options.Get("lossy8bit");
    if (!option_value.IsBoolean()) {
      error = Napi::TypeError::New(env, "Wrong type for option 'lossy8bit'");
      return 1;
    }
    png_options.lossy_8bit = option_value.As<Napi::Boolean>().Value();
  }

  // "more" iterations
  if (options.Has("more")) {
    option_value = options.Get("more");
    if (!option_value.IsBoolean()) {
      error = Napi::TypeError::New(env, "Wrong type for option 'more'");
      return 1;
    }
    if (option_value.As<Napi::Boolean>().Value()) {
      png_options.num_iterations *= 4;
      png_options.num_iterations_large *= 4;
    }
  }

  // Zopfli number of iterations
  if (options.Has("iterations")) {
    option_value = options.Get("iterations");
    if (!option_value.IsNumber()) {
      error = Napi::TypeError::New(env, "Wrong type for option 'iterations'");
      return 1;
    }
    int num = option_value.As<Napi::Number>().Int32Value();

    // 0 iterations would mean no optimization
    // This matches the behavior of the zopflipng CLI
    if (num < 1) num = 1;

    png_options.num_iterations = num;
    png_options.num_iterations_large = num;
  }

  // Preserve certain chunks from the original PNG
  if (options.Has("keepChunks")) {
    option_value = options.Get("keepChunks");
    if (!option_value.IsArray()) {
      error = Napi::TypeError::New(env, "Wrong type for option 'keepChunks'");
      return 1;
    }
    const Napi::Array inputArray = option_value.As<Napi::Array>();
    const uint32_t length = inputArray.Length();

    if (length > 0) {
      png_options.keepchunks.reserve(length);

      for (uint32_t i = 0; i < length; ++i) {
        const Napi::Value val = inputArray[i];
        if (!val.IsString()) {
          error =
              Napi::TypeError::New(env, "Wrong type for option 'keepChunks'");
          return 1;
        }
        png_options.keepchunks.push_back(val.As<Napi::String>().Utf8Value());
      }
    }
  }

  return 0;
}

int check_png_encoding(const std::vector<unsigned char> inputPng,
                       const std::vector<unsigned char> outputPng,
                       ZopfliPNGOptions png_options) {
  int error = 0;
  std::vector<unsigned char> image;
  unsigned w, h;

  error = lodepng::decode(image, w, h, outputPng);
  if (!error) {
    std::vector<unsigned char> origimage;
    unsigned origw, origh;
    lodepng::decode(origimage, origw, origh, inputPng);
    if (origw != w || origh != h || origimage.size() != image.size()) {
      error = 1;
    } else {
      for (size_t i = 0; i < image.size(); i += 4) {
        bool same_alpha = image[i + 3] == origimage[i + 3];
        bool same_rgb = (png_options.lossy_transparent && image[i + 3] == 0) ||
                        (image[i + 0] == origimage[i + 0] &&
                         image[i + 1] == origimage[i + 1] &&
                         image[i + 2] == origimage[i + 2]);
        if (!same_alpha || !same_rgb) {
          error = 1;
          break;
        }
      }
    }
  }

  return error;
}

#define NAPI_THROW_EMPTY_BUFFER(e) \
  NAPI_THROW(e, Napi::Buffer<unsigned char>::New(info.Env(), 0))

Napi::Buffer<unsigned char> OptimzeZopfliPNGSync(
    const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Error errorValue;
  int error = 0;

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    NAPI_THROW_EMPTY_BUFFER(
        Napi::TypeError::New(env, "input must be a buffer"));
  }

  ZopfliPNGOptions png_options;
  if (info.Length() >= 2) {
    if (!info[1].IsObject()) {
      NAPI_THROW_EMPTY_BUFFER(
          Napi::TypeError::New(env, "options must be an object"));
    }
    Napi::Object options = info[1].ToObject();
    error = parseOptions(options, png_options, env, errorValue);
    if (error) {
      NAPI_THROW_EMPTY_BUFFER(errorValue);
    }
  }

  Napi::Buffer<unsigned char> inputBuffer =
      info[0].As<Napi::Buffer<unsigned char>>();
  size_t inputBufferSize = inputBuffer.Length();
  const unsigned char* inputBufferData = inputBuffer.Data();
  const std::vector<unsigned char> inputPng(inputBufferData,
                                            inputBufferData + inputBufferSize);
  std::vector<unsigned char> outputPng;

  bool verbose = false;

  error = ZopfliPNGOptimize(inputPng, png_options, verbose, &outputPng);
  if (error) {
    if (error == 1) {
      NAPI_THROW_EMPTY_BUFFER(Napi::Error::New(env, "Decoding error"));
    } else {
      std::ostringstream errstr;
      errstr << "Decoding error " << error << ": " << lodepng_error_text(error);
      NAPI_THROW_EMPTY_BUFFER(Napi::Error::New(env, errstr.str()));
    }
  }

  error = check_png_encoding(inputPng, outputPng, png_options);
  if (error) {
    // Reset the error to 0 and set the output to the input PNG
    // We'll just pretend we couldn't optimize this PNG
    error = 0;
    outputPng = inputPng;
  }

  // If the output is larger (or equal) to the input, preseve it
  if (outputPng.size() >= inputPng.size()) {
    outputPng = inputPng;
  }

  return Napi::Buffer<unsigned char>::Copy(
      info.Env(), reinterpret_cast<unsigned char*>(&outputPng[0]),
      outputPng.size());
}

class PngOptimizeAsyncWorker : public Napi::AsyncWorker {
 public:
  PngOptimizeAsyncWorker(Napi::Env& env, ZopfliPNGOptions& png_options,
                         const std::vector<unsigned char>& inputPng)
      : Napi::AsyncWorker(env),
        png_options(png_options),
        inputPng(inputPng),
        deferred(Napi::Promise::Deferred::New(env)) {}

  ~PngOptimizeAsyncWorker(){};

  // Executed inside the worker-thread.
  // It is not safe to access JS engine data structure
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    bool verbose = false;
    int error = 0;

    error = ZopfliPNGOptimize(inputPng, png_options, verbose, &outputPng);
    if (error) {
      if (error == 1) {
        SetError("Decoding error");
      } else {
        std::ostringstream errstr;
        errstr << "Decoding error " << error << ": "
               << lodepng_error_text(error);
        SetError(errstr.str());
      }
      return;
    }

    error = check_png_encoding(inputPng, outputPng, png_options);
    if (error) {
      // Reset the error to 0 and set the output to the input PNG
      // We'll just pretend we couldn't optimize this PNG
      error = 0;
      outputPng = inputPng;
    }

    // If the output is larger (or equal) to the input, preseve it
    if (outputPng.size() >= inputPng.size()) {
      outputPng = inputPng;
    }
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use JS engine data again
  void OnOK() {
    const Napi::Buffer<unsigned char> outputBuffer =
        Napi::Buffer<unsigned char>::Copy(
            Env(), reinterpret_cast<unsigned char*>(&outputPng[0]),
            outputPng.size());
    deferred.Resolve(outputBuffer);
  }

  void OnError(Napi::Error const& error) { deferred.Reject(error.Value()); }

  Napi::Promise GetPromise() { return deferred.Promise(); }

 private:
  ZopfliPNGOptions png_options;
  const std::vector<unsigned char> inputPng;
  Napi::Promise::Deferred deferred;
  std::vector<unsigned char> outputPng;
};

#define NAPI_REJECT(e, env)                                               \
  do {                                                                    \
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env); \
    deferred.Reject(e.Value());                                           \
    return deferred.Promise();                                            \
  } while (0)

Napi::Promise OptimzeZopfliPNG(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Error errorValue;

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    NAPI_REJECT(Napi::TypeError::New(env, "input must be a buffer"), env);
  }

  ZopfliPNGOptions png_options;
  if (info.Length() >= 2) {
    if (!info[1].IsObject()) {
      NAPI_REJECT(Napi::TypeError::New(env, "options must be an object"), env);
    }
    Napi::Object options = info[1].ToObject();
    int error = parseOptions(options, png_options, env, errorValue);
    if (error) {
      NAPI_REJECT(errorValue, env);
    }
  }

  Napi::Buffer<unsigned char> inputBuffer =
      info[0].As<Napi::Buffer<unsigned char>>();
  size_t inputBufferSize = inputBuffer.Length();
  const unsigned char* inputBufferData = inputBuffer.Data();
  const std::vector<unsigned char> inputPng(inputBufferData,
                                            inputBufferData + inputBufferSize);

  PngOptimizeAsyncWorker* worker =
      new PngOptimizeAsyncWorker(env, png_options, inputPng);

  auto promise = worker->GetPromise();
  worker->Queue();
  return promise;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "optimizeZopfliPngSync"),
              Napi::Function::New(env, OptimzeZopfliPNGSync));
  exports.Set(Napi::String::New(env, "optimizeZopfliPng"),
              Napi::Function::New(env, OptimzeZopfliPNG));

  return exports;
}

NODE_API_MODULE(addon, Init)
