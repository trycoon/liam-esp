const { resolve } = require('path'),
      data = require(resolve(__dirname, 'mock-data.js'));

module.exports = (port) => {
  let http = require('http');
  let sockjs = require('sockjs');
  let clients = new Map();

  let wsServerer = sockjs.createServer({ sockjs_url: 'http://cdn.jsdelivr.net/sockjs/1.0.1/sockjs.min.js' });
  wsServerer.on('connection', (conn) => {
    clients.set(conn.id, conn);
    console.log(`Websocket client connected, id: ${conn.id}`);

    conn.on('data', (message) => {
      console.log('websocket got:' + message);
    });
    conn.on('close', () => {
      console.log(`Websocket client disconnected, id: ${conn.id}`);
      clients.delete(conn.id);
    });
  });
  
  let httpServer = http.createServer();
  wsServerer.installHandlers(httpServer);
  httpServer.listen(port, '0.0.0.0');

  setInterval(() => {
    clients.forEach((client) => {
      client.write(JSON.stringify({
        type: "status",
        payload: data.getCurrentState(),
      }));
    });
  }, 2000);
};


