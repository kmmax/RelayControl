/**
 * This is an example of the interaction with aREST library for Arduino/ESP8266.
 * Written in 2016 by Lex ( http://numidium.ru/ ) under a GPL license.
 */
var http = require('http');

/**
 * Gets URL and parsing it.
 * @param {string} req URL address to request.
 * @param {function} cb Callback (optional).
 *
 * Callback function have to take one object with fields:
 * {
 *   'message': string, // Error message
 *   'error': int,      // If error is happened, this number will be 1 otherwise 0
 *   'value': int,      // 'return_value' field of the JSON response from server.
 * }
 */
var getInfo = function(req, cb) {
  // If callback is specified:
  var ret = function(d) {
    if (typeof(cb) !== 'undefined') {
      cb(d);
    }
  };

  // Typical HTTP GET request.
  http.get(req, (res) => {
    var result = '';

    // Save incoming data chunk by chunk
    res.on('data', (chunk) => {
      result += chunk;
    });

    // At the end of process
    res.on('end', () => {
      // Trying to parse a result
      try {
        var obj = JSON.parse(result);
        ret({
          'error': 0,
          'value': obj.return_value
        });
      } catch (e) {
        // If we unable to parse result
        // (Arduino hangs, wrong IP running another server, et.c.)
        ret({
          'error': 1,
          'message': 'Error while parsing result.'
        });
      }
    });
  }).on('error', (e) => {
    // On connection error
    ret({
      'error': 1,
      'message': 'Connection error.'
    });
  });
};

// Main
getInfo('http://192.168.1.203/relay1', (result) => {
  if (result.error != 0) {
    console.log('Error: ' + result.message);
    return;
  }
  var res = (result.value == 1)?'on':'off';
  console.log('Relay 1 is ' + res);

  if (result.value == 1) {
    // It's on! Let's turn it off now.
    console.log('Turning off.');
    getInfo('http://192.168.1.203/relay1?params=0');
  } else {
    // It's off! Let's turn it on now.
    console.log('Turning on.');
    getInfo('http://192.168.1.203/relay1?params=1');
  }
});