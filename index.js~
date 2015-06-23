var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.use(express.static('public'));

app.get('/', function(req, res){
  var mag = req.query.g_mag;
  if(mag != null) {
	io.sockets.emit('data', req.query.g_mag);    
	res.sendStatus(200);
	res.end();
  }else {
  	res.sendFile(__dirname + '/index_edited.html');
  }
});

http.listen(1111, function(){
  console.log('listening on *:1111');
});

