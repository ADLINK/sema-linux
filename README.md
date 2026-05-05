# sema-linux 4.4.0
This is the common repository for unified sema linux supporting TIVA BMC and EC

## SEMA Overview
* SEMA (Smart Embedded Management Agent) is a unified and extensible software framework designed for ADLINK embedded platforms.

* Implements the PICMG Embedded API (EAPI) specification that interacts with hardware consistently across different platforms.

* Acts as a bridge between the hardware's Board Management Controller (BMC) or EC and the operating system.

* Provides standardized access to board-level hardware features such as sensors, i2c, thermal, GPIO, and watchdog.

* Provides key features such as Hardware Monitoring, Board Information, GPIO acess Control, Fan Control, I2C Control and WDT management.

## Repo Files Overview

* app – Contains the main user-space application used to access and demonstrate SEMA functionality.
* driver – Linux kernel driver source implementing low-level hardware control such as GPIO, I²C, watchdog, backlight, and board information.
* lib – User-space SEMA library implementing EAPI interfaces and helper functions for hardware interaction.
* sbom – Contains generated sbom json for this repo
* test – Multi-threaded test programs used to validate and stress-test various SEMA features.
* watchdogtest – Standalone utility used specifically to test watchdog timer behavior.
* LICENSE.BSD3 – BSD 3-Clause License file
* LICENSE.GPLV2 – GNU GENERAL PUBLIC LICENSE file
* LICENSE.dual – Unified SEMA Linux License file
* Makefile – Build script used to compile the SEMA drivers, libraries, and applications.
* README.md – Documentation describing the project overview, build steps, and usage instructions.
* x509.genkey – OpenSSL configuration file used to generate X.509 keys for Secure Boot kernel module signing.
 
## Supported Hardware List
* cExpress-TL , cExpress-EL, cExpress-AR, cExpress-ALN, cExpress-MTL
* cExpress-AL, cExpress-SL, cExpress-KL, cExpress-WL
* Express-ID7, Express-ADP, Express-TL, Express-ADP
* Express-CF, Express-BD7, Express-DN7, Express-CFR, Express-SL2, Express-SL/KL
* COM-HPC-cRLS, COM-HPC-sIDH, COM-HPC-mMTL, COM-HPC-cADP
* NanoX-EL
* NanoX-BT
* NanoX-ASL
* Express-VR7
* LEC-ASL/ALN
  
## Supported Operating System
* Ubuntu 20.04, 24.04

## Documents
Refer to this ([link](https://www.adlinktech.com/Products/DownloadMDownload?lang=en&pdNo=1274&MainCategory=Industrial_IoT_and_Cloud_solutions&kind=M)) to see the following guides,
- SEMA Installation Guide
- SEMA App User Guide
- SEMA EAPI Guide





