# Smart Energy Monitor (SEM)
> An implementation of different modes of SEM IOT project.
* [Introduction](#Introduction)
* [Getting Started](#Getting-Started)


## Introduction
The whole project is for measuring and monitoring energy of home devices. It can show some statistics information about electric devices energu usage.
The part that is implemented in this repository is 4 different modes of this project. the SEM device can be operated in 4 different modes.

## Getting-Started
First of you have to connect device into urban electrisity.

![Connect-to-E](/Screenshots/Connect-to-E.png)


Then go to Wifi setting of your phone or computer. Connect to <ConfigMePlease>
![Connect-to-SSID](/Screenshots/Connect-to-SSID.PNG)

Go to your browser and enter <sem.local>
The page containes 3 modes plus 1 gateway mode.
Mode 1,2 is for local mode of device using mobile App. The differene of this 2 modes is in the data which is calculated.
In mode 1 its aggregate energy, but in mode 2 the data is per hour.
In order to work in these modes you have to enter SSID and Password which you want to use for your device. 
In mode 1,2 you device works as an Access Point.
![Mode1](/Screenshots/Mode1.PNG) ![Mode2](/Screenshots/Mode2.PNG)

In the third mode your device works as a node. It should be connected into a SSID and use it as a gateway. So you have to enter SSID and Password of a gateway.
![Mode3](/Screenshots/Mode3.PNG)

The last mode is gateway mode. SEM device is a gateway here. So you should enter SSID and Password of a router or external network.
Here you should enter the token which MobileApp or website gives to you.
![Mode4](/Screenshots/Mode4.PNG)
