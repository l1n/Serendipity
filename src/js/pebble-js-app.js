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

function getBuildings(callback) {
    var req = new XMLHttpRequest();
    req.open('GET', 'http://api.umd.io/v0/map/buildings');
    req.onload = function (e) {
        var buildings = JSON.parse(req.responseText);
        var buildingsMash = {};
        var buildingsProcessed = 0;
        buildings.forEach(function(b) {
            buildingsMash[b.number] = b;
            if (++buildingsProcessed==buildings.length) callback(buildingsMash);
        });
    };
    req.send(null);
};


function getSections(predicate, callback) {
    var req = new XMLHttpRequest();
    req.open('GET', 'http://api.umd.io/v0/courses?grading_method=Audit');
    req.onload = function (e) {
        var courses = JSON.parse(req.responseText);
        var nresponded = 0;
        var allsections = [];
        courses.forEach(function (course) {
            if (course.sections)
            course.sections.forEach(function(section) {
            var req = new XMLHttpRequest();
            req.open('GET', 'http://api.umd.io/v0/courses/sections/' + section);
            req.onload = function (e) {
                var sections = JSON.parse(req.responseText);
                if (sections.error_code === 400) console.log(JSON.stringify(course));
                else sections.forEach(function (section) {
                    var sectionPredicated = predicate(section);
                    if (sectionPredicated != null) allsections.push(sectionPredicated);
                    if (++nresponded === courses.length) callback(allsections);
            });
            };
            req.send(null);
            });
        });
    };
    req.send(null);
};

Pebble.addEventListener("ready", function (e) {
    getBuildings(function(buildings) {
        console.log('BUILDINGS COMPLETE');
        getSections(function(section) {
            section.meetings.building = buildings[section.meetings.building];
            var days = ['Su','M','Tu','W','Th','F','Sa'];
            return (section.meetings[0].classtype == "Lecture" && section.days.indexOf(days[new Date().getDay()]) + 1) ? (section) : (null);
        }, function (sections) {
            var destination = sections[Math.floor(Math.random() * sections.length)].meetings[0];
            console.log('INFO: '+destination);
            navigator.geolocation.getCurrentPosition(function() {
                var req = new XMLHttpRequest();
                req.open('GET', 'https://maps.googleapis.com/maps/api/directions/json?origin='+pos.coords.latitude+','+pos.coords.longitude+'&destination='+destination.lat+','+destination.lng+'&mode=walking&key=AIzaSyCxTPa24kl86coKhxb_h7xdyLgq9dCKmuw');
                req.onload = function (e) {
                    var json = JSON.parse(req.responseText);
                    var steps = json.route[0].legs[0];
                    var step = 0;
                    (function dostuff() {
                        navigator.geolocation.getCurrentPosition(function() {
                            if (distance(pos.coords.latitude, pos.coords.longitude, steps[step].end_location.lat, steps[step].end_location.lng) < 0.002 && ++step===steps.length) return;
                            var message = floatMsg(bearing(pos.coords.latitude, pos.coords.longitude, steps[step].end_location.lat, steps[step].end_location.lng));
                            var transactionId = Pebble.sendAppMessage( {0:message});
                        }, dostuff, locationOptions);
                    })();
                };
                req.send(null);
            });
            req.send(null);
        });
    });
});
Pebble.addEventListener('appmessage', function(e) {
    // console.log('Received message: ' + JSON.stringify(e.payload));
});
