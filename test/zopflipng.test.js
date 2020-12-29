'use strict'

const fs = require('fs')
const path = require('path')
const { promisify } = require('util')

const test = require('ava')
const isPng = require('is-png')
const sharp = require('sharp')

const { optimizeZopfliPng, optimizeZopfliPngSync } = require('..')

const readFile = promisify(fs.readFile)

test('sync: optimize a PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  const data = optimizeZopfliPngSync(buf)

  t.true(data.length < buf.length)
  t.true(isPng(data))
})

test('sync: optimize a PNG with 0 iterations', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  const data = optimizeZopfliPngSync(buf, { iterations: 0 })

  // If zopflipng actually did 0 iterations, the file would be identical
  // Internally we assure iterations >= 1 to avoid this trap
  t.true(data.length < buf.length)
  t.true(isPng(data))
})

test('sync: optimize a PNG with "more" option', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  const data = optimizeZopfliPngSync(buf, { more: true })

  t.true(data.length < buf.length)
  t.true(isPng(data))
})

test('sync: throws error if input is not a buffer', t => {
  t.throws(() => {
    optimizeZopfliPngSync('nope')
  }, {
    instanceOf: TypeError,
    message: 'input must be a buffer'
  })
})

test('sync: throws error on non-PNG file', async t => {
  const buf = await readFile(__filename)
  t.throws(() => {
    optimizeZopfliPngSync(buf)
  }, {
    instanceOf: Error,
    message: 'input must be a valid PNG'
  })
})

test('sync: throws error if options is not an object', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  t.throws(() => {
    optimizeZopfliPngSync(buf, 'nope')
  }, {
    instanceOf: TypeError,
    message: 'options must be an object'
  })
})

test('sync: throws error if option value has wrong type', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  t.throws(() => {
    optimizeZopfliPngSync(buf, { more: 123 })
  }, {
    instanceOf: TypeError,
    message: 'Wrong type for option \'more\''
  })
})

test('sync: skip optimizing an already optimized PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test-smallest.png'))
  const data = optimizeZopfliPngSync(buf)

  t.deepEqual(data, buf)
})

test('async: optimize a PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  const data = await optimizeZopfliPng(buf)

  t.true(data.length < buf.length)
  t.true(isPng(data))
})

test('async: optimize a PNG with "more" option', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  const data = await optimizeZopfliPng(buf, { more: true })

  t.true(data.length < buf.length)
  t.true(isPng(data))
})

test('async: throws error if input is not a buffer', async t => {
  await t.throwsAsync(optimizeZopfliPng('nope'), {
    instanceOf: TypeError,
    message: 'input must be a buffer'
  })
})

test('async: throws error on non-PNG file', async t => {
  const buf = await readFile(__filename)
  await t.throwsAsync(optimizeZopfliPng(buf), {
    instanceOf: Error,
    message: 'input must be a valid PNG'
  })
})

test('async: throws error if options is not an object', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  await t.throwsAsync(optimizeZopfliPng(buf, 'nope'), {
    instanceOf: TypeError,
    message: 'options must be an object'
  })
})

test('async: throws error if option value has wrong type', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  await t.throwsAsync(optimizeZopfliPng(buf, { more: 123 }), {
    instanceOf: TypeError,
    message: 'Wrong type for option \'more\''
  })
})

test('async: throws error if keepChunks is not an array of strings', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  await t.throwsAsync(optimizeZopfliPng(buf, { keepChunks: 123 }), {
    instanceOf: TypeError,
    message: 'Wrong type for option \'keepChunks\''
  })

  await t.throwsAsync(optimizeZopfliPng(buf, { keepChunks: ['exif', 123] }), {
    instanceOf: TypeError,
    message: 'Wrong type for option \'keepChunks\''
  })
})

test('async: keep exif metadata when requested', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/exif2c08.png'))
  const originalImage = sharp(buf)
  const originalMetadata = await originalImage.metadata()

  t.not(originalMetadata.exif, undefined)

  const compressedData = await optimizeZopfliPng(buf)
  const compressedImage = sharp(compressedData)
  const compressedMetadata = await compressedImage.metadata()

  t.is(compressedMetadata.exif, undefined)

  const keepChunksData = await optimizeZopfliPng(buf, { keepChunks: ['eXIf'] })
  const keepChunksImage = sharp(keepChunksData)
  const keepChunksMetadata = await keepChunksImage.metadata()

  t.not(keepChunksMetadata.exif, undefined)
  t.deepEqual(keepChunksMetadata.exif, originalMetadata.exif)
})

test('async: skip optimizing an already optimized PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test-smallest.png'))
  const data = await optimizeZopfliPng(buf)

  t.deepEqual(data, buf)
})
