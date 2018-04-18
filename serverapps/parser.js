var http = require('http');
var requestfunc = require('request');
http.createServer(function(request,response) {
	let body = [];
  	request.on('error', (err) => {
    		console.error(err);
  	}).on('data', (chunk) => {
    		body.push(chunk);
  	}).on('end', () => {
		var recvd = JSON.parse(body);
		if(recvd.hasOwnProperty('data')) {
			//check for existence of the data key
			let data = JSON.parse(new Buffer(recvd['data'], 'hex'));
			//data should now be a JSON object, format correctly and then POST to influx
			console.log('data a = %s', data['a']);
			requestfunc({
				url: "http://localhost:9100",
				method: "POST",
				json: true,
				body: data,
			}, function (error, res, body){

			});	
		}
		console.log('recieved: %s', recvd['app']);
	});
	
}).listen(8080);
