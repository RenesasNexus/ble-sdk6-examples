
# DA14585/DA14586 BLE Erratum 10734 - security vulnerability

---


## Example description

This SDK6 DA14585 example shows how to send a command
from the applicaiton layer to the BLE STACK to ensure and
update the security key.
As a proof of concept it can be confirmed by analyzing SMP in BLE sniffer log.

## HW and SW configuration


* **Hardware configuration**

	- This example runs on The DA14585/DA14586 Bluetooth Smart SoC devices.
	- The Basic or pro Development kit is needed for this example.
	- Connect the USB Development kit to the host computer.
	- SPI jumper on P00/P03/P05/P06/BATT_SEL
		
		![jumper_config](assets/jumper_setup.png)


* **Software configuration**

	- This example requires:
    * Smartsnippets Studio 1.6.3.
    * SDK6.0.10
	- **SEGGER’s J-Link** tools should be downloaded and installed.
	- Python 3.5 or greater


## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](__Github sdk6 readme link__).

### Initial Setup

 - Start Keil
 - Compile and launch the example
 - Open the development kit serial port with the following parameters

		- baudrate: 115200
		- data: 8 bits
		- stop: 1 bit
		- parity: None
		- flow  control: none

As a proof of concept it can be confirmed by analyzing SMP in BLE sniffer log.

![SMP_results](assets/sec_vulnerability_sniffer_log.jpg)



## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums).
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.
