const esbuild = require('esbuild');


esbuild.build({
    entryPoints: ['entry.js'],
    bundle: true,
    platform: 'neutral', // neutral keeps node/browser shims out; mathjax-full works
    format: 'iife',
    globalName: 'MathJaxBundle',
    outfile: 'dist/mathjax-lite-bundle.js',
    minify: true,
    sourcemap: false,
    logLevel: 'info'
}).catch(() => process.exit(1));