
// Define this constant for easier usage
const isProd = process.env.NODE_ENV === 'production';
const { resolve } = require('path');
const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const ExtractTextPlugin = require('extract-text-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const apiMocker = require('webpack-api-mocker');
const CleanWebpackPlugin = require('clean-webpack-plugin')

const config = {

  mode: isProd ? 'production' : 'development',

  entry: {
    // Main entry point of our app
    app: resolve(__dirname, '..', 'src', 'index.js'),
  },

  output: {
    // built files are stored in "data"-directory in the root of the project.
    path: resolve(__dirname, '..', '..', 'data'),

    // In our case we serve assets directly from root
    publicPath: '/',

    // We add hash to filename to avoid caching issues
    filename: '[name].[hash].js',
  },

  resolve: {
    extensions: ['*', '.js'],
    modules: [
      resolve(__dirname, '..', 'node_modules'),
    ],
  },

  module: {
    rules: [
      {
        test: /\.js$/,
        exclude: /node_modules/,
        use: {
          loader: 'babel-loader',
          options: {
            presets: ['env']
          }
        },
      },
      {
        test: /\.css$/,
        use: ExtractTextPlugin.extract({
          fallback: 'style-loader',
          use: 'css-loader?minimize=true',
        }),
      },
      {
        test: /\.scss|\.sass$/,
        use: ExtractTextPlugin.extract({
          fallback: 'style-loader',
          use: ['css-loader?minimize=true', 'sass-loader'],
        }),
      },
      {
        test: /\.(png|jpe?g|gif|svg)(\?.*)?$/,
        loader: 'url-loader',
        query: {
          limit: 10000,
          name: 'images/[name].[hash:7].[ext]',
        },
      },
      {
        test: /\.(woff2?|eot|ttf|otf)(\?.*)?$/,
        loader: 'url-loader',
        options: {
          limit: 1000,
          name: 'fonts/[name].[hash:7].[ext]',
        },
      },
    ],
  },

  plugins: [
    new CleanWebpackPlugin(resolve(__dirname, '..', '..', 'data'), {
      allowExternal: true
    }),
    new HtmlWebpackPlugin({
      template: resolve(__dirname, '..', 'src', 'html', 'index.ejs'),
    }),
    new ExtractTextPlugin({
      filename: 'style.[hash].css',
      disable: !isProd,
    }),
    new webpack.optimize.ModuleConcatenationPlugin(), // Scope Hoisting: https://www.codementor.io/drewpowers/high-performance-webpack-config-for-front-end-delivery-90sqic1qa#1-scope-hoisting
    new webpack.optimize.LimitChunkCountPlugin({
      maxChunks: 1
    }),
    new CopyWebpackPlugin([
      {
        from: resolve(__dirname, '..', 'src', 'html', 'swagger'),
        to: resolve(__dirname, '..', '..', 'data')
      }
    ]),
  ],
};

if (!isProd) {
  config.devServer = {
    contentBase: resolve(__dirname, '..', 'static'),
    hot: true,
    publicPath: '/',
    historyApiFallback: true,
    before(app){
      apiMocker(app, resolve(__dirname, '..', 'mocker.js'));
    }
  };
} else {
  // Add buildcheck if any resources grow too big, the embeded system has very limited space.
  config.performance = {
    hints: 'warning',
    maxEntrypointSize: 50000,
    maxAssetSize: 60000,
  };
}

module.exports = config;
