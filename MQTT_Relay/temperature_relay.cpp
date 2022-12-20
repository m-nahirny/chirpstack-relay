// async_subscribe.cpp
//
// This is a Paho MQTT C++ client, sample application.
//
// This application is an MQTT subscriber using the C++ asynchronous client
// interface, employing callbacks to receive messages and status updates.
//
// The sample demonstrates:
//  - Connecting to an MQTT server/broker.
//  - Subscribing to a topic
//  - Receiving messages through the callback API
//  - Receiving network disconnect updates and attempting manual reconnects.
//  - Using a "clean session" and manually re-subscribing to topics on
//    reconnect.
//

/*******************************************************************************
 * Copyright (c) 2013-2020 Frank Pagliughi <fpagliughi@mindspring.com>
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Frank Pagliughi - initial implementation and documentation
 *******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include <string>
#include "mqtt/async_client.h"
//#include "/home/pi/json/include/nlohmann/json.hpp"
#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>
#include "base64.h"
#include "UDP.h"
#include "CayenneLPP_Dec.h"

const std::string SERVER_ADDRESS("tcp://localhost:1883");
// note that this needs to be unique 
const std::string CLIENT_ID("wisblock2_subcribe");
//const std::string TOPIC("application/2/device/01d6b4124e84f1f1/rx");
const std::string TOPIC("application/3/device/d3afa8e7f553596c/rx");
//const std::string TOPIC("gateway/00e04cfffe534458/event/up");

const int	QOS = 1;
const int	N_RETRY_ATTEMPTS = 5;

using json = nlohmann::json;

/////////////////////////////////////////////////////////////////////////////

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.

class action_listener : public virtual mqtt::iaction_listener
{
	std::string name_;

	void on_failure(const mqtt::token& tok) override {
		std::cout << name_ << " failure";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		std::cout << std::endl;
	}

	void on_success(const mqtt::token& tok) override {
		std::cout << name_ << " success";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		auto top = tok.get_topics();
		if (top && !top->empty())
			std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
		std::cout << std::endl;
	}

public:
	action_listener(const std::string& name) : name_(name) {}
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */
class callback : public virtual mqtt::callback,
					public virtual mqtt::iaction_listener

