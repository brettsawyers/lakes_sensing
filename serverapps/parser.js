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
            let influxstr = "airsensors,device=" + recvd['devaddr'] +
                " Temperature=" + data['a'] +",Humidity=" + data['b'] +
                ",CO2=" + data['c'] + ",NO2=" + data['d'] + ",Ozone=" +
                data['e'] + ",CO=" + data['f'] + ",Battery=" + data['g'];
/*
            data['temperature_air'] = data['a'];
            delete data['a'];
            data['humidity_air'] = data['b'];
            delete data['b'];
            data['CO2_air'] = data['c'];
            delete data['c'];
            data['NO2_air'] = data['d'];
            delete data['d'];
            data['Ozone_air'] = data['e'];
            delete data['e'];
            data['CO_air'] = data['f'];
            delete data['f'];
            data['Battery_air'] = data['g'];
            delete data['g'];
*/
			//data should now be a string object, formatted correctly and ready to POST to influx
			console.log('%s',influxstr);
			requestfunc({
				url: "http://localhost:8086/write?db=testdb",
				method: "POST",
				json: true,
				body: influxstr,
			}, function (error, res, body){
                console.log(body);
			});
		}
		console.log('recieved: %s', recvd['app']);
	});

}).listen(8080);
