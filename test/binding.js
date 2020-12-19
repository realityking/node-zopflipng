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

test('sync: optimize a PNG with 0 iterations', async t => {
	const buf = await readFile(path.join(__dirname, 'fixtures/test.png'));
	const data = optimizeZopfliPngSync(buf, {iterations: 0});

  // If zopflipng actually did 0 iterations, the file would be identical
	// Internally we assure iterations >= 1 to avoid this trap
	t.true(data.length < buf.length);
	t.true(isPng(data));
});

test('sync: optimize a PNG with "more" option', async t => {
	const buf = await readFile(path.join(__dirname, 'fixtures/test.png'));
	const data = optimizeZopfliPngSync(buf, {more: true});

	t.true(data.length < buf.length);
	t.true(isPng(data));
});

test('sync: throws error on non-PNG file', async t => {
	const buf = await readFile(__filename);
  t.throws(() => {
		optimizeZopfliPngSync(buf);
	}, {
    instanceOf: Error,
    message: 'input must be a valid PNG'
  });
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

test('async: throws error on non-PNG file', async t => {
  const buf = await readFile(__filename);
  const data = await t.throwsAsync(optimizeZopfliPng(buf), {
    instanceOf: Error,
    message: 'input must be a valid PNG'
  });
});

test('async: skip optimizing an already optimized PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test-smallest.png'));
  const data = await optimizeZopfliPng(buf);

  t.deepEqual(data, buf);
});
