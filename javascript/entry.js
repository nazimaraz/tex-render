import { mathjax } from 'mathjax-full/js/mathjax.js';
import { TeX } from 'mathjax-full/js/input/tex.js';
import { SVG } from 'mathjax-full/js/output/svg.js';
import { liteAdaptor } from 'mathjax-full/js/adaptors/liteAdaptor.js';
import { RegisterHTMLHandler } from 'mathjax-full/js/handlers/html.js';


// create the lite adaptor (no DOM required)
const adaptor = liteAdaptor();
RegisterHTMLHandler(adaptor);


// TeX input with the packages you requested: base + ams
const tex = new TeX({ packages: ['base', 'ams'] });
const svg = new SVG({ fontCache: 'none' });
const html = mathjax.document('', { InputJax: tex, OutputJax: svg });


function _minifyWhitespace(str) {
// convert all consecutive whitespace including newlines into single space
    return str.replace(/\s+/g, ' ').trim();
}


function latexToSVG(latex, display = false) {
// convert; html.convert returns an adaptor node
    const node = html.convert(latex, { display });
// adaptor.outerHTML(node) returns a string which *may* include a wrapper
    const out = adaptor.outerHTML(node);
// extract the first <svg ...>...</svg> occurrence
    const match = out.match(/<svg[\s\S]*?<\/svg>/);
    if (match) return _minifyWhitespace(match[0]);
// fallback: minify whole output
    return _minifyWhitespace(out);
}


// expose to globalThis (QuickJS will see this)
globalThis.latexToSVG = latexToSVG;


// also export as default for bundlers
export default latexToSVG;