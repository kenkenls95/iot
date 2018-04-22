var mysql = require('mysql');
var mqtt = require('mqtt');
// var express = require('express');
// var app = express();

// ===============Setup MQTT Broker==============
const client = mqtt.connect("mqtt://m14.cloudmqtt.com", {
    username: "maukmmii",
    password: "nsSM0k2eJ2z3",
    port: 15712,
    clientId: "WebUI"
})
client.on("connect", () => {
    client.subscribe("Topic")
    client.publish("Client-parse");
    console.log("connected!")
})
client.on("error", (e) => {
    console.log(e)
})
client.on("close", (e) => {
    client.reconnect()
})
client.on("message", (topic, message) => {
    switch(topic) {
        case "Topic":  addSql(message); break;
    }
})

//================Setup Mysql Connection================

var con = mysql.createConnection({
  host: "localhost",
  user: "root",
  password: "",
  database: "mydb"
});

const addSql = (data) => {
  try{
  var value = JSON.parse(data)
  var humidity = value.humidity
  var temperature = value.temperature
  var soilmoisture = value.soilmoisture
  var led = value.led
  // console.log(led)
  var led1 = led[0]
  var led2 = led[1]
  var led3 = led[2]
  var led4 = led[3]
  var isSuccess = value.success

  addHumidity(humidity,isSuccess)
  addTemperature(temperature,isSuccess)
  addSoilmoisture(soilmoisture)
  addLed(led1,led2,led3,led4)
  }catch (error) {
        console.log(error)
    }
}

const addHumidity = (humidity,isSuccess) => {
  con.connect(function(err) {
  var now = new Date()
  var date = now.getFullYear()+"/"+(now.getMonth()+1)+"/"+now.getDate()+" "+now.getHours()+":"+now.getMinutes()+":"+now.getSeconds();
  console.log("Connected!");
  var sql = "INSERT INTO `tbl_humidity` (`id`, `value`, `date`,`status`) VALUES (NULL, '"+humidity+"', '"+date+"','"+isSuccess+"')";
  con.query(sql, function (err, result) {
    console.log("1 record humidity inserted");
  });
});
}

const addTemperature = (temperature,isSuccess) => {
  con.connect(function(err) {
  var now = new Date()
  var date = now.getFullYear()+"/"+(now.getMonth()+1)+"/"+now.getDate()+" "+now.getHours()+":"+now.getMinutes()+":"+now.getSeconds();
  console.log("Connected!");
  var sql = "INSERT INTO `tbl_temperature` (`id`, `value`, `date`,`status`) VALUES (NULL, '"+temperature+"', '"+date+"','"+isSuccess+"')";
  con.query(sql, function (err, result) {    
    console.log("1 record temperature inserted");
  });
});
}

const addSoilmoisture = (soilmoisture) => {
  con.connect(function(err) {
  var now = new Date()
  var date = now.getFullYear()+"/"+(now.getMonth()+1)+"/"+now.getDate()+" "+now.getHours()+":"+now.getMinutes()+":"+now.getSeconds();
  console.log("Connected!");
  var sql = "INSERT INTO `tbl_soilmoisture` (`id`, `value`, `date`) VALUES (NULL, '"+soilmoisture+"', '"+date+"')";
  con.query(sql, function (err, result) {
    console.log("1 record soilmoisture inserted");
  });
});
}

const addLed = (led1,led2,led3,led4) =>{
  con.connect(function(err) {
  var now = new Date()
  var date = now.getFullYear()+"/"+(now.getMonth()+1)+"/"+now.getDate()+" "+now.getHours()+":"+now.getMinutes()+":"+now.getSeconds();
  console.log("Connected!");
  var sql = "INSERT INTO `tbl_led` (`id`, `led1`, `led2`,`led3`,`led4`, `date`) VALUES (NULL, '"+led1+"','"+led2+"','"+led3+"','"+led4+"', '"+date+"')";
  con.query(sql, function (err, result) {
    console.log("1 record led status inserted");
  });
});
}

// ============Setup API==========================

// var server = app.listen('3000', function(){
//   var host = server.address().address;
//   var port = server.address.port();
//   console.log("Server at 3000");
// })


// app.post('/doLed', function(req,res){

// });