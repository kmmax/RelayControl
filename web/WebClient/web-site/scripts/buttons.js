document.getElementById("r1_on").onclick = function() {
    this.textContent = (this.textContent != "ON") ? "ON": "OFF";
}

function onClick(device, state) {
var address = "192.168.1.203";
var request = new XMLHttpRequest();
request.open("GET", `http://${address}/${device}?params=${state}`, false);
request.send();
}