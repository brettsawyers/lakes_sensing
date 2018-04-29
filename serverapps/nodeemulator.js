var sys = require('util');
var exec = require('child_process').exec;
var datastr = "\"data\":\"7b2261223a3130302c2262223a3230302c2263223a31322c2264223a302e3231322c2265223a3135362c2266223a343934322c2267223a32377d\"";
var devaddrstr = "\"devaddr\":";
var appstr = "curl -X POST -d '{\"app\":\"sensortest\"";
function puts(error, stdout, stderr) {sys.puts(stdout)}
var command = appstr + "," + datastr + "," + devaddrstr + "\"AB0123\"}'" + " http://localhost:8080";
function curlcommand (){
    exec(command,function(err, stdout, stderr){
    console.log(stdout);
    console.log(command);
    });
}
setInterval(curlcommand, 10000);
