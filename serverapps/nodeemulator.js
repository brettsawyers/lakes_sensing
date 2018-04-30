var sys = require('util');
var exec = require('child_process').exec;
var datastr = ["\"data\":\"7b2261223a3130302c2262223a3230302c2263223a31322c2264223a302e3231322c2265223a3135362c2266223a343934322c2267223a32377d\"", "\"data\":\"7b2261223a38372c2262223a3139312c2263223a31352c2264223a302e38322c2265223a3137322e34352c2266223a353132342c2267223a33327d\"", "\"data\":\"7b2261223a39322c2262223a3137322c2263223a31392c2264223a302e3637332c2265223a3135322e36372c2266223a343930302c2267223a33377d\"", "\"data\":\"7b2261223a3130382c2262223a3230352c2263223a33342c2264223a312e3033322c2265223a3133322e352c2266223a343736352e39332c2267223a39397d\""];
var devaddrstr = "\"devaddr\":";
var appstr = "curl -X POST -d '{\"app\":\"sensortest\"";
function puts(error, stdout, stderr) {sys.puts(stdout)}
function deviceAB0123(){
    let command = appstr + "," + datastr[Math.floor(Math.random() * 3)] + "," + devaddrstr + "\"AB0123\"}'" + " http://localhost:8080";
    exec(command,function(err, stdout, stderr){
    console.log(stdout);
    console.log(command);
    });
}

function deviceCD1042(){
    let command = appstr + "," + datastr[Math.floor(Math.random() * 3)] + "," + devaddrstr + "\"CD1042\"}'" + " http://localhost:8080";
    exec(command,function(err, stdout, stderr){
    console.log(stdout);
    console.log(command);
    });
}

setInterval(deviceAB0123, 10000);
setInterval(deviceCD1042, 10000);
