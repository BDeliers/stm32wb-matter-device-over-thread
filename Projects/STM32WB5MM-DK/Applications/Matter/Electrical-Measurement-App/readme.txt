****
  @page Electrical_Measurement_App
  @verbatim
  ******************************************************************************
  * @file    Matter/Electrical_Measurement_App
  * @author  BDeliers, derived from MCD Application Team work
  * @brief   This file lists the modification done by BDeliers
  ******************************************************************************
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * Copyright (c) 2024 BDeliers.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

### 07-Feb-2024 ###
========================

STM32WB-Matter-BLE-Thread example is based on Matter and behaves as a Matter accessory communicating over a 802.15.4 Thread network.
It can be paired into an existing matter network and can be controlled by this network.
The STM32WB55xx-Nucleo board running electrical-measurement-app application is capable 
of BLE and OpenThread activity at the same time.

@par Keywords

COAP,THREAD,BLE,Matter

@par Directory contents 

 
  Using Matter v1.1.0 SDK.
  

  - Matter/Electrical-Measurement-App/STM32_WPAN/App/app_ble.h            Header for app_ble.c module
  - Matter/Electrical-Measurement-App/Core/Core/Inc/app_common.h          Header for all modules with common definition
  - Matter/Electrical-Measurement-App/Core/Core/Inc/app_conf.h            Parameters configuration file of the application
  - Matter/Electrical-Measurement-App/Core/Core/Inc/app_entry.h           Parameters configuration file of the application
  - Matter/Electrical-Measurement-App/STM32_WPAN/App/app_thread.h         Header for app_thread.c module
  - Matter/Electrical-Measurement-App/STM32_WPAN/App/ble_conf.h           BLE Services configuration
  - Matter/Electrical-Measurement-App/STM32_WPAN/App/ble_dbg_conf.h       BLE Traces configuration of the BLE services
  - Matter/Electrical-Measurement-App/Core/Core/Inc/hw_conf.h             Configuration file of the HW
  - Matter/Electrical-Measurement-App/Core/Inc/main.h                     Header for main.c module
  - Matter/Electrical-Measurement-App/STM32_WPAN/App/p2p_server_app.h     Header for BLE P2P Server application
  - Matter/Electrical-Measurement-App/Core/Inc/stm32wbxx_hal_conf.h       HAL configuration file
  - Matter/Electrical-Measurement-App/Core/Inc/stm32wbxx_it.h             Interrupt handlers header file
  - Matter/Electrical-Measurement-App/STM32_WPAN/App/app_ble.c            BLE Profile implementation
  - Matter/Electrical-Measurement-App/STM32_WPAN/App/app_matter.c         Matter Profile implementation
  - Matter/Electrical-Measurement-App/STM32_WPAN/app/app_thread.c         Thread application implementation
  - Matter/Electrical-Measurement-App/STM32_WPAN/Target/hw_ipcc.c         IPCC Driver
  - Matter/Electrical-Measurement-App/Core/Src/app_entry.cpp              Initialization of the application
  - Matter/Electrical-Measurement-App/Core/Core/Src/stm32_lpm_if.c        Low Power Manager Interface
  - Matter/Electrical-Measurement-App/Core/Core/Src/hw_timerserver.c      Timer Server based on RTC
  - Matter/Electrical-Measurement-App/Core/Core/Src/hw_uart.c             UART Driver
  - Matter/Electrical-Measurement-App/Core/Src/app_main.cpp               Main program
  - Matter/Electrical-Measurement-App/STM32_WPAN/App/p2p_server_app.c     BLE P2P Server application implementation
  - Matter/Electrical-Measurement-App/Core/Src/stm32wbxx_it.c             Interrupt handlers
  - Matter/Electrical-Measurement-App/Core/Src/system_stm32wbxx.c         stm32wbxx system source file
  - Matter/Electrical-Measurement-App/Core/Src/AppTask.cpp                Entry application source file for matter 
  - Matter/Electrical-Measurement-App/Core/Src/ZclCallbacks.cpp           Cluster output source file for Matter
  - Matter/Electrical-Measurement-App/Core/Src/freertos_port.c            Custom porting of FreeRTOS functionalities
  - Matter/Electrical-Measurement-App/Core/Src/entropy_hardware_poll.c    Custom porting of entropy with MbedTLS
  - Matter/Electrical-Measurement-App/Core/Inc/FreeRTOSConfig.h           FreeRTOS specific defines
  - Matter/Electrical-Measurement-App/Core/Inc/mbedtls_user_config.h      MbedTLS specific defines
  - Matter/Electrical-Measurement-App/Core/Inc/CHIPProjectConfig.h        Matter specific defines
  - Matter/Electrical-Measurement-App/Core/Inc/AppEvent.h                 App event handler
  - Matter/Electrical-Measurement-App/Core/Inc/AppTask.h                  Header for AppTask.cpp module
  
 
