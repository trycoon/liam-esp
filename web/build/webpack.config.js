
// Define this constant for easier usage
const isProd = process.env.NODE_ENV === 'production';
const { resolve } = require('path');
const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const ExtractTextPlugin = require('extract-text-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const apiMocker = require('webpack-api-mocker');
const CleanWebpackPlugin = require('clean-webpack-plugin');
const CompressionPlugin = require('compression-webpack-plugin');
const StyleLintPlugin = require('stylelint-webpack-plugin');
// eslint-disable-next-line no-unused-vars
const myIp = require('my-local-ip');

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
    filename: '[hash].js',
    //filename: '[name].[hash].js', // can't use this because filename becomes too long, max limit of 32 chars in SPIFFS.
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
            presets: [
              ["@babel/preset-env",
                {
                  "targets": ">5%",
                  "debug": true,
                },
              ],
            ],
          },
        },
      },
      {
        test: /\.css$/,
        use: ExtractTextPlugin.extract({
          fallback: 'style-loader',
          use: 'css-loader',
        }),
      },
      {
        test: /\.scss|\.sass$/,
        use: ExtractTextPlugin.extract({
          fallback: 'style-loader',
          use: ['css-loader', 'sass-loader'],
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
      allowExternal: true,
    }),
    new HtmlWebpackPlugin({
      template: resolve(__dirname, '..', 'src', 'html', 'index.ejs'),
    }),
    new ExtractTextPlugin({
      filename: '[hash].css',
      disable: !isProd,
    }),
    new webpack.optimize.ModuleConcatenationPlugin(), // Scope Hoisting: https://www.codementor.io/drewpowers/high-performance-webpack-config-for-front-end-delivery-90sqic1qa#1-scope-hoisting
    new webpack.optimize.LimitChunkCountPlugin({
      maxChunks: 1,
    }),
    new CopyWebpackPlugin([
      {
        from: resolve(__dirname, '..', 'src', 'html', 'swagger'),
        to: resolve(__dirname, '..', '..', 'data'),
      },
    ]),
    new CompressionPlugin({
      test: /(\.html|\.js|\.css|\.yaml)$/i,
      algorithm: 'gzip',
      threshold: 4096, // Only assets bigger than this size are processed. In bytes.
      compressionOptions: { level: 9 },
      deleteOriginalAssets: isProd,
    }),
    new StyleLintPlugin({
      configFile: resolve(__dirname, '..', '.stylelintrc'),
      context: resolve(__dirname, '..', 'src', 'styles'),
    }),
  ],
};

if (!isProd) {
  config.devServer = {
    contentBase: resolve(__dirname, '..', 'static'),
    hot: true,
    publicPath: '/',
    historyApiFallback: true,
    proxy: {
      '/ws': {
        target: `ws://localhost:8081/websocket`,
        pathRewrite: {'^/ws' : ''},
        ws: true,
        secure: false,
        logLevel: 'debug',
      },
    },
    before(app) {
      // load common mock data
      app.locals.mock = require(resolve(__dirname, '..', 'mock', 'mock-data.js'));
      // setup REST endpoints with mocked data, mock will be available in api-handlers on app.locals.mock.
      apiMocker(app, resolve(__dirname, '..', 'mock', 'api-mocker.js'));
      // setup Websocket with mocked data
      require(resolve(__dirname, '..', 'mock', 'socket-mocker.js'))(8081, app.locals.mock);
    },
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
