
module.exports = (port, mock) => {
  let http = require('http');
  let WebSocket = require('ws');
  let clients = new Map();

  let httpServer = http.createServer();
  let wsServer = new WebSocket.Server({
    server: httpServer,
    path: '/websocket',
  });

  wsServer.on('connection', (conn) => {
    clients.set(conn, conn);
    console.log('Websocket client connected');

    conn.on('error', (error) => {
      console.warn(`Websocket client error: ${error.message}`);
      clients.delete(conn);
    });

    conn.on('message', (message) => {
      console.log('websocket got:' + message);
      let json = JSON.parse(message.toString());

      if (json.type === 'forward') {
        mock.forward(json.payload.speed, json.payload.turnrate, json.payload.smooth);
      } else if (json.type === 'backward') {
        mock.backward(json.payload.speed, json.payload.turnrate, json.payload.smooth);
      }

    });
    conn.on('close', function() {
      console.log('Websocket client disconnected');
      clients.delete(conn);
    });
  });

  httpServer.listen(port, '0.0.0.0');

  setInterval(() => {
    clients.forEach((client) => {
      client.send(JSON.stringify({
        type: "status",
        payload: mock.getCurrentState(),
      }));
    });

    mock.advanceSequence();
  }, 2000);
};


