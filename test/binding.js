'use strict';

const fs = require('fs');
const path = require('path');
const {promisify} = require('util');

const test = require('ava');
const isPng = require('is-png');

const {optimizeZopfliPng, optimizeZopfliPngSync} = require('..');

const readFile = promisify(fs.readFile);

test('sync: optimize a PNG', async t => {
	const buf = await readFile(path.join(__dirname, 'fixtures/test.png'));
	const data = optimizeZopfliPngSync(buf);

	t.true(data.length < buf.length);
	t.true(isPng(data));
});

test('sync: skip optimizing a non-PNG file', async t => {
	const buf = await readFile(__filename);
	const data = optimizeZopfliPngSync(buf);

	t.deepEqual(data, buf);
});

test('sync: skip optimizing an already optimized PNG', async t => {
	const buf = await readFile(path.join(__dirname, 'fixtures/test-smallest.png'));
	const data = optimizeZopfliPngSync(buf);

	t.deepEqual(data, buf);
});

test('async: optimize a PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'));
  const data = await optimizeZopfliPng(buf);

  t.true(data.length < buf.length);
  t.true(isPng(data));
});

test('async: skip optimizing a non-PNG file', async t => {
  const buf = await readFile(__filename);
  const data = await optimizeZopfliPng(buf);

  t.deepEqual(data, buf);
});

test('async: skip optimizing an already optimized PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test-smallest.png'));
  const data = await optimizeZopfliPng(buf);

  t.deepEqual(data, buf);
});
