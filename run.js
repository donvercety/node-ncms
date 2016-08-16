var test = require('./build/Release/test');


setInterval(function() {
	console.log("go");
}, 1000);

test(4, function(square) {
	console.log(square);
});
