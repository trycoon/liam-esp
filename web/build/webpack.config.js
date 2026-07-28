
// Define this constant for easier usage
const isProd = process.env.NODE_ENV === 'production';
const { resolve } = require('path');
const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const express = require('express');
const CompressionPlugin = require('compression-webpack-plugin');
const StyleLintPlugin = require('stylelint-webpack-plugin');
// eslint-disable-next-line no-unused-vars
const myIp = require('my-local-ip');

const config = {

  mode: isProd ? 'production' : 'development',

  externals: {
    'three': 'THREE',
  },

  entry: {
    // Main entry point of our app
    app: resolve(__dirname, '..', 'src', 'index.js'),
  },

  output: {
    // built files are stored in "data"-directory in the root of the project.
    path: resolve(__dirname, '..', '..', 'data'),
    clean: true,

    // In our case we serve assets directly from root
    publicPath: '/',

    // We add hash to filename to avoid caching issues
    filename: '[fullhash].js',
    //filename: '[name].[fullhash].js', // can't use this because filename becomes too long, max limit of 32 chars in SPIFFS.
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
        test: /\.css$/,
        use: [
          isProd ? MiniCssExtractPlugin.loader : 'style-loader',
          'css-loader',
        ],
      },
      {
        test: /\.scss|\.sass$/,
        use: [
          isProd ? MiniCssExtractPlugin.loader : 'style-loader',
          'css-loader',
          'sass-loader',
        ],
      },
      {
        test: /\.(png|jpe?g|gif|svg|webp)(\?.*)?$/,
        loader: 'url-loader',
        options: {
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
    new HtmlWebpackPlugin({
      template: resolve(__dirname, '..', 'src', 'html', 'index.ejs'),
    }),
    new MiniCssExtractPlugin({
      filename: '[fullhash].css',
    }),
    new webpack.optimize.ModuleConcatenationPlugin(), // Scope Hoisting: https://www.codementor.io/drewpowers/high-performance-webpack-config-for-front-end-delivery-90sqic1qa#1-scope-hoisting
    new webpack.optimize.LimitChunkCountPlugin({
      maxChunks: 1,
    }),
    new CopyWebpackPlugin({
      patterns: [
        {
          from: resolve(__dirname, '..', 'src', 'html', 'swagger'),
          to: resolve(__dirname, '..', '..', 'data'),
        },
        {
          from: resolve(__dirname, '..', 'src', 'favicons'),
          to: resolve(__dirname, '..', '..', 'data'),
        },
      ],
    }),
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
    static: [
      {
        directory: resolve(__dirname, '..', 'static'),
      },
      {
        directory: resolve(__dirname, '..', 'src', 'resources'),
        publicPath: '/resources',
      },
    ],
    client: {
      webSocketURL: {
        pathname: '/hmr-ws',
      },
    },
    webSocketServer: {
      type: 'ws',
      options: {
        path: '/hmr-ws',
      },
    },
    hot: true,
    historyApiFallback: true,
    proxy: [
      {
        context: ['/ws'],
        target: 'ws://localhost:8081/websocket',
        pathRewrite: { '^/ws': '' },
        ws: true,
        secure: false,
      },
    ],
    setupMiddlewares(middlewares, devServer) {
      const app = devServer.app;

      // load common mock data
      app.locals.mock = require(resolve(__dirname, '..', 'mock', 'mock-data.js'));
      app.use(express.json());
      app.use(express.urlencoded({ extended: true }));

      // setup REST endpoints with mocked data, mock will be available in handlers on app.locals.mock.
      const apiHandlers = require(resolve(__dirname, '..', 'mock', 'api-mocker.js'));
      Object.entries(apiHandlers).forEach(([signature, handler]) => {
        const [method, ...pathParts] = signature.split(' ');
        const routePath = pathParts.join(' ');
        const register = app[method.toLowerCase()];
        if (typeof register === 'function') {
          register.call(app, routePath, handler);
        }
      });

      // setup Websocket with mocked data
      require(resolve(__dirname, '..', 'mock', 'socket-mocker.js'))(8081, app.locals.mock);

      return middlewares;
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
