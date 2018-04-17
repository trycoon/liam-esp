const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CleanWebpackPlugin = require('clean-webpack-plugin');

module.exports = {
  context: __dirname + '/web',
  entry: './liam.js',
  plugins: [
    new CleanWebpackPlugin(['data']),
    new HtmlWebpackPlugin({
      template: 'index.html',
      inject: 'body',
    })
  ],
  output: {
    filename: 'liam-esp.js',
    path: path.resolve(__dirname, 'data')
  },
  mode: 'production'
};
