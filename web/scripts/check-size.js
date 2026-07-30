const fs = require('node:fs');
const path = require('node:path');

const OUTPUT_DIR = path.resolve(__dirname, '..', '..', 'data');
const limits = {
  jsGz: Number(process.env.MAX_JS_GZ_BYTES || 60000),
  cssGz: Number(process.env.MAX_CSS_GZ_BYTES || 20000),
  htmlGz: Number(process.env.MAX_HTML_GZ_BYTES || 10000),
};

function getSize(filePath) {
  return fs.statSync(filePath).size;
}

function findSingleAsset(regex) {
  const files = fs.readdirSync(OUTPUT_DIR).filter((name) => regex.test(name));
  if (files.length === 0) {
    throw new Error(`Missing expected asset for pattern ${regex}`);
  }
  if (files.length > 1) {
    throw new Error(`Expected single asset for pattern ${regex}, got ${files.length}: ${files.join(', ')}`);
  }
  return files[0];
}

function checkAsset(fileName, maxBytes) {
  const absPath = path.join(OUTPUT_DIR, fileName);
  const size = getSize(absPath);
  const ok = size <= maxBytes;
  const status = ok ? 'OK' : 'FAIL';
  console.log(`${status} ${fileName}: ${size} bytes (limit ${maxBytes})`);
  return ok;
}

function run() {
  const jsGz = findSingleAsset(/\.js\.gz$/);
  const cssGz = findSingleAsset(/\.css\.gz$/);
  const htmlGz = 'index.html.gz';

  const results = [
    checkAsset(jsGz, limits.jsGz),
    checkAsset(cssGz, limits.cssGz),
    checkAsset(htmlGz, limits.htmlGz),
  ];

  if (results.every(Boolean)) {
    console.log('Size check passed.');
    return;
  }

  process.exitCode = 1;
  console.error('Size check failed. Adjust output size or update MAX_*_GZ_BYTES budget env vars.');
}

run();
