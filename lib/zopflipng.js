'use strict'

const isPng = require('is-png')
const { dirname } = require('path')
const binding = require('node-gyp-build')(dirname(__dirname))

function optimizeZopfliPngSync (input, options = {}) {
  if (!Buffer.isBuffer(input)) {
    throw new TypeError('input must be a buffer')
  }

  if (!isPng(input)) {
    throw new Error('input must be a valid PNG')
  }

  return binding.optimizeZopfliPngSync(input, options)
}

async function optimizeZopfliPng (input, options = {}) {
  if (!Buffer.isBuffer(input)) {
    throw new TypeError('input must be a buffer')
  }

  if (!isPng(input)) {
    throw new Error('input must be a valid PNG')
  }

  return binding.optimizeZopfliPng(input, options)
}

module.exports = {
  optimizeZopfliPng,
  optimizeZopfliPngSync
}
