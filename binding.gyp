{
  'targets': [
    {
      'target_name': 'zopflipng',
      'sources': [
        'src/zopflipng.cc',
        'src/zopflipng/zopflipng_lib.cc',
        'src/lodepng/lodepng.cc',
        'src/lodepng/lodepng_util.cc',
        "src/zopfli/deflate.c",
        "src/zopfli/util.c",
        "src/zopfli/blocksplitter.c",
        "src/zopfli/lz77.c",
        "src/zopfli/hash.c",
        "src/zopfli/tree.c",
        "src/zopfli/katajainen.c",
        "src/zopfli/cache.c",
        "src/zopfli/squeeze.c"
      ],
      'include_dirs': [
        'src',
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'dependencies': [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      "cflags": [
        "-Wextra",
        "-Wall",
        "-O3"
      ],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS",
        "NAPI_VERSION=3"
      ],
    }
  ]
}
