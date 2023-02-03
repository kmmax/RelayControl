"use strict";


function relayState(device, lamp, state=undefined) {
    var address = "192.168.1.203";

    const url_set = `http://${address}/${device}?params=${state}`;
    const url_get = `http://${address}/${device}`;

    // Acsessible values: 0 or 1
    let url = url_get;
    if (1 == state || 0 == state) {
        url = url_set;
    }
    console.log(url);

    // Request
    fetch(url, {
        mode: 'cors' 
    })
    .then((response) => {
        return response.json();
    })
    .then((data) => {
        console.log(data);
        let authors = data;

        console.log(`State: ${authors.return_value}`)
        console.log(lamp);
        if (1 == authors.return_value) {
            document.getElementById(lamp).textContent = "ON";
        }
        else {
            document.getElementById(lamp).textContent = "OFF";
        }
    })
    .catch(function(error) {
        console.log("--- error ---");
        console.log(error);
    });
}

document.addEventListener("DOMContentLoaded", () => {
    relayState('relay1', 'lamp1');
    relayState('relay2', 'lamp2');
    relayState('relay3', 'lamp3');
    relayState('relay4', 'lamp4');
})