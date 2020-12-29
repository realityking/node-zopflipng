import { join } from 'path';
import { readFileSync, writeFileSync } from 'fs';
import { readFile, writeFile } from 'fs/promises';
import {expectType, expectError} from 'tsd';

import {optimizeZopfliPng, optimizeZopfliPngSync} from '..';

// sync
const image = readFileSync(join(__dirname, 'test/fixtures/test.png'));
const result = optimizeZopfliPngSync(image);
writeFileSync(join(__dirname, '/test-out.png'), result);

// Supported options
optimizeZopfliPngSync(image, {
  lossyTransparent: true,
  lossy8bit: true,
  more: true,
  iterations: 12,
  keepChunks: ['eXif']
});

// Arrays can be empty
expectError(optimizeZopfliPngSync(image, {
  iterations: []
}));

// Unknown option
expectError(optimizeZopfliPngSync(image, {unknown: true}));

// Not an object as options
expectError(optimizeZopfliPngSync(image, ''));

// Incorect options
expectError(optimizeZopfliPngSync(image, {
  lossyTransparent: 1
}));
expectError(optimizeZopfliPngSync(image, {
  iterations: '12'
}));
expectError(optimizeZopfliPngSync(image, {
  iterations: ['12', 12]
}));

// Not an object as input
expectError(optimizeZopfliPngSync(''));

// No arguments
expectError(optimizeZopfliPngSync());

// async
readFile(join(__dirname, 'test/fixtures/test.png'))
.then(image => optimizeZopfliPng(image))
.then(image => {
  expectType<Buffer>(image);
  return writeFile(join(__dirname, '/test-out.png'), result);
});

// Unknown option
expectError(optimizeZopfliPng(image, {unknown: true}));

// Not an object as options
expectError(optimizeZopfliPng(image, ''));

// Not an object as input
expectError(optimizeZopfliPng(''));

// No arguments
expectError(optimizeZopfliPng());
