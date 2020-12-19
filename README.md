# node-zopflipng

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

### Options

| Name               | Type      | Default                | Lossless | Description |
|--------------------|-----------|------------------------|----------|------------ |
| `lossyTransparent` | `boolean` | `false`                | ✗        | Remove colors behind alpha channel 0. No visual difference, removes hidden information. |
| `lossy8bit`        | `boolean` | `false`                | ✗        | Convert 16-bit per channel image to 8-bit per channel. |
| `more`             | `boolean` | `false`                | ✓        | Compress more. Uses more iterations depending on file size. |
| `iterations`       | `integer` | `15`/`5` (small/large) | ✓        | Number of iterations, more iterations makes it slower but provides slightly better compression. Default: 15 for small files, 5 for large files. Overrides `more` option. |
