## Introduction



<img src="https://cdn.adlinktech.com/webupd/en/Upload/ProductNews/logo_sema.png" alt="sema_logo" width="30%" align="right"  />

## What is SEMA 4.0?

**The Smart Embedded Management Agent (SEMA®)** 

Downtime of devices or systems is not acceptable in today's industries. To help customers to analyze their 
systems and take counter measures for preventive maintenance, ADLINK Technology Inc. has developed a tool which is able to monitor and collect system performance and status information from the hardware in a timely, flexible and precise manner. A Board Management Controller collects all relevant technical information from the chipset and other sources.

Using the System Management Bus driver, an application layer fetches the data and presents it to the user. 
SEMA® provides a ready-made application that shows the data in user-friendly graphic interfaces, suitable 
for supervision and troubleshooting.



Features
----------

SEMA® is designed to be:

* Power Consumption
* User Area Access
* I2C Control 
* Temperatures (CPU and Board)
* Board Information (Serial Number, Part Number, Firmware Version...)
* Fan Control
* GPIO Control (only support PCA9535 I/O Expander)
* Watch Dog  


Detailed forensic information is available after system or module failures. The BMC Power-Up Error Log function provides detailed information about history of failures that may have occurred during power-up sequences. Log information includes e.g. error number, flags, restart event, power cycles, boot count, status, CPU temperature and board temperature. Moreover minimum and maximum temperature of the CPU and system is available to analyze system or module failure in detail.



Support Operating System
--------------------------
* **Windows 10 64bit**

* **Linux (kernel 4.15 or above)**
  
* **Yocto Linux ([meta-adlink-sema](https://github.com/ADLINK/meta-adlink-sema/tree/sema4.0))**  

* **VxWorks (by request)** 
* **QNX (by request)**


## How to install
* see [documentation](https://adlinktech.github.io/sema-doc/#/source/HowToInstallSEMA?id=ubuntu-linux) for more details

## Supported Hardware List:
* see [the hardware list](https://adlinktech.github.io/sema-doc/#/source/SupportedHardware) for more details

## Developer's Guide (how to use EAPI or Sysfs): 
* See [documentation](https://adlinktech.github.io/sema-doc/#/source/DeveloperGuide) for more details.

   
## Other information:
internal GitLab commit ID: 981f6bf59bca14101d6c9d1f5345a69d1a5f91df

