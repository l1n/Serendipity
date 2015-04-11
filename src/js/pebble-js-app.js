Pebble.addEventListener("ready",
  function(e) {
    var req = new XMLHttpRequest();
    req.open('GET', 'https://maps.googleapis.com/maps/api/directions/json?origin=here&destination=NS%20Norfolk&mode=walking&avoid=highways&arrival_time='+(Date.now()+600000)+'&key=AIzaSyCxTPa24kl86coKhxb_h7xdyLgq9dCKmuw');
    req.onload = function(e) {
    // console.log('ready');
    var i = 0;
    Pebble.sendAppMessage({0: ""+i}, function resend() {
      // console.log('XMLHttpRequest()');
      req.open('GET', 'http://server.adrusi.com:25565', true);
      req.onload = function(e) {
        // console.log('req.onload');
        if (req.readyState == 4 && req.status == 200) {
          if(req.status == 200) {
            // console.log(req.responseText);
            Pebble.sendAppMessage({0: ""+req.responseText}, setTimeout(resend, 1000), null);
          } else { console.log('Error'); }
        }
      };
      req.send(null);
    }, null);
    };
    req.send(null);
  }
);