@par Hardware and Software environment

  - This example has been tested with an STMicroelectronics STM32WB5MM-DK 
    board 
    


@par How to use it ? 

This application requests having the stm32wb5x_BLE_Thread_ForMatter_fw.bin binary flashed on the Wireless Coprocessor.
If it is not the case, you need to use STM32CubeProgrammer to load the appropriate binary.
All available binaries are located under /Projects/STM32_Copro_Wireless_Binaries directory.
Refer to UM2237 to learn how to use/install STM32CubeProgrammer.
Refer to /Projects/STM32_Copro_Wireless_Binaries/ReleaseNote.html for the detailed procedure to load the proper
Wireless Coprocessor binary. 


Minimum requirements for the demo:
- 1 STM32WB5MM-DK board with Electrical-Measurement-App firmware.
- 1 STM32WB55xx-Nucleo board running a Thread RCP connected to a Raspberry pi 4 (recommanded) via FTDI
- 1 Smartphone (Android) with "CHIPTool" Phone Application (available Utilities/APK/app-debug-v_1_0.zip)



In order to make the program work, you must do the following: 
 - Connect each STM32WB55xx-Nucleo boards to your PC 
 - Open STM32CubdeIDE version 1.9.0 or above
 - Rebuild all files and load your image into target memory
 - Run the example
 
To get the traces in real time, you can connect an HyperTerminal to the STLink Virtual Com Port. 
    
 For the Cli control and for the traces, the UART must be configured as follows:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits 
    - Stop Bit = 1 bit
    - Parity = none
    - Flow control = none

**** START DEMO ****
By default, after a reset, the board will start both protocols BLE and OpenThread.
So you BLE activity and OpenThread activity can be done in parallel :

**** BLE ACTIVITY ****
The BLE activity demonstrates a Point-to-Point communication using BLE component. 
The Peripheral device (P2P Server) starts a fast advertising after reset for 30 seconds.


**** THREAD ACTIVITY ****
After reset, OpenThread is started and nodes tries to build up a Thread network or attach to a existing one


**** LCD SUMMARY ****
LCD Mapping :
 - The LCD screen displays "BLE connected" when the BLE rendezvous started.
 - The LCD screen displays "Network Joined" when the board joined a thread network. 
   and pressing "On" and erase this line when pressing "Off" on Android phone.
 - The LCD screen displays: "Fabric Created": if commissioning success.
                            "Fabric Failed" : if commissioning failed.
                            "Fabric Found"  : if the credentials from the non-volatile memory(NVM)
                                              have been found 
****  BUTTON SUMMARY ****
Button Mapping:
- Button 1 : Press the button for at least 10 seconds to do a factory reset.
          Push the button the save the non-volatile memory after unpairing the device then power off the board.

Coprocessor firmware:
The Coprocessor firmware remains unchanged. It has been tested with the binary listed above.



 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 * <h3><center>&copy; COPYRIGHT BDeliers</center></h3>
 */
 


 
	   
