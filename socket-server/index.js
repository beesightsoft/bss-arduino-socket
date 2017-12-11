const express = require('express');
const http = require('http');
const url = require('url');
const WebSocket = require('ws');

const app = express();

app.use('/libs', express.static('libs'));

app.get('/', function(req, res){
    res.sendFile(__dirname + '/index.html');
});

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

function heartbeat() {
  this.isAlive = true;
}

const interval = setInterval(function ping() {
  wss.clients.forEach(function each(ws) {
    if (ws.isAlive === false) return ws.terminate();

    ws.isAlive = false;
    ws.ping('', false, true);
});
}, 30000);

function sendMsg(ws, message){
    try {
        var data = {
            msg: message
        }
        var jsonMessage = JSON.stringify(data);
        console.log('Sending message: ', data);
        ws.send(jsonMessage);

    } catch (e) {
        console.error(e);
    }
}

wss.on('connection', function connection(ws, req) {
    const location = url.parse(req.url, true);
    // You might use location.query.access_token to authenticate or share sessions
    // or req.headers.cookie (see http://stackoverflow.com/a/16395220/151312)
    const ip = req.connection.remoteAddress; //const ip = req.headers['x-forwarded-for'];
    console.log('a user connected: ' + ip);
    sendMsg(ws, Date.now());

    ws.on('close', function close() {
        console.log('a user disconnected');
    });

    ws.on('message', function incoming(message) {
        console.log('received:', message);
    });

    sendMsg(ws, "something");

    ws.isAlive = true;
    ws.on('pong', heartbeat);

});

server.listen(82, function listening() {
  console.log('Listening on %d', server.address().port);
});



