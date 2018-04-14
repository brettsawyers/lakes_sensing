function lookuptype(type) {
  var typestr;
  switch(type){
    case "a":
      typestr = "Air Temp";
      break;
    case "b":
      typestr = "Humidity";
      break;
    case "c":
      typestr = "NO2";
      break;
    case "d":
      typestr = "O3";
      break;
    case "e":
      typestr = "CO";
      break;
    case "f":
      typestr = "CO2";
      break;
    case "g":
      typestr = "Air Battery";
      break;
    case "h":
      typestr = "Water Temp";
      break;
    case "i":
      typestr = "Conductivity";
      break;
    case "j":
      typestr = "Specific Conductance";
      break;
    case "k":
      typestr = "Salinity";
      break;
    case "l":
      typestr = "pH mV";
      break;
    case "m":
      typestr = "pH range";
      break;
    case "n":
      typestr = "ORP";
      break;
    case "o":
      typestr = "depth";
      break;
    case "p":
      typestr = "ODO %Sat";
      break;
    default:
      typestr = "Unknown type";
      break;
  }
  return typestr;
}

function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var text = String.fromCharCode.apply(null, bytes);
  var values = text.split(",");
  var recvd_dec = {};
  var measurements_dec = [];
  recvd_dec.measurements = measurements_dec;
  //values == [identifier:num, identifier:num.....]
  for(count=0; count < values.length; count++) {
    var datarecvd = values[count].split(":");
    var type = datarecvd[0];
    var measurement = datarecvd[1];
    //convert type to its respective string
    var typestr = lookuptype(type);
    var measurement_dec = {
       "type": typestr,
      "value":measurement
    }
    recvd_dec.measurements.push(measurement_dec);
    }
  return recvd_dec;
  }
