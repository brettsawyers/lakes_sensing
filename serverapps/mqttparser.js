var mqtt = require('mqtt')
var client = mqtt.connect('mqtt://farmbot-lora.zones.eait.uq.edu.au')
var requestfunc = require('request');
client.on('connect', function() {
    client.subscribe('application/5/node/+/rx')
    console.log("connected")
})

client.on('message', function(topic,message) {
    let recvd = JSON.parse(message);
    recvd['data'] = Buffer.from(recvd['data'],'base64').toString("ascii");;
    console.log(recvd['data']);
    let data = JSON.parse(recvd['data']);
    let influxstr = "airsensors,device=" + recvd['deviceName'] +" Temperature=" + data['a'] +",Humidity=" + data['b'] + ",CO2=" + data['c'] + ",NO2=" + data['d'] + ",Ozone=" + data['e'] + ",CO=" + data['f'] + ",Battery=" + data['g'];
    console.log("To influx %s\n", influxstr);
    requestfunc({
        url: "http://localhost:8086/write?db=testdb",
        method: "POST",
        json: false,
        body: influxstr.replace(/\"/g,""),
        }, function (error, res, body){
            console.log(body);
        });

})
