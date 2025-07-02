# Changelog

## [3.1.0](https://github.com/realityking/node-zopflipng/compare/v3.0.0...v3.1.0) (2025-07-02)


### Features

* add provenance when publishing the npm package ([e6f19a6](https://github.com/realityking/node-zopflipng/commit/e6f19a601437739dee1c8827848706ce219b7b10))

## [3.0.0](https://github.com/realityking/node-zopflipng/compare/v2.0.0...v3.0.0) (2025-01-20)


### ⚠ BREAKING CHANGES

* remove prebuild for glibc 2.17. Now require glibc 2.28.
* drop support for Node.js 14 and 16

### Features

* **deps:** upgrade lodepng to version 20241228 ([#388](https://github.com/realityking/node-zopflipng/issues/388)) ([814b100](https://github.com/realityking/node-zopflipng/commit/814b1004399cf036d7e3a9b80fbdb5dd0075dd16))
* provide prebuild for linux arm64 with musl libc ([5c7dd86](https://github.com/realityking/node-zopflipng/commit/5c7dd8649d6ad38a71b73d58c49dc33c41d363e1))
* provide prebuild for macos with Apple Silicone ([408ee5b](https://github.com/realityking/node-zopflipng/commit/408ee5b386ae1bcb6c2b12b76ee6a5ac5be0c8b2))


### Bug Fixes

* avoid potential crashed when loading another addon using node-addon-api on macOS by enabling -fvisibility=hidden ([b492846](https://github.com/realityking/node-zopflipng/commit/b492846eb545214958e5f08a6306b920b7a59e4d))


### Miscellaneous Chores

* drop support for Node.js 14 and 16 ([5baae5a](https://github.com/realityking/node-zopflipng/commit/5baae5ad69852311727cec32a4b002e591080ac9))
* remove prebuild for glibc 2.17. Now require glibc 2.28. ([9e725e2](https://github.com/realityking/node-zopflipng/commit/9e725e2a68bc483fcb3b6517600bda1ab8812411))

## [2.0.0](https://github.com/realityking/node-zopflipng/compare/v1.1.0...v2.0.0) (2023-03-09)


### ⚠ BREAKING CHANGES

* removes armv6 builds
* drop support for Node.js 10 and 12

### Features

* **deps:** update lodepng to version 20221108 ([#289](https://github.com/realityking/node-zopflipng/issues/289)) ([df6b7ec](https://github.com/realityking/node-zopflipng/commit/df6b7ec2b6dbc68571003bc6bb2ba4ab1d64a05d))
* use an older glibc version for linux-arm64 builds to improve compatability ([810f9df](https://github.com/realityking/node-zopflipng/commit/810f9dfdc3fdbc5e8ff58590e98606e052f6bc55))


### Miscellaneous Chores

* drop support for Node.js 10 and 12 ([a6b2db3](https://github.com/realityking/node-zopflipng/commit/a6b2db358987ae16714b2bf79db33b5af1cc541c))
* removes armv6 builds ([e7f4217](https://github.com/realityking/node-zopflipng/commit/e7f421746f0a1bcdb0ef00adfaa84efe1d135136))

## [1.1.0](https://github.com/realityking/node-zopflipng/compare/v1.0.0...v1.1.0) (2022-06-03)


### Features

* add "keepChunks" option to preseve selected chunks ([#22](https://github.com/realityking/node-zopflipng/issues/22)) ([a00adaf](https://github.com/realityking/node-zopflipng/commit/a00adaf1da436b47a38d11fc763db3b02a664804))
* **deps:** update lodepng to version 20220109. Comes with a a small speed improvement for PNG decoding. ([518c904](https://github.com/realityking/node-zopflipng/commit/518c90449d58b5ff512ec19cad6b728b224e4c49))
* enable link time optimization to improve performance ([#24](https://github.com/realityking/node-zopflipng/issues/24)) ([76332ab](https://github.com/realityking/node-zopflipng/commit/76332ab464fe3387185cf5f8ad9dad8dcf93f6c2))

## 1.0.0 (2020-12-21)

Initial release
