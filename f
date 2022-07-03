[33mcommit ff19c52a83161ee991e62e93a147c5a3a6e0c253[m[33m ([m[1;36mHEAD -> [m[1;32mACPI-EC[m[33m, [m[1;31morigin/ACPI-EC[m[33m)[m
Author: Author Name <k.mohanraj@adlinktech.com>
Date:   Mon Mar 14 12:28:17 2022 +0530

    updated print messages and passcode for ODM region
    
    Signed-off-by: sandadi.mahesh <sandadi.mahesh@adlinktech.com>

[33mcommit 181a5efc4fb9e3c2a44d2c3b0b0401dfcf3b3ab1[m
Author: Author Name <k.mohanraj@adlinktech.com>
Date:   Fri Mar 11 17:04:00 2022 +0530

    Added ODM region write support
    
    Signed-off-by: sandadi.mahesh <sandadi.mahesh@adlinktech.com>

[33mcommit b017cde012cdb6aea6a28d4f4e1ac99e094b10fe[m
Author: Tamilvanan <tamilvanan.sivam@adlinktech.com>
Date:   Thu Feb 10 10:22:04 2022 +0530

    Added Hardware Monitor String capability support
    
    Signed-off-by: Tamilvanan <tamilvanan.sivam@adlinktech.com>

[33mcommit a3e4a7f5cedf2bd39926df5c59befc05791633a3[m
Author: viswanathan.m <viswanathan.m@adlinktech.com>
Date:   Tue Nov 9 14:46:05 2021 +0530

    Fix for "ADLINK" Customer ID Checking Issue
    
    Signed-off-by: viswanathan.m <viswanathan.m@adlinktech.com>

[33mcommit dcb8084d4ac7c9642e937ae13fb9e93ccb69551e[m
Author: Viswanathan M <viswanathan.m@adlinktech.com>
Date:   Thu Aug 26 10:40:50 2021 +0530

    Added function to stop watchdog reboot
    
    Signed-off-by: Viswanathan M <viswanathan.m@adlinktech.com>

[33mcommit ddbbf06818c586ba0e15451865a3566bd8407e95[m
Author: Antony Abee Prakash XV <antonyabee.prakashxv@adlinktech.com>
Date:   Wed Aug 25 12:38:25 2021 +0530

    Fix for Secure data region read/write
    
    - offset value has been changed to secure data region range
      for read/write functions in adl-bmc-nvmem-sec driver.
    
    Signed-off-by: Antony Abee Prakash XV <antonyabee.prakashxv@adlinktech.com>

[33mcommit cda2f1fd8db564ad4c1bdc7debb819efd474c18a[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Thu Jul 15 02:34:43 2021 +0800

    removed board check in the init.

[33mcommit 87ce7c0a5b98c3aba22692f51acad6da737b140d[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Thu Jul 8 17:54:39 2021 +0800

    wdt mirror bytes update issue solved.

[33mcommit dfee381059f5da3ddd3d6162d4f0e75ef6684a39[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Wed Jul 7 14:29:46 2021 +0800

    fixing WDT issue

[33mcommit 280b81a1e1f600f34e4a94a4176e87f6b51dc495[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Tue Jul 6 17:59:37 2021 +0800

    Fixing probe issue

[33mcommit 4989988d89398cb704bd6cc7f841b88b22921b69[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Tue Jul 6 01:16:11 2021 +0800

    Fixing probe and status function of I2C.

[33mcommit 635af22c62e6aac7ba4f276eec6720c26198c852[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Fri Jul 2 22:11:25 2021 +0800

    Fixing WDT issue in EL

[33mcommit 77615cb401c14908d8d59704f558efa5e40e187b[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Fri Jul 2 17:10:41 2021 +0530

    fixing I2C bus name issue

[33mcommit 10f8f1e45e3ce21ca893acb07e960e9a6439e20b[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Fri Jul 2 02:21:34 2021 +0800

    added passcode for unlock the memory

[33mcommit 020e112bdb392f1e13c4627aa97afa6f3dade712[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Thu Jul 1 19:03:20 2021 +0800

    added BIOS select command. updated RTC

[33mcommit 8419c473003e3b2131e8140cead8bcd69264d365[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Thu Jul 1 15:42:21 2021 +0530

    adding BIOS Sel variable in error log. Partial.

[33mcommit 6230fd9e8cb8f4bb4d2b97f7b851585ea6bbbe8c[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Tue Jun 29 20:52:19 2021 +0800

    adding totalontime in errorlog

[33mcommit d2b9a194d4a50814e514499e8e14338c14348d50[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Tue Jun 29 19:56:37 2021 +0800

    updating Exceptio descrition

[33mcommit 0c98f6b138ae957a774b693e7bfbef9d1fa69ae6[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Sat Jun 26 06:15:39 2021 +0530

    updated voltage description and exception description

[33mcommit 689a14d2fb22bea80548dd8d7fb821156edf04bd[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Mon Jun 21 12:24:21 2021 +0800

    fixing get board info and wdt issue.

[33mcommit 338b695e116e1270926c433f9848e5fa2a02b1a8[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Thu Jun 17 20:30:17 2021 +0800

    Fixing WDT and BDInfo issue.

[33mcommit f4c6a66cc5d9fe35ea1d833c53e1cf07188d9b13[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Mon Jun 14 17:06:52 2021 +0800

    Clear function is removed from latest spec.

[33mcommit 1fe8fc7ddf86b02ab5e0519db80c88226e664e76[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Thu Jun 10 16:35:19 2021 +0800

    Fixing Board information access issue.

[33mcommit c4d44006072781526963fb30de541dc5643a6231[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Thu Jun 10 15:39:23 2021 +0800

    Fixing error log and storage issue.

[33mcommit 4bffb3fbf02d603e58524dde637f46cead06e543[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Wed Jun 9 01:00:00 2021 +0800

    Fixing Storage issue.

[33mcommit 157d419abf53e581ae7088b9ed6d362633ebfb1a[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Wed May 5 15:47:41 2021 +0800

     Fixing JIRA issues EL-840, TL-686 and AR-535

[33mcommit 0c8b89551ac3ec9688ef5437bec2f53932db7561[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Tue Apr 27 18:49:17 2021 +0530

    "Fixing JIRA issue AR-512"

[33mcommit f0ea64ee3f88b2c7791f29291bd8af82a6a8d985[m
Author: sankrraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Tue Apr 27 15:06:43 2021 +0800

    Fixing AR-536 JIRA issue.

[33mcommit 6b0c4524dddf3150ed0519f6df8e83a17963b245[m
Author: sankrraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Mon Apr 26 19:23:53 2021 +0800

    Fixing AR-536 JIRA issue.

[33mcommit dd3c47667734036581a58821789261033ce91ac9[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Fri Apr 23 18:08:51 2021 +0530

    "Fixing JIRA issues 533, 534"

[33mcommit 69325da0b50ecaaba5f0ed5ffc5de5f529c2c502[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Fri Apr 23 16:23:05 2021 +0530

    "Fixing JIRA issues 533 and 534"

[33mcommit 448b23b8b78ad5b130b686f58aeff91d687f07d3[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Fri Apr 16 17:55:21 2021 +0530

    Fixing JIRA issue 677 and updated JIRA issues 840, 658

[33mcommit dbc0054afe8a7c399636a3305f17908d3e701f76[m
Author: sankarraj.saravanan <sankarraj.saravanan@ltts.com>
Date:   Wed Apr 14 00:05:10 2021 +0800

    Fixing JIRA issues CEXPRESSARR-527.

[33mcommit 504ffc43ef01bc80a9d56b2ed4ccaa774a97b741[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Tue Apr 13 18:52:04 2021 +0530

    Fixing JIRA issues 644 and 518

[33mcommit e4400185fb037a69397390ef8c241f9ab69a7ebe[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Tue Apr 13 18:50:49 2021 +0530

    Fixing JIRA issues 644 and 518

[33mcommit 1c69f32930653d9945a84d093ea3396cc92dc047[m
Author: sankarraj.saravanan <sankarraj.saravanan@ltts.com>
Date:   Tue Apr 13 20:33:11 2021 +0800

    fixing JIRA issues 654 658

[33mcommit 65f72992bf6d31317719a25039d932737b3b5409[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Fri Apr 9 16:39:00 2021 +0530

    Fixing JIRA issue CEXPTRESSTL 643 650 655 653 and 649 and CEPRESSARR-457.

[33mcommit 9f7e13d388f504868dd5f958371b6bb5139ca772[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Mar 11 21:19:17 2021 +0800

    Removing unwanted files(generated at compilation time).

[33mcommit 01299936ad87ec40d6dce7e1f22753a53c626964[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Mar 11 21:17:10 2021 +0800

    Fixing JIRA issue no CEXPTRESSL-840

[33mcommit 137c89541d9fc99e57ef7be5f47e3bbded0f5084[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Mon Jan 25 17:42:16 2021 +0530

    updated i2c driver- resource handling & code optimization

[33mcommit 2c6a4525dc7b03197476b78726cef146a559fd4e[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Mon Jan 25 15:38:44 2021 +0530

    nvmem user and secure space working

[33mcommit 2f5af5e4230fbef24b3fda51c29983e61349414e[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Fri Jan 22 18:40:22 2021 +0530

    nvmem with 2 drivers, code modularity in process

[33mcommit 24f0241e5fdd30b7f09de6ae0b05d0c2fff99e37[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Thu Jan 21 20:23:27 2021 +0530

    Updated bus I2C 3 functionality.

[33mcommit a3ec095401971d0f363db65433276500fd233b8b[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Wed Jan 20 17:49:22 2021 +0530

    nvmem for Secured and user region is working

[33mcommit 34e068fb09261ed5c8b9d155ffbd8b6e639769f1[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Tue Jan 19 18:44:52 2021 +0530

    secure and user space in nvmem working properly

[33mcommit e59c3ae30c6bba93008be04ac6557731c65012d6[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Mon Jan 18 18:55:47 2021 +0530

    User and secure storage working with 2 saparate drivers

[33mcommit 0a0988b56944986e1d76684f85fca12e05ed031c[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Wed Jan 13 16:04:36 2021 +0530

    Secure data lock and unlock working user data not working

[33mcommit e0dcbb8e2691101ec1578419ebe1317a13ea9d4a[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Tue Jan 12 18:52:42 2021 +0530

    Added lock and unlock in nvmem

[33mcommit 8c111a7297a9ebeec069b34a28031c0a82d43378[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Fri Jan 8 12:10:50 2021 +0530

    storage.c fileUpdated

[33mcommit 1fc772a269c99e311cb31c1562270cc58e0e237e[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Wed Jan 6 18:17:11 2021 +0530

    main.c updated

[33mcommit f40c14066bc404232ead7cf3c6cf96df19b3a20b[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Tue Jan 5 18:42:19 2021 +0530

    main.c is modifed

[33mcommit d263ad602a52aecd4673720cc742321bd0a50277[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Thu Dec 31 18:21:32 2020 +0530

    EC storage secure area in progress code

[33mcommit 224a17855726efe2090c325d230a5e2249cb9cce[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Thu Dec 31 16:47:21 2020 +0530

    fix for improper print in storage

[33mcommit 09ee6dfcb887b53834ec92da76c773d5f864a02c[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Tue Dec 22 14:59:12 2020 +0530

    i2c.c code update

[33mcommit d014ad376bce016274f0226272ad0ee183232ad1[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Mon Dec 21 17:00:19 2020 +0530

    main.c Update

[33mcommit 6e58ff19695612bb6791676fe4d92d2bc9b8bef3[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Fri Dec 18 16:27:00 2020 +0530

    Library files code update

[33mcommit f7d10c186d0f133e5e005fd62db7674ab791e286[m
Author: sankarraj.saravanan <sankarraj.saravanan.com>
Date:   Fri Dec 18 13:05:21 2020 +0530

    Library and Application code update

[33mcommit bc78b00e8d45ff929233492fbf55ee729e087026[m
Author: sankarraj <sankarraj.saravanan>
Date:   Fri Dec 18 12:03:03 2020 +0530

    Library and application code update

[33mcommit f9dac5f3ffbd329bb9387adaa7c48a0c78f4830b[m
Author: sankarraj <sankarraj.saravanan@adlinktech.com>
Date:   Wed Dec 16 14:52:44 2020 +0530

    print issue in storage option

[33mcommit ece8dc018994e2b4a4f88bbca423f1d42b540a39[m
Author: sankarraj <sankarraj.saravanan.com>
Date:   Wed Dec 16 10:34:22 2020 +0530

    Library files update

[33mcommit ff0e7c68688da65f18f9dbcf0462ae6fc862e951[m
Author: sankarraj <sankarraj.sarvanan.com>
Date:   Mon Dec 14 19:31:10 2020 +0530

    gpio.c file updated

[33mcommit 4912a4165eeacad1f15c499beb53ab80f3c07a96[m
Author: sankar raj <sankarraj.saravanan.com>
Date:   Mon Dec 14 18:39:00 2020 +0530

    gpio code update

[33mcommit dcad72d095498abc5684b1e41c3d200869d104b9[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Dec 3 14:43:26 2020 +0800

    gpio code update for lib/app

[33mcommit ab50841ca2ad71ef672776a4ad84658fd1b99160[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Tue Dec 1 21:24:15 2020 +0800

    i2cdetect working

[33mcommit ae818c38eec580f1e249445a09ee02e77820c466[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Tue Dec 1 20:10:10 2020 +0800

    tested write_raw, read_raw, write_xfer and read_xfer with testi2c.sh script file

[33mcommit d12dcb2f9ea4ca4c63b60a2076f1992d04fe186a[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Tue Dec 1 08:58:56 2020 +0800

    script files for debugging

[33mcommit fc142e6b6e7a1aaab42b8fa35d663ca12d7ab885[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Sat Nov 28 09:05:41 2020 +0800

    cpu fan speed

[33mcommit 3d6a8b7e9bb5b4859277a98444036daae4e05c63[m
Author: sankarraj <sankarraj.saravanan@adlinktech.com>
Date:   Fri Nov 27 14:56:30 2020 +0530

    WDT stop led of issue

[33mcommit 55bc66b81fe69f814f69ff517e9e0d3e7bebc8df[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Wed Nov 25 20:42:13 2020 +0800

    board infor junk fix

[33mcommit c83ca8f1887627fc43627536e81e7043e2d6e58c[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Mon Nov 23 21:58:49 2020 +0800

    i2c code

[33mcommit 2dd6c7a4dec430c6ce50815dacdc4329d4abc43d[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Wed Nov 18 22:36:07 2020 +0800

    write_xfer and write_raw working

[33mcommit 6f13edc3dd9c21b6f8cd956358755df871000f06[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Nov 12 23:53:38 2020 +0800

    i2c read/write xfer cleaned

[33mcommit 56e011489fc1d4bf96baf7f665dd643fbc54ec95[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Nov 12 20:07:00 2020 +0800

    read_xfer code un cleaned

[33mcommit 39611841e49f155c6026eb3ccd6f4f8b5cc6dff3[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Nov 12 17:11:22 2020 +0800

    read_xfer working with hardcoded

[33mcommit 1eba198e399a2da7c9fbb74e289d8411fd55c904[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Tue Nov 10 19:35:14 2020 +0800

    write_xfer code

[33mcommit d4b5c26a6e577762e4047de5f036e470ea781365[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Fri Nov 6 16:03:33 2020 +0800

    gpio code

[33mcommit 351466d9cf8f8ff961314fd487f7540c99db7773[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Fri Nov 6 09:17:04 2020 +0800

    gpio code

[33mcommit 23788455fab1955184e30e0eb3757f73a1ac5c9a[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Fri Nov 6 07:44:34 2020 +0800

    gpio code

[33mcommit 1e68c02674a09310995ca2a2b32a55f8c30185a2[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Fri Nov 6 07:30:08 2020 +0800

    gpio code

[33mcommit 8eaf87a4b918d2feaabefe52577b93a8687d9d94[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Nov 5 23:01:44 2020 +0800

    gpio code

[33mcommit cfbc7e8d70713baafc9fc7c520df9919600c527b[m
Author: Sankar raj <sankarraj.saravanan@adlinktech.com>
Date:   Thu Nov 5 22:58:05 2020 +0800

    gpio code

[33mcommit 73e8ae51a33c7831e15e11fbb3f55a08e8d89872[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Fri Sep 18 12:40:15 2020 -0700

    Support added for Board Values and I2C functions.

[33mcommit 1734a2487ac1835cff5e2fec8f19f53bc31fd9c5[m
Author: sankarraj.saravanan <sankarraj.saravanan@adlinktech.com>
Date:   Wed Sep 9 13:14:55 2020 -0700

    Initial support added for backlight, Board information, FAN, NVMEM, WDT, Exception description and voltage monitor.

[33mcommit 17cb6d85bdc1705a0af8306e8cecb148e5aef9b3[m[33m ([m[1;31morigin/ec[m[33m)[m
Author: gowtham.r <gowtham.r@adlinktech.com>
Date:   Tue Apr 14 10:14:54 2020 -0700

    updating santhana kumar and windows code in EC
