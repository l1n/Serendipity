'use strict';
function floatMsg(x) {
  x *= 1000000;
  x = Math.round(x);
  var mask = 0xFF, array = new Array(4);
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
  var φ1 = deg2rad(lat1);
  var φ2 = deg2rad(lat2);
  var λ1 = deg2rad(lon1);
  var λ2 = deg2rad(lon2);

  var y = Math.sin(λ2-λ1) * Math.cos(φ2);
  var x = Math.cos(φ1)*Math.sin(φ2) -
    Math.sin(φ1)*Math.cos(φ2)*Math.cos(λ2-λ1);
  var bearing = Math.atan2(y, x);
  return (bearing + 2 * Math.PI) % (2 * Math.PI);
}

function distance(lat1, lon1, lat2, lon2) {
  var R = 6371; // km
  var dLat = deg2rad(lat2-lat1);
  var dLon = deg2rad(lon2-lon1);
  var a = Math.sin(dLat/2) * Math.sin(dLat/2) +
    Math.cos(deg2rad(lat1)) * Math.cos(deg2rad(lat2)) * 
    Math.sin(dLon/2) * Math.sin(dLon/2); 
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a)); 
  var d = R * c;
}

var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 10000, 
  timeout: 10000
};

function getSections(callback) {
  getCourses(function (courses) {
    var sectionRes = [];
    var totalSections = 0;
    var sectionsHandled = 0;
    courses.forEach(function (course, offset) {
      totalSections += course.sections.length;
      course.sections.forEach(function (section) {
        umdApiCall(
          '/v0/courses/sections/'+section,
          function (sectionInfo) {
            sectionRes.push(sectionInfo);
            if (++sectionsHandled == totalSections) callback(sectionRes);
          }, function (err) {
            if (++sectionsHandled == totalSections) callback(sectionRes);
          });
      });
    });
  });
};

function getCourses(callback) {
  umdApiCall('/v0/courses?grading_method=Audit&dept_id=CMSC', function (res) {
    var coursesProcessed = 0;
    res.forEach(function (element, offset) {
      umdApiCall('/v0/courses/'+element.course_id, function(courseInfo) {
        res[offset] = courseInfo;
        if (++coursesProcessed === res.length) callback(res);
      });
    });
  });
};

function eachCourse(callback) {
  umdApiCall('/v0/courses?grading_method=Audit', function (res) {
    var coursesProcessed = 0;
    res.forEach(function (element) {
      umdApiCall('/v0/courses/'+element.course_id, callback);
    });
  });
}

function coursesSections(course, buildings, callback) {
  course.sections.forEach(function (section) {
    umdApiCall(
      '/v0/courses/sections/'+section,
      function (sectionInfo) {
        sectionInfo.course = course;
        sectionInfo.meetings.forEach(function (meeting) {
          meeting.building = buildings[meeting.building];
        });
        callback(sectionInfo);
      });
  });
}

function eachSection(buildings, callback) {
  eachCourse(function (course) {
    coursesSections(course, buildings, callback);
  });
}

function getBuildings(callback) {
  umdApiCall('/v0/map/buildings', function (buildings) {
    var buildingsProcessed = 0;
    var hashRes = {};
    buildings.forEach(function (element) {
      hashRes[element.code] = element;
      if (++buildingsProcessed === buildings.length) callback(hashRes);
    });
  });
};

function umdApiCall(path, callback, error) {
  var req = new XMLHttpRequest();
  req.open('GET', 'http://api.umd.io'+path);
  req.onload = function (e) {
    var response = JSON.parse(req.responseText);
    callback(response);
  };
  function logerror (e) {
    console.log("API Error " + path + " " + e);
    if (error) error(e);
  }
  req.addEventListener("error", logerror, false);
  req.addEventListener("abort", logerror, false);
  req.send();
};

Pebble.addEventListener("ready", function (e) {
  navigator.geolocation.getCurrentPosition(function(pos) {
    var req = new XMLHttpRequest();
    req.open('GET', 'http://api.yelp.com/v2/search?limit=1&sort=1&ll='+pos.coords.latitude+','+pos.coords.longitude);
    req.onload = function (res) {
      console.log(res.responseText);
      var res = JSON.parse(res.responseText);
      var req = new XMLHttpRequest();
      var qstr = 'https://maps.googleapis.com/maps/api/directions/json?origin='+pos.coords.latitude+','+pos.coords.longitude+'&destination='+res.businesses[0].location.address[0]+'&mode=walking&key=AIzaSyCxTPa24kl86coKhxb_h7xdyLgq9dCKmuw';
      req.open('GET', qstr);
      console.log(qstr);
      req.onload = function (e) {
        var json = JSON.parse(req.responseText);
        var steps = json.routes[0].legs[0].steps;
        var step = 0;
        (function dostuff() {
          navigator.geolocation.getCurrentPosition(function() {
            if (distance(pos.coords.latitude+3.58, pos.coords.longitude, steps[step].end_location.lat, steps[step].end_location.lng) < 0.002 && ++step===steps.length) return Pebble.sendAppMessage({1:"Arrived at destination"});
            var message = floatMsg(bearing(pos.coords.latitude+3.58, pos.coords.longitude, steps[step].end_location.lat, steps[step].end_location.lng));
            var transactionId = Pebble.sendAppMessage({0:message});
          }, dostuff, locationOptions);
        })();
      };
      req.send();
    };
    req.send();
  });
});
Pebble.addEventListener('appmessage', function(e) {
  // console.log('Received message: ' + JSON.stringify(e.payload));
  Pebble.sendAppMessage({2:"Hell"});
});
