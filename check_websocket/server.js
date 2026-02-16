const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const os = require('os');

const app = express();
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Store connected clients and identify them
const clients = {
  robot: null,
  controller: null
};

// Serve the controller UI
app.get('/', (req, res) => {
  res.render('index');
});

// Handle WebSocket connections
wss.on('connection', (ws) => {
  console.log('A client connected.');

  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message);

      // --- Client Identification ---
      if (data.type === 'identify') {
        if (data.id === 'robot') {
          clients.robot = ws;
          console.log('ROBOT identified and connected.');
        } else if (data.id === 'controller') {
          clients.controller = ws;
          console.log('CONTROLLER identified and connected.');
        }
        return;
      }

      // --- Clearer Logging Logic ---
      if (ws === clients.controller) {
        console.log(`Message from Controller: ${message}`);
        if (clients.robot && clients.robot.readyState === WebSocket.OPEN) {
          clients.robot.send(message);
          console.log('--> Forwarded to Robot.');
        } else {
          console.log('--> Robot not connected. Command not sent.');
        }
      } else if (ws === clients.robot) {
        console.log(`Message from Robot: ${message}`);
      }

    } catch (error) {
      console.error('Failed to parse message or invalid message format:', message.toString());
    }
  });

  ws.on('close', () => {
    console.log('A client disconnected.');
    if (ws === clients.robot) {
      clients.robot = null;
      console.log('ROBOT disconnected.');
    } else if (ws === clients.controller) {
      clients.controller = null;
      console.log('CONTROLLER disconnected.');
    }
  });
});

// Find local IP address to display
const networkInterfaces = os.networkInterfaces();
let ipAddress = 'localhost';
for (const netInterface in networkInterfaces) {
  for (const network of networkInterfaces[netInterface]) {
    if (network.family === 'IPv4' && !network.internal) {
      ipAddress = network.address;
      break;
    }
  }
}

// Start the server
const port = 3000;
server.listen(port, () => {
  console.log(`Server is running.`);
  console.log(`Open controller UI at: http://${ipAddress}:${port}`);
});