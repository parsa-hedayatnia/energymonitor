# Smart Energy Monitor (SEM)
> An implementation of different modes of SEM IOT project.
* [Introduction](#Introduction)
* [Getting Started](#Getting-Started)
* [Includes](#Includes)
* [Mode1](#Mode1)


## Introduction
The whole project is for measuring and monitoring energy of home devices. It can show some statistics information about electric devices energu usage.
The part that is implemented in this repository is 4 different modes of this project. the SEM device can be operated in 4 different modes.

## Getting-Started
First of you have to connect device into urban electrisity.

![Connect-to-E](/Screenshots/Connect-to-E.png)


Then go to Wifi setting of your phone or computer. Connect to <ConfigMePlease>

![Connect-to-SSID](/Screenshots/Connect-to-Ssid.PNG)

Go to your browser and enter <sem.local>
The page containes 3 modes plus 1 gateway mode.
### Mode 1,2
Mode 1,2 is for local mode of device using mobile App. The differene of this 2 modes is in the data which is calculated.
In mode 1 its aggregate energy, but in mode 2 the data is per hour.
In order to work in these modes you have to enter SSID and Password which you want to use for your device. 
In mode 1,2 you device works as an Access Point.

![Mode1,2](/Screenshots/Mode1,2.PNG) 

### Mode 3
In the third mode your device works as a node. It should be connected into a SSID and use it as a gateway. So you should enter SSID and Password of a router or external network.

![Mode3](/Screenshots/Mode3.PNG)

### Mode 4(Gateway)
The last mode is gateway mode. SEM device is a gateway here. So you should enter SSID and Password of a router or external network.
Here you should enter the token which MobileApp or website gives to you. The network which you connect to, should be the one in mode 3

![Mode4](/Screenshots/Mode4.PNG)


## Includes

![includes](/Screenshots/includes.png)
#### calculate.hpp
As it can be understood by its name, this library helps us for our calculations. The functions are based on the official library of Arduino. 

#### client.hpp
This library is for requesting https by clients.

#### complex.hpp
This library is for calculate and work with complex numbers

#### configportal.hpp
This library is for the main portal of application(sem.local). Based on our current mode, the device is an access point or not. The token attribute is only used in gateway mode.

#### constants.hpp
Our constants are in this library. We divide the constants into 4 groups. The first group is for pins of the board. The second one has addresses of NVS. The third group has the constants of config portal (sem.local). The last group, which is named others, is for MDNS configuration.

#### gateway.hpp
#### mode1.hpp
#### mode2.hpp
#### mode3.hpp
Same as arduino, esp has 2 main functions: Init and loop. Based on the mode which we are in, the function which is related to that mode is called. The signature of those functions are in .hpp files.

#### page.hpp
This library is for html pages. We have two pages. The first page is for the configuration page (configPage) and the second one will be shown when the configuration is done.

#### server.hpp
The last library is for creating esp asynchronous web server.


## Mode1
First of all we should implement some libraries which are defined in [Includes](#Includes)

![Mode1-onData](/Screenshots/mode1-onData.png)

This function recieves a request as a parameter. Based on that request , creates a response.response is a json file which contains mode, energy and mac. The getEnergy function that fills energy field is in calculate.hpp. Wifi.macAddress is in wifi.h that is a library in constants.hpp
Then , the created json file will be serialized and will be sent as response.

![Mode1-init](/Screenshots/mode1-init.png)

In this function we first set our endpoints. We only have 2 endpoints, so for every other endpoints except “/” and “/data” the result will be 404 not found.
onData function is called when we are in “/data” endpoint.

After creating endpoints, we begin the server.

![Mode1-loop](/Screenshots/mode1-loop.png)

We have 5s delay per loop and in each loop by calling calculateANDwritenergy, the calculation of the energy begins. This function is overwritten in calculate.hpp
