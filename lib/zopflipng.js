'use strict';

const isPng = require('is-png');
const addon = require('../build/Release/zopflipng.node');

function optimizeZopfliPngSync(input, options = {}) {
  if (!isPng(input)) {
		return input;
	}

  return addon.optimizeZopfliPngSync(input, options);
}

module.exports =  optimizeZopfliPngSync;
