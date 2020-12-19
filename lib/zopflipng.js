'use strict';

const isPng = require('is-png');
const addon = require('../build/Release/zopflipng.node');

function optimizeZopfliPngSync(input, options = {}) {
  if (!Buffer.isBuffer(input)) {
    throw new TypeError('input must be a buffer');
  }

  if (!isPng(input)) {
		return input;
	}

  return addon.optimizeZopfliPngSync(input, options);
}

module.exports =  optimizeZopfliPngSync;
