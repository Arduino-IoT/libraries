/*
  WiFi.h - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef WiFi1_h
#define WiFi1_h

#include <inttypes.h>

#include "wl_definitions.h"

#include "IPAddress.h"
#include "WiFiClient1.h"



class WiFiClass1
{
public:
    static int16_t 	_state[MAX_SOCK_NUM];

    WiFiClass1();


    void init();

	/* Start Wifi connection with passphrase
	 * the most secure supported mode will be automatically selected
	 *
	 * param ssid: Pointer to the SSID string.
	 * param passphrase: Passphrase. Valid characters in a passphrase
	 *		  must be between ASCII 32-126 (decimal).
	 */
	int begin(char* ssid);
	int begin(char* ssid, const char *passphrase);

	  /*
	   * Disconnect from the network
	   *
	   * return: one value of wl_status_t enum
	   */
	  int disconnect(void);

	  /*
	   * Get the interface MAC address.
	   *
	   * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
	   */
	  uint8_t* macAddress(uint8_t* mac);
	 
	  /*
	   * Get the interface IP address.
	   *
	   * return: Ip address value
	   */
	  IPAddress localIP();
	 
	  /*
	   * Get the interface subnet mask address.
	   *
	   * return: subnet mask address value
	   */
	  IPAddress subnetMask();
	 
	  /*
	   * Get the gateway ip address.
	   *
	   * return: gateway ip address value
	   */
	 IPAddress gatewayIP();
	 
	  /*
	   * Return the current SSID associated with the network
	   *
	   * return: ssid string
	   */
	  char* SSID();
	 
	  /*
		* Return the current BSSID associated with the network.
		* It is the MAC address of the Access Point
		*
		* return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
		*/
	  uint8_t* BSSID(uint8_t* bssid);
	 
	  /*
		* Return the current RSSI /Received Signal Strength in dBm)
		* associated with the network
		*
		* return: signed value
		*/
	  int32_t RSSI();
	 
	  /*
		* Return the Encryption Type associated with the network
		*
		* return: one value of wl_enc_type enum
		*/
	  uint8_t encryptionType();
	 
	  /*
	   * Start scan WiFi networks available
	   *
	   * return: Number of discovered networks
	   */
	  int8_t scanNetworks();
	 
	  /*
	   * Return the SSID discovered during the network scan.
	   *
	   * param networkItem: specify from which network item want to get the information
	   *
	   * return: ssid string of the specified item on the networks scanned list
	   */
	  char*   SSID(uint8_t networkItem);
	 
	  /*
	   * Return the encryption type of the networks discovered during the scanNetworks
	   *
	   * param networkItem: specify from which network item want to get the information
	   *
	   * return: encryption type (enum wl_enc_type) of the specified item on the networks scanned list
	   */
	  uint8_t encryptionType(uint8_t networkItem);
	 
	  /*
	   * Return the RSSI of the networks discovered during the scanNetworks
	   *
	   * param networkItem: specify from which network item want to get the information
	   *
	   * return: signed value of RSSI of the specified item on the networks scanned list
	   */
	  int32_t RSSI(uint8_t networkItem);
	 
	  /*
	   * Return Connection status.
	   *
	   * return: one of the value defined in wl_status_t
	   */
	  uint8_t status();


	/*
	 * Resolve the given hostname to an IP address.
	 * param aHostname: Name to be resolved
	 * param aResult: IPAddress structure to store the returned IP address
	 * result: 1 if aIPAddrString was successfully converted to an IP address,
	 *			else error code
	 */
	int hostByName(const char* aHostname, IPAddress& aResult);

	friend class WiFiClient1;


};

extern WiFiClass1 WiFi1;

#endif
