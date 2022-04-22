

### What is SEMA?

**The Smart Embedded Management Agent (SEMA®)**

Downtime of devices or systems is not acceptable in today's industries. To help customers to analyze their systems and take counter measures for preventive maintenance, ADLINK Technology Inc. has developed a tool which is able to monitor and collect system performance and status information from the hardware in a timely, flexible and precise manner. A Board Management Controller collects all relevant technical information from the chipset and other sources.

Using the System Management Bus driver, an application layer fetches the data and presents it to the user. SEMA® provides a ready-made application that shows the data in user-friendly graphic interfaces, suitable for supervision and troubleshooting.

<br>

### Important Notice 

SEMA4.0 would be supported for the different board controllers with our design. It would take effort to implement the layer between the driver and the controller. 

Prior to the release of SEMA 4.0 integration, **please check on which platform/hardware you're using first and then select the corresponding branch on GitHub.**

| sema-ec<br> https://github.com/ADLINK/sema-linux/tree/sema-ec            | sema-bmc<br>https://github.com/ADLINK/sema-linux/tree/sema-bmc |
| :----------------------------------------------------------- | :------------------------------------------------------------ |
| - cExpress-TL , cExpress-EL, cExpress-AR<br>- Express-ID7, Express-ADP, Express-TL <br>- COM-HPC-cADP, COM-HPC-sIDH <br>- LEC-EL <br>- nanoX-EL | - Express-CF/CFE, Express-KL/KLE, Express-SL/SLE, Express-DN7, Express-BD7 <br>- cExpress-WL, cExpress-KL, cExpress-SL, cExpress-AL<br>- nanoX-AL<br>- LEC-AL<br>- Q7-AL<br>- LEC-PX30 |

<br>

### Architecture Overview 

Here is the architecture of SEMA 4.0 as below:

 

![image-20220422162134957](Readme.assets/image-20220422162134957.png)
 



* Modularization implementation in SEMA drivers an each driver can be individually installed based on your needs.

* When you develop your program,  we provide two approaches to access the board controller:

  * **EAPI(Embedded API) library:**  

    PICMG® organization defined the software specification on COM Express for the industrial applications. Here is the available specification https://www.picmg.org/wp-content/uploads/COM_EAPI_R1_0.pdf for your reference.

    Also, can use EPAI function calls to access the board controller if the developers are not familiar with sysfs interfaces.

  * **Sysfs Interface:** 

    With the exposure of Sysfs interfaces, it can be easier and straightforward to access the board controller.  

    

**Note:** All of source code is free to use. it includes SEMA driver, EAPI library, and utility. 

<br>


### Features Set

* Power Consumption
* User Area Access
* I2C Control 
* Temperatures (CPU and Board)
* Board Information (Serial Number, Part Number, Firmware Version...)
* Fan Control
* GPIO Control 
* Watch Dog  


Detailed forensic information is available after system or module failures. The BMC Power-Up Error Log function provides detailed information about history of failures that may have occurred during power-up sequences. Log information includes e.g. error number, flags, restart event, power cycles, boot count, status, CPU temperature and board temperature. Moreover minimum and maximum temperature of the CPU and system is available to analyze system or module failure in detail.






#### 
