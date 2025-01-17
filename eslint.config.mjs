import neostandard from 'neostandard'
import ava from 'eslint-plugin-ava'

export default [
  ...neostandard(),
  ava.configs['flat/recommended'],
  {
    rules: {
      'ava/no-ignored-test-files': 'off'
    }
  }
]
