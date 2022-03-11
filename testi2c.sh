sudo ./semautil /i2c write_xfer 1 a8 2 00 4 aa 11 22 33
sudo ./semautil /i2c read_xfer 1 a8 2 00 4
sudo ./semautil /i2c write_xfer 1 c4 1 3 40 2 90
sudo ./semautil /i2c read_xfer 1 c4 1 3
sudo ./semautil /i2c read_raw 1 c4 3
