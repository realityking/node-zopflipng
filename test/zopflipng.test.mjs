import { readFile } from 'fs/promises'
import path from 'path'
import { fileURLToPath } from 'url'

import test from 'ava'
import isPng from 'is-png'

import { optimizeZopfliPng, optimizeZopfliPngSync } from 'node-zopflipng'

const __dirname = path.dirname(fileURLToPath(import.meta.url))

test('sync: optimize a PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  const data = optimizeZopfliPngSync(buf)

  t.true(data.length < buf.length)
  t.true(isPng(data))
})

test('async: optimize a PNG', async t => {
  const buf = await readFile(path.join(__dirname, 'fixtures/test.png'))
  const data = await optimizeZopfliPng(buf)

  t.true(data.length < buf.length)
  t.true(isPng(data))
})
