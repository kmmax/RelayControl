/**
 * This a simple example of the aREST Library interaction with ENC28J60 shield.
 * Written in 2016 by Lex ( http://numidium.ru/ ) under a GPL license.
 */
#include <EtherCard.h>

// Declaring aREST
#include <aREST.h>

// defines
#define ON 1
#define OFF 0
#define TIME_ON 2000  // on delay time

unsigned long off_time = millis();
long previousMillis = 0;

aREST rest = aREST();

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x32 };
static byte myip[] = { 192, 168, 1, 203 };
static byte netmask[] = { 255, 255, 255, 0 };
static byte gwip[] = { 192, 168, 1, 1 };
static byte dnsip[] = { 0, 0, 0, 0 };

// TCP/IP send and receive buffer.
// 350 selected due to aREST output buffer size is the same,
// and for memory economy.
byte Ethernet::buffer[500];  // tcp/ip send and receive buffer
BufferFiller bfill;

int pinRelay1 = 2;
int pinRelay2 = 3;


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  ether.begin(sizeof Ethernet::buffer, mymac, 10);  // GPIO 10 for CS!
  ether.staticSetup(myip, gwip, 0, netmask);

  Serial.println("RelayRestControl:");
  ether.printIp("IP:      ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("Gtw:     ", ether.gwip);
  ether.printIp("Dns:     ", ether.dnsip);

  // rest init:
  rest.set_id("1");
  rest.set_name("Computer");
  rest.function("relay1", relay1);
  rest.function("relay2", relay2);
  rest.function("relay3", relay3);

  // Set up the relays (2, 3 pins):
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);

  // Relays are off by default:
  digitalWrite(pinRelay1, 1);
  digitalWrite(pinRelay2, 1);
}


// Cutom functions for relays
int relay1(String params) {
  Serial.println("relay1 function");
  if (params.length() != 0) {
    int newVal = params.toInt();
    newVal == 0 ? newVal = 1 : newVal = 0;
    Serial.println(newVal);
    digitalWrite(pinRelay1, newVal);
  };
  return digitalRead(pinRelay1) == 0 ? 1 : 0;
}

int relay2(String params) {
  Serial.println("relay2 function");
  if (params.length() != 0) {
    int newVal = params.toInt();
    newVal == 0 ? newVal = 1 : newVal = 0;
    digitalWrite(pinRelay2, newVal);
  }
  return digitalRead(pinRelay2) == 0 ? 1 : 0;
}

int relay3(String params) {
  Serial.println("relay3 function");
  Serial.println(params);
  Serial.println(params.length());
  return 0;
}

void loop() {
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  if (pos) {
    bfill = ether.tcpOffset();
    char *data = (char *)Ethernet::buffer + pos;
    Serial.println("-----");
    Serial.println(data);
    Serial.println("-----");

    if (strncmp("GET /", data, 5) != 0) {
      // Unsupported request method (POST or other).
      bfill.emit_p(PSTR("Unsupported HTTP request."));
    } else {
      // vvv--- Handle REST manually ---vvv
      rest.handle_proto(data);
      char *res = rest.getBuffer();
      Serial.println(res);
      // Send HTTP header:
      bfill.emit_p(
        PSTR("HTTP/1.0 200 OK\r\nContent-Type: application/json\r\nPragma: no-cache\r\n\r\n"));
      // Send result:
      bfill.emit_raw(res, strlen(res));
      rest.resetBuffer();
      rest.reset_status();
      // ^^^--- Handle REST manually ---^^^
    }

    ether.httpServerReply(bfill.position());  // send http response
  }
}