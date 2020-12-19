# node-zopflipng

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