{
	// Counter for the number of connection retries
	int nretry_;
	// The MQTT client
	mqtt::async_client& cli_;
	// Options to use if we need to reconnect
	mqtt::connect_options& connOpts_;
	// An action listener to display the result of actions.
	action_listener subListener_;

	// This deomonstrates manually reconnecting to the broker by calling
	// connect() again. This is a possibility for an application that keeps
	// a copy of it's original connect_options, or if the app wants to
	// reconnect with different options.
	// Another way this can be done manually, if using the same options, is
	// to just call the async_client::reconnect() method.
	void reconnect() {
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
		try {
			cli_.connect(connOpts_, nullptr, *this);
		}
		catch (const mqtt::exception& exc) {
			std::cerr << "Error: " << exc.what() << std::endl;
			exit(1);
		}
	}

	// Re-connection failure
	void on_failure(const mqtt::token& tok) override {
		std::cout << "Connection attempt failed" << std::endl;
		if (++nretry_ > N_RETRY_ATTEMPTS)
			exit(1);
		reconnect();
	}

	// (Re)connection success
	// Either this or connected() can be used for callbacks.
	void on_success(const mqtt::token& tok) override {}

	// (Re)connection success
	void connected(const std::string& cause) override {
		std::cout << "\nConnection success" << std::endl;
		std::cout << "\nSubscribing to topic '" << TOPIC << "'\n"
			<< "\tfor client " << CLIENT_ID
			<< " using QoS" << QOS << "\n"
			<< "\nPress Q<Enter> to quit\n" << std::endl;

		cli_.subscribe(TOPIC, QOS, nullptr, subListener_);
	}

	// Callback for when the connection is lost.
	// This will initiate the attempt to manually reconnect.
	void connection_lost(const std::string& cause) override {
		std::cout << "\nConnection lost" << std::endl;
		if (!cause.empty())
			std::cout << "\tcause: " << cause << std::endl;

		std::cout << "Reconnecting..." << std::endl;
		nretry_ = 0;
		reconnect();
	}

	// Callback for when a message arrives.
	void message_arrived(mqtt::const_message_ptr msg) override {
		
		json jmsg = "";
		try
		{
			jmsg = json::parse(msg->to_string());
		}
		catch (std::exception& ex)
		{
			std::cout << "\nJSON Parse exception " << ex.what() << "\n";
		}

		try
		{
			if (jmsg["data"] != NULL)
			{
				std::string encoded_data = jmsg["data"];
				std::string decoded_data;
				macaron::Base64* b64 = new macaron::Base64();
				std::cout << "\n" << "encoded data: " << encoded_data << "\n";
				b64->Decode(encoded_data, decoded_data);

				// convert the raw Cayenne LPP data to json
				int n = decoded_data.length();
				printf("Decoded Data size: %d\n", n);
				uint8_t char_array[n+1];
				memcpy(char_array, decoded_data.c_str(), n);
				char_array[n+1] = '\0';
				puts("Decoded data : ");
				for (int i = 0; i < n; i++)
					printf("%02x", char_array[i]);
				puts("\n");
				std::string json_output;
				if (CayenneLPPDec::ParseLPP(char_array, n, &json_output))
				{
					std::cout << "Json output " << json_output << " \n\n";
				}
				else
					std::cout << "Error decoding Cayenne message\n";
				
				// create a json object to send to the server
				json j_out;
				j_out["time"] = jmsg["rxInfo"][0]["time"];
				j_out["rssi"] = jmsg["rxInfo"][0]["rssi"];
				j_out["snr"] = jmsg["rxInfo"][0]["loRaSNR"];
//				j_out["crc"] = jmsg["rxInfo"][0]["crcStatus"];
				j_out["sf"] = jmsg["txInfo"]["loRaModulationInfo"]["spreadingFactor"];
//				j_out["message"] = decoded_data;
				j_out["message"] = json_output;
				
				std::string str_out;
				str_out = j_out.dump();
//				std::cout << "\nJSON to send: " << str_out << "\n";

				UDP udp;
				DNS_Name server;
				char udp_buffer[1000];
				
				strcpy (server.dns_name, "sensor.pprresponder.com");
				server.port = 40000;
				strcpy(udp_buffer, str_out.c_str());
				udp.SendUDPMessage_DNS(udp_buffer, &server);
			}
		}
		catch (std::exception& ex)
		{
			std::cout << "\nJSON Data access" << ex.what() << "\n";
		}
	}

	void delivery_complete(mqtt::delivery_token_ptr token) override {}

public:
	callback(mqtt::async_client& cli, mqtt::connect_options& connOpts)
				: nretry_(0), cli_(cli), connOpts_(connOpts), subListener_("Subscription") {}
};

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	// A subscriber often wants the server to remember its messages when its
	// disconnected. In that case, it needs a unique ClientID and a
	// non-clean session.

	mqtt::async_client cli(SERVER_ADDRESS, CLIENT_ID);

	mqtt::connect_options connOpts;
	connOpts.set_clean_session(false);

	// Install the callback(s) before connecting.
	callback cb(cli, connOpts);
	cli.set_callback(cb);

	// Start the connection.
	// When completed, the callback will subscribe to topic.

	try {
		std::cout << "Connecting to the MQTT server..." << std::flush;
		cli.connect(connOpts, nullptr, cb);
	}
	catch (const mqtt::exception& exc) {
		std::cerr << "\nERROR: Unable to connect to MQTT server: '"
			<< SERVER_ADDRESS << "'" << exc << std::endl;
		return 1;
	}

	// Just block till user tells us to quit.

	while (std::tolower(std::cin.get()) != 'q')
		;

	// Disconnect

	try {
		std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
		cli.disconnect()->wait();
		std::cout << "OK" << std::endl;
	}
	catch (const mqtt::exception& exc) {
		std::cerr << exc << std::endl;
		return 1;
	}

 	return 0;
}

