export type Options = {
  lossyTransparent?: boolean
  lossy8bit?: boolean
  more?: boolean
  iterations?: number
  keepChunks?: string[]
}

export function optimizeZopfliPngSync(input: Buffer, options?: Options) : Buffer;

export function optimizeZopfliPng(input: Buffer, options?: Options) : Promise<Buffer>;
