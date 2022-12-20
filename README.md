# chirpstack-relay
Subscribe to Chirpstack MQTT and forward messages received via UDP message to a server. Messages use the Cayenne LPP protocol. There is a Windows service that receives these messages and saves them to a database.

## MQTT_Relay

This is a C++ program that subscribes to the Chirpstack MQTT broker on a Raspberry Pi 3B. When a LoRa message is received the program decodes the raw base64 encoded data then decodes the Cayenne LPP message. The decoded data is assembled into a JSON message along with the LoRa RSSI, SF, SNR and time the message was received. This JSON message is then sent as a UDP message to a server.

There are two versions of this - lora_relay.cpp and temperature_relay.cpp. The lora_relay simply forwards raw "Hello World" type data and temperature_relay also decodes the message as a Cayenne LPP message.

## UDP_Receiver

This is a C# Windows service that listens for a UDP message on a specified port and saved the received JSON data in a SQL Server database.

## Libraries Used

For JSON encoding and decoding I have used nlohmann/json (https://github.com/nlohmann/json)
To decode Cayenne LPP I modified gizmocuz/CayenneLPP-Decoder (https://github.com/gizmocuz/CayenneLPP-Decoder). For my purposes I simplified this and changed the JSON handling to the nlohmann library.
To decode Base64 I used taomykaira Base64.h (https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594)

## Cloning

```sh
git clone --recurse-submodules https://github.com/m-nahirny/chirpstack-relay.git 
```

## Building

There are text files that contain the compile/link commands for lora_relay and temperature_relay. UDP_Receiver is a Visual Studio C# project.

