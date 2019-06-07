
module.exports = (port, mock) => {
  let http = require('http');
  let sockjs = require('sockjs');
  let clients = new Map();

  let wsServerer = sockjs.createServer({ sockjs_url: 'http://cdn.jsdelivr.net/sockjs/1.0.1/sockjs.min.js' });
  wsServerer.on('connection', (conn) => {
    clients.set(conn.id, conn);
    console.log(`Websocket client connected, id: ${conn.id}`);

    conn.on('data', (message) => {
      console.log('websocket got:' + message);
      let json = JSON.parse(message);

      if (json.type === 'forward') {
        mock.forward(json.payload.speed, json.payload.turnrate, json.payload.smooth);
      } else if (json.type === 'backward') {
        mock.backward(json.payload.speed, json.payload.turnrate, json.payload.smooth);
      }

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
        payload: mock.getCurrentState(),
      }));
    });

    mock.advanceSequence();
  }, 2000);
};


