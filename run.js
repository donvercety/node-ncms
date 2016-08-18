var thread = require('./build/Release/ncms'),
	buff   = new Buffer("1234567890");


// simulates tasks running in the event loop
setInterval(function() {
	console.log("..event loop tick..");
}, 1000);

// log the buffer to be sent to C++
console.log("buffer to be sent to C++: [%s]", buff.toString());

// run the C++ module
thread(buff, function(buff) {

	// log the returned "processed" :) buffer
	console.log("Buffer in final callback: " + new Buffer(buff));
});
