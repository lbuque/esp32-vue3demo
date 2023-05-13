import { fileURLToPath, URL } from 'node:url'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import viteCompression from "vite-plugin-compression"


// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    // viteCompression({
    //   threshold: 102400,
    //   deleteOriginFile: true
    // })
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    }
  },
  build: {
    outDir: "../data/www",
    reportCompressedSize: true,
    emptyOutDir: true
  }
})
