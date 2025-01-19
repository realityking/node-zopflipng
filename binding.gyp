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
      'cflags': [
        '-fno-exceptions',
        '-Wextra',
        '-Wall',
        '-Wno-unused-function',
        '-Wno-unused-parameter',
        '-ansi',
        '-pedantic',
        '-O3',
        '-flto',
      ],
      'cflags_cc': [
        '-fno-exceptions',
        '-Wextra',
        '-Wall',
        '-std=c++17',
        '-pedantic',
        '-O3',
        '-flto',
      ],
      'ldflags': [
        '-flto'
      ],
      'conditions': [
        ['OS=="mac"', {
          'cflags+': ['-fvisibility=hidden']
        }]
      ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',
        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'OTHER_CFLAGS': [
          '-fno-exceptions',
          '-Wextra',
          '-Wall',
          '-Wno-unused-function',
          '-Wno-unused-parameter',
          '-ansi',
          '-pedantic',
          '-O3',
          '-flto'
        ],
        'OTHER_CPLUSPLUSFLAGS': [
          '-fno-exceptions',
          '-Wextra',
          '-Wall',
          '-std=c++17',
          '-pedantic',
          '-O3',
          '-flto'
        ],
        'OTHER_LDFLAGS': [
          '-flto'
        ]
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS",
        "NODE_ADDON_API_DISABLE_DEPRECATED",
        "NAPI_VERSION=3",
        "NDEBUG"
      ],
    }
  ]
}
