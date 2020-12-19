'use strict';

const fs = require('fs');
const path = require('path');
const {promisify} = require('util');

const test = require('ava');
const isPng = require('is-png');

const zopflipng = require('..');

const readFile = promisify(fs.readFile);

test('optimize a PNG', async t => {
	const buf = await readFile(path.join(__dirname, 'fixtures/test.png'));
	const data = zopflipng(buf);

	t.true(data.length < buf.length);
	t.true(isPng(data));
});

test('skip optimizing a non-PNG file', async t => {
	const buf = await readFile(__filename);
	const data = zopflipng(buf);

	t.deepEqual(data, buf);
});
