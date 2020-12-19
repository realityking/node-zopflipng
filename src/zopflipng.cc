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
    int num = option_value.As<Napi::Number>().Int32Value();

    // 0 iterations would mean no optimization
    // This matches the behavior of the zopflipng CLI
    if (num < 1) num = 1;

    png_options.num_iterations = num;
    png_options.num_iterations_large = num;
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
      Napi::TypeError::New(env, "options must be an object").ThrowAsJavaScriptException();
    }
    Napi::Object options = info[1].ToObject();
    parseOptions(options, png_options);
  }

  Napi::Buffer<unsigned char> inputBuffer = info[0].As<Napi::Buffer<unsigned char>>();
  size_t inputBufferSize = inputBuffer.Length();
  const unsigned char * inputBufferData = inputBuffer.Data();
  const std::vector<unsigned char> inputPng(inputBufferData, inputBufferData + inputBufferSize);

  std::vector<unsigned char> outputPng;
  unsigned char* outputData = 0;
  size_t outputSize = 0;

  bool verbose = false;
  int error = 0;

  error = ZopfliPNGOptimize(inputPng, png_options, verbose, &outputPng);
  if (error) {
    if (error == 1) {
      Napi::Error::New(env, "Decoding error").ThrowAsJavaScriptException();
    } else {
      std::ostringstream errstr;
      errstr << "Decoding error " << error << ": " << lodepng_error_text(error);
      Napi::Error::New(env, errstr.str()).ThrowAsJavaScriptException();
    }
  }

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
        bool same_rgb =
            (png_options.lossy_transparent && image[i + 3] == 0) ||
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

class PngOptimizeAsyncWorker : public Napi::AsyncWorker {
  public:
    PngOptimizeAsyncWorker(Napi::Env &env, ZopfliPNGOptions &png_options, const std::vector<unsigned char> &inputPng)
      : Napi::AsyncWorker(env), png_options(png_options), inputPng(inputPng), deferred(Napi::Promise::Deferred::New(env)), outputData(0), outputSize(0)
    {}

  ~PngOptimizeAsyncWorker() {};

  // Executed inside the worker-thread.
  // It is not safe to access JS engine data structure
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute() {
    std::vector<unsigned char> outputPng;
    bool verbose = false;
    int error = 0;

    error = ZopfliPNGOptimize(inputPng, png_options, verbose, &outputPng);
    if (error) {
      if (error == 1) {
        SetError("Decoding error");
      } else {
        std::ostringstream errstr;
        errstr << "Decoding error " << error << ": " << lodepng_error_text(error);
        SetError(errstr.str());
      }
      return;
    }

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
          bool same_rgb =
              (png_options.lossy_transparent && image[i + 3] == 0) ||
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

    outputSize = outputPng.size();
    outputData = (unsigned char*) malloc(outputSize);
    if (!outputData) {
      SetError("can't allocate memory for output png");
      return;
    }

    memcpy(outputData,
           reinterpret_cast<unsigned char*>(&outputPng[0]),
           outputSize);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use JS engine data again
  void OnOK() {
    Napi::Buffer<unsigned char> outputBuffer = Napi::Buffer<unsigned char>::New(Env(), outputData, outputSize);
    deferred.Resolve(outputBuffer);
  }

  void OnError(Napi::Error const &error) {
    deferred.Reject(error.Value());
  }

  Napi::Promise GetPromise() { return deferred.Promise(); }

  private:
    ZopfliPNGOptions png_options;
    const std::vector<unsigned char> inputPng;
    Napi::Promise::Deferred deferred;
    unsigned char* outputData = 0;
    size_t outputSize = 0;
};

Napi::Promise OptimzeZopfliPNG(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    auto deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(Napi::TypeError::New(env, "input must be a buffer").Value());
    return deferred.Promise();
  }

  ZopfliPNGOptions png_options;
  if(info.Length() >= 2) {
    if (!info[1].IsObject()) {
      auto deferred = Napi::Promise::Deferred::New(env);
      deferred.Reject(Napi::TypeError::New(env, "options must be an object").Value());
      return deferred.Promise();
    }
    Napi::Object options = info[1].ToObject();
    parseOptions(options, png_options);
  }

  Napi::Buffer<unsigned char> inputBuffer = info[0].As<Napi::Buffer<unsigned char>>();
  size_t inputBufferSize = inputBuffer.Length();
  const unsigned char * inputBufferData = inputBuffer.Data();
  const std::vector<unsigned char> inputPng(inputBufferData, inputBufferData + inputBufferSize);

  PngOptimizeAsyncWorker *worker = new PngOptimizeAsyncWorker(env, png_options, inputPng);

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
