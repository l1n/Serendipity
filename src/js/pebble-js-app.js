function floatMsg(x) {
  x *= 1000000;
  x = Math.round(x);
  var mask = 0xFF;
  var array = new Array(4);
  array[0] = x & mask;
  x >>>= 8;
  array[1] = x & mask;
  x >>>= 8;
  array[2] = x & mask;
  x >>>= 8;
  array[3] = x & mask;
  return array;
}

function deg2rad(x) {
  return x / 180 * Math.PI;
}

function rad2deg(x) {
  return x / Math.PI * 180;
}

function bearing(lat1, lon1, lat2, lon2) {
  var R = 6371000; // metres
  var φ1 = deg2rad(lat1);
  var φ2 = deg2rad(lat1);
  var λ1 = deg2rad(lon1);
  var λ2 = deg2rad(lon2);

  var y = Math.sin(λ2-λ1) * Math.cos(φ2);
  var x = Math.cos(φ1)*Math.sin(φ2) -
          Math.sin(φ1)*Math.cos(φ2)*Math.cos(λ2-λ1);
  var bearing = Math.atan2(y, x);
  return (bearing + 2 * Math.PI) % (2 * Math.PI);
}

Pebble.addEventListener("ready",
  function dostuff(e) {
    var message = floatMsg(bearing(38.987986, -76.946594, 38.992272, -76.949995));
    var transactionId = Pebble.sendAppMessage( {0:message},
      function(e) {
      },
      function(e) {
      }
    );
    setTimeout(dostuff, 1000);
    // var req = new XMLHttpRequest();
    // req.open('GET', 'https://maps.googleapis.com/maps/api/directions/json?origin=here&destination=Redmond&mode=walking&avoid=highways&arrival_time='+(Date.now()+600000)+'&key=AIzaSyCxTPa24kl86coKhxb_h7xdyLgq9dCKmuw');
    // req.onload = function (e) {
    //   // console.log('ready');
    //   var i = 0;
    //   var message = int32ByteArray(1337).concat(int32ByteArray(-69));
    //   Pebble.sendAppMessage({0: message}, function resend() {
    //     // console.log('XMLHttpRequest()');
    //     req.open('GET', 'http://server.adrusi.com:25565', true);
    //     req.onload = function (e) {
    //       // console.log('req.onload');
    //       if (req.readyState == 4 && req.status == 200) {
    //         if (req.status == 200) {
    //           // console.log(req.responseText);
    //           Pebble.sendAppMessage({0: message}, setTimeout(resend, 1000), null);
    //         } else { console.log('Error'); }
    //       }
    //     };
    //     req.send(null);
    //   }, null);
    // };
    // req.send(null);
  }
);

Pebble.addEventListener('appmessage', function(e) {
  // console.log('Received message: ' + JSON.stringify(e.payload));
});
