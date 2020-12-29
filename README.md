# node-zopflipng

[![npm](https://img.shields.io/npm/v/node-zopflipng)](https://www.npmjs.com/package/node-zopflipng) [![Built on N-API v3](https://img.shields.io/badge/N--API-v3-green.svg)](https://nodejs.org/api/n-api.html#n_api_n_api_version_matrix) ![CI](https://github.com/realityking/node-zopflipng/workflows/CI/badge.svg)

Node.js bindings for [zopflipng](https://en.wikipedia.org/wiki/Zopfli#PNG_optimization).

Optimizes PNG images for size. This is very slow and should only be done for static resources.

## Install

```
$ npm install --save node-zopflipng
```

## Usage

```js
const {optimizeZopfliPng} = require('node-zopflipng');
const {readFile, writeFile} = require('fs/promises');

readFile('./raw.png')
.then(buffer => optimizeZopfliPng(buffer))
.then(buffer => writeFile('./optimized.png', buffer));
```

### API

#### optimizeZopfliPng(input, [options]): Promise<Buffer>

#### input

Type: `Buffer`

#### options

Type: `Object`

See [Options](#options) for possible values.

#### optimizeZopfliPngSync(input, [options]): Buffer

*Using this function is strongly discouraged.* Running zopflipng can easily take
more than 30s even on relatively small images (< 1 MB). `optimizeZopfliPngSync`
will block the entire process during this time.

#### input

Type: `Buffer`

#### options

Type: `Object`

See [Options](#options) for possible values.


### Options

| Name               | Type       | Default                | Lossless | Description |
|--------------------|------------|------------------------|----------|------------ |
| `lossyTransparent` | `boolean`  | `false`                | ✗        | Remove colors behind alpha channel 0. No visual difference, removes hidden information. |
| `lossy8bit`        | `boolean`  | `false`                | ✗        | Convert 16-bit per channel image to 8-bit per channel. |
| `more`             | `boolean`  | `false`                | ✓        | Compress more. Uses more iterations depending on file size. |
| `iterations`       | `integer`  | `15`/`5` (small/large) | ✓        | Number of iterations, more iterations makes it slower but provides slightly better compression. Default: 15 for small files, 5 for large files. Overrides `more` option. |
| `keepChunks`       | `string[]` | `[]`                   | ✓        | Keep metadata chunks with these names that would normally be removed, e.g. `tEXt`, `zTXt`, `iTXt`, `gAMA`. Due to adding extra data, this increases the result size. By default ZopfliPNG only keeps (and losslessly modifies) the following chunks because they are essential: `IHDR`, `PLTE`, `tRNS`, `IDAT` and `IEND`. |
