Air Environment INFO collection device with Web-Based IOT
This project implements web-based IOT through the WizFi-360-EVB-Pico to provide real-time air environment information to users on the Web. 


Feuture of project

The project collects temperature, humidity, fine dust (PM2.5), and CO2 concentrations in the air and provides them to users through the web-based IoT.
From anywhere, users can access real-time air environment information in a particular area, their home, or other buildings through a web browser such as Chrome.
Without relying on an external IoT platform, the project realized a web-based IoT by building its own web server based on WizFi360.


Parts List
![image](https://user-images.githubusercontent.com/41229508/199252284-89025619-72b2-426e-bee5-7590a85d1fac.png)

Hardware wiring diagram
![image](https://user-images.githubusercontent.com/41229508/199252240-06cfc910-8148-4bc1-bbb0-ae6366938459.png)
※ The source of WizFi360-EVB-Pico image is https://docs.wiznet.io .

Structure diagram
![image](https://user-images.githubusercontent.com/41229508/199252356-dfc2ded4-bdbd-4a74-9369-4b2e1ac51213.png)

Schematic
![image](https://user-images.githubusercontent.com/41229508/199252403-dcc5768c-1dd0-417a-89e2-6243110b3750.png)


Summary of Code

[ First CORE Loop ]
Every 0.2 Seconds : PM2.5 Sensor Control, RGB LED Control
Evrery Second : Sensor Value Refresh, LCD Print

[ Second CORE Loop ]
WIFI WEB Server working


Web

In the form of a web page, the device provides real-time temperature, humidity, pm2.5, and CO2 information every second.

If the real-time air status changes, the last message will be changed according to the fine dust standard of the Korean Ministry of Environment.
"The concentration of fine dust in the air is Good / Moderate / Unhealty / Very Unhealthy."


■ Web page when fine dust concentration in the air is Moderate.
![image](https://user-images.githubusercontent.com/41229508/199252656-36c721b3-f4f2-42f6-ab55-e23d74cee8c3.png)

■ If you blow a person's breath on the CO2 sensor,
![image](https://user-images.githubusercontent.com/41229508/199252710-94c5a237-6872-46ad-8ecc-ecaf89236bab.png)


Pictures of Project

■ On the LCD screen, Device displays the WIFI SSID, IP and air environment information to the user.
![20221023_213703](https://user-images.githubusercontent.com/41229508/199253035-d90ef5df-f17b-4b2b-a7c0-939cb59bdda9.jpg)
※ If you connect to that IP address through a web browser on the same network, you can view the website above.
※ If you port forward the internal IP on the router, you can access it from anywhere in the world.

■ The color of the RGB LED changes when the real-time air condition changes.
![20221023_213703](https://user-images.githubusercontent.com/41229508/199253207-e7ab9a0e-c4b7-4844-87c8-063256114451.jpg)
![20221023_213417](https://user-images.githubusercontent.com/41229508/199253249-568ae042-87ca-45de-959b-ed04cd42f98e.jpg)
![20221023_213849](https://user-images.githubusercontent.com/41229508/199253281-05826687-f8ba-4205-bf2f-8218155b97a9.jpg)
![20221023_213848](https://user-images.githubusercontent.com/41229508/199253315-579c336e-a112-47ae-b997-4f4b234d68a9.jpg)

Thank you very much to WIZnet for supporting the WizFi360-EVB-Pico hardware and holding the contest.
Thank you for reading my Project article :)
