// 1. Import necessary libraries
const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');

// 2. Initialize Express app and create an HTTP server
const app = express();
const server = http.createServer(app);

// 3. Set up the view engine to use EJS
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));

// 4. Initialize WebSocket server
const wss = new WebSocket.Server({ server });

// 5. Store WebSocket connections
let robotSocket = null; // We'll store the robot's connection here
let controllerSocket = null; // And the controller's connection here

// 6. Handle WebSocket connections
wss.on('connection', (ws) => {
    console.log('A client connected.');

    // Heuristic: The first client to connect is the robot.
    // A more robust method would involve an initial handshake message.
    if (!robotSocket) {
        robotSocket = ws;
        console.log('Robot connected!');

        ws.on('close', () => {
            console.log('Robot disconnected.');
            robotSocket = null; // Clear the stored socket
        });

        // Handle messages from the robot (e.g., sensor data), if any
        ws.on('message', (message) => {
            console.log(`Message from Robot: ${message}`);
            // If robot sends data, you could forward it to the controller
            if (controllerSocket && controllerSocket.readyState === WebSocket.OPEN) {
                controllerSocket.send(`Robot says: ${message}`);
            }
        });

    } else if (!controllerSocket) {
        controllerSocket = ws;
        console.log('Controller connected!');

        ws.on('close', () => {
            console.log('Controller disconnected.');
            controllerSocket = null; // Clear the stored socket
        });

        // Handle messages from the controller
        ws.on('message', (message) => {
            const command = message.toString();
            console.log(`Received command from controller: ${command}`);

            // Forward the command to the robot if it's connected
            if (robotSocket && robotSocket.readyState === WebSocket.OPEN) {
                console.log(`Forwarding command to robot: ${command}`);
                robotSocket.send(command);
            } else {
                console.log('Robot is not connected. Command not sent.');
                // Optionally, send a feedback message to the controller
                ws.send('Error: Robot is not connected.');
            }
        });
    } else {
        // A third client tried to connect
        console.log('A third client tried to connect. Disconnecting.');
        ws.send('Error: A controller is already connected.');
        ws.close();
    }
});

// 7. Serve the main controller UI
app.get('/', (req, res) => {
    res.render('index');
});

// 8. Start the server
const PORT = 3000;
server.listen(PORT, '0.0.0.0', () => {
    console.log(`Server is running.`);
    console.log(`Open http://<YOUR_COMPUTER_IP>:${PORT} on your phone or browser.`);
    console.log('Make sure your ESP32 and computer are on the same Wi-Fi network.');
});
