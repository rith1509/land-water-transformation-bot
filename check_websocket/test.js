const socket = new WebSocket('ws');
socket.onopen =() => {
    console.log('connected');
    socket.send('hello');
};
socket.onmessage = (event) => {
    console.log('Received', event.data);
};