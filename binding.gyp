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
        'src'
      ],
      'dependencies': [
        "<!(node -p \"require('node-addon-api').targets\"):node_addon_api"
      ],
      'cflags': [
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
        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
        'OTHER_CFLAGS': [
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
      "defines": [
        "NODE_ADDON_API_DISABLE_DEPRECATED",
        "NAPI_VERSION=3",
        "NDEBUG"
      ],
    }
  ]
}
