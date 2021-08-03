Release Date: 28 July 2021
Release Version: 1.1.1
History: 
  Ver.1.0 11 Apr 2019 Initial version. Support TC9560/TC9562 I2C/SPI mode.
  Ver.1.1 30 Mar 2021 Support TC9563/TC9564 I2C/SPI mode.
  Ver.1.1.1 License changed to MIT License
===============================================================================


1. Function
  - This application is to write firmware to internal SRAM(IMEM)
     on reference board via I2C/SPI interface using Raspberry Pi 3.


2. Tested environment
      HW: Raspberry Pi 3 Model B 
      OS: Linux raspberrypi 4.4.39-v7

3. HW Setting
  - Switch
     Set the switch along the board revision you are using.
        |--------------------||----------------|----------------|-------------------------------|
        |                    ||    For I2C     |    For SPI     |      Remark                   |
        |--------------------||----------------|----------------|-------------------------------|
        |                    || SW7: No.1->OFF | SW7: No.1->ON  |I2C/SPI select                 |
        |                    ||                |                |                               |
        | TC9560 Rev.2 board || SW6: No.1->OFF | SW6: No.1->ON  |SW6-1/2 OFF/ON I2C slave       |
        |                    ||      No.2->ON  |      No.2->OFF |        ON/OFF SPI slave       |
        |                    ||      No.9->ON  |                |I2C Address ON:0x77, OFF:0x33  |
        |--------------------||----------------|----------------|-------------------------------|
        |                    || SW4: No.1->OFF | SW4: No.1->ON  |I2C/SPI select                 |
        |                    ||                |                |                               |
        | TC9560 Rev.3 board || SW1: No.1->OFF | SW1: No.1->ON  |SW1-1/2 OFF/ON I2C slave       |
        |                    ||      No.2->ON  |      No.2->OFF |        ON/OFF SPI slave       |
        |                    ||      No.9->ON  |                |I2C Address ON:0x77, OFF:0x33  |
        |--------------------||----------------|----------------|-------------------------------|
        |                    || SW4: No.1->OFF | SW4: No.1->ON  |I2C/SPI selection              |
        |                    ||                |                |                               |
        | TC9562 board       || SW1: No.1->ON  | SW1: No.1->OFF |Slave mode ON:I2C , OFF:SPI    |
        |                    ||      No.4->ON  |                |I2C Address ON:0x77, OFF:0x33  |
        |--------------------||----------------|----------------|-------------------------------|
        |                    || SW17: 2-3 Short| SW17: 1-2 Short|I2C/SPI selection (data)       |
        |                    || SW18: 2-3 Short| SW18: 1-2 Short|I2C/SPI selection (clock)      |
        | TC9564 Evaluation  ||                |                |                               |
        | board (EBTC9564)   || SW3: No.1->ON  | SW3: No.1->OFF |Slave mode ON:I2C , OFF:SPI    |
        |                    ||      No.4->ON  |                |I2C Address ON:0x77, OFF:0x33  |
        |--------------------||----------------|----------------|-------------------------------|
        |                    || SW9 : 2-3 Short| SW9 : 1-2 Short|I2C/SPI selection (data)       |
        | TC9563 Reference   || SW10: 2-3 Short| SW10: 1-2 Short|I2C/SPI selection (clock)      |
        | board (RBTC9563)   ||                |                |                               |
        |                    || SW1: No.1->ON  | SW1: No.1->OFF |Slave mode ON:I2C , OFF:SPI    |
        |                    ||      No.4->ON  |                |I2C Address ON:0x77, OFF:0x33  |
        |--------------------||----------------|----------------|-------------------------------|
  
  - Wire Connection
     Connect the board and Raspberry Pi 3 along the function of the pins.
        |---------------------------||-----------------------------------|-----------------------------------------------------|
        |                           ||              For I2C              |                       For SPI                       |
        |                           ||-----------------------------------|-----------------------------------------------------|
        |                           ||  SCL   |  GND   |  SDA   |  VDD   |  MISO  |  VDD   |  CSLK  |  MOSI  |   SS   |  GND   |
        |---------------------------||--------|--------|--------|--------|--------|--------|--------|--------|--------|--------|
        |      TC956X Board         ||        |        |        |        |        |        |        |        |        |        |
        |                           ||        |        |        |        |        |        |        |        |        |        |
        | (TC9560 Rev.2 board: J7)  || Pin 1  | Pin 2  | Pin 3  | Pin 4  | Pin 5  | Pin 6  | Pin 7  | Pin 8  | Pin 9  | Pin 10 |
        | (TC9560 Rev.3 board: J45) ||        |        |        |        |        |        |        |        |        |        |
        | (TC9562 board: J45)       ||        |        |        |        |        |        |        |        |        |        |
        | (EBTC9564 board: CN11)    ||        |        |        |        |        |        |        |        |        |        |
        | (RBTC9563 board: CN7)     ||        |        |        |        |        |        |        |        |        |        |
        |---------------------------||--------|--------|--------|--------|--------|--------|--------|--------|--------|--------|
        |          Rasp.Pi          || Pin 5  | Pin 9  | Pin 3  | Pin 1  | Pin 21 | Pin 17 | Pin 23 | Pin 19 | Pin 24 | Pin 25 |
        |---------------------------||--------|--------|--------|--------|--------|--------|--------|--------|--------|--------|

4. SW Setting
  - Set Raspberry Pi configration and build source
     - For I2C
        $ sudo raspi-config nonint do_i2c 0
        $ sudo sh -c 'echo "dtparam=i2c_arm_baudrate=400000" >> /boot/config.txt'
        $ sudo sh -c 'echo "options i2c_bcm2708 combined=1" > /etc/modprobe.d/i2c_repeated_start.conf'
        $ sudo reboot
        
        $ cd <package path>/src
        $ sed -i "s/TC956X_REG_IF_TYPE\t[0-9]/TC956X_REG_IF_TYPE\t1/" tc956x.h
        $ make clean
        $ make
     
     - For SPI
        $ sudo raspi-config nonint do_spi 0
        $ sudo reboot
        
        $ cd <package path>/src
        $ sed -i "s/TC956X_REG_IF_TYPE\t[0-9]/TC956X_REG_IF_TYPE\t2/" tc956x.h
        $ make clean
        $ make

        <to change SPI speed>
        change the following line.
        tc956x_spi.c +69
            spi_speed = 5000000;                //5MHz (200nS per bit)

        <to change SPI device>
        change the following line.
        tc956x_spi.c +48
            static const char *spi_device = "/dev/spidev0.0";

5. Usage
  - Load the firmware and start
    $ ./<tc9560_ramloader or tc956x_ramloader> <binary_file_name>
      Ex.  $ ./tc9560_ramloader ../fw_OSLess_HWSeq_emac_v2.5.bin


6. Trademarks

  Raspberry Pi is a trademark of the Raspberry Pi Foundation
  Linux is the registered trademark of Linus Torvalds in the United States and/or other countries.
