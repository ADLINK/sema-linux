
What is SEMA?
-----
<img src="https://cdn.adlinktech.com/webupd/en/Upload/ProductNews/logo_sema.png" alt="sema_logo" width="20%" align="right"  />


**The Smart Embedded Management Agent (SEMA®)**

Downtime of devices or systems is not acceptable in today's industries. To help customers to analyze their systems and take counter measures for preventive maintenance. We provide the solution which is able to monitor and collect system performance and status information from the hardware in a timely.

A Board Management Controller is embedded on our hardware and collects all relevant technical information from the chipset through the different communication interfaces such as eSPI, I2C and SMBus.

SEMA® middleware is on the top of Board controller that provides a ready-made application that shows the data in user-friendly graphic interfaces, suitable for supervision and troubleshooting.


### Important Notice 

SEMA4.0 would be designed the abstraction layer to integrate the different controllers (EC, TivaBCC and LiteBMC) and this integration will be ready in Q4 2022. 

Prior to the release of SEMA 4.0 integration, **please check which platform/hardware you're using first and then select the corresponding branch on GitHub**

| sema-ec<br> https://github.com/ADLINK/sema-linux/tree/sema-ec            | sema-bmc<br>https://github.com/ADLINK/sema-linux/tree/sema-bmc |
| :----------------------------------------------------------- | :------------------------------------------------------------ |
| - cExpress-TL , cExpress-EL, cExpress-AR<br>- Express-ID7, Express-ADP, Express-TL <br>- COM-HPC-cADP, COM-HPC-sIDH <br>- LEC-EL <br>- NanoX-EL | - Express-CF/CFE, Express-KL/KLE, Express-SL/SLE, Express-DN7, Express-BD7 <br>- cExpress-WL, cExpress-KL, cExpress-SL, cExpress-AL<br>- nanoX-AL<br>- LEC-AL<br>- Q7-AL<br>- LEC-PX30 |


Architecture Overview
-----

Here is the architecture of SEMA 4.0 as below:

 

![image-20220422162134957](Readme.assets/image-20220422162134957.png)
 



* Modularization implementation in SEMA drivers and each driver can be individually installed based on your needs.

* When you develop your program,  we provide two approaches to access the board controller:

  * **EAPI(Embedded API) library:**  

    PICMG® organization defined the software specification on COM Express for the industrial applications. Here is the available specification https://www.picmg.org/wp-content/uploads/COM_EAPI_R1_0.pdf for your reference.


  * **Sysfs Interface:** 

    With the exposure of Sysfs interfaces, it can be easier and straightforward to access the board controller.  

    

**Note:** All of source code is free to use which including SEMA driver, EAPI library, and utility.

<br>


Features Set
-----

* Power Consumption
* User Area Access
* I2C Control 
* Temperatures (CPU and Board)
* Board Information (Serial Number, Part Number, Firmware Version...)
* Fan Control
* GPIO Control 
* Watch Dog  


Support Operating System
-----
* Windows OS: Windows 10 64bit
* Linux OS: kernel 4.15 or above
* Yocto Linux: see meta-adlink-sema for more details
* VxWorks (by request)
* QNX (by request)





#### 
