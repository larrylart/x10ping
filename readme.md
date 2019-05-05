
# What is x10ping and why use it?

 Have you ever had a network device such as a router, DSL modem, pc, etc. which will hang every now and then and the only way to get it up and running again was to powercycle it? To make it worst it happens when you are away ?
Then x10ping is your answer. With this application you will also need an 
X10 controller such as CM11A(US)/CM12U(Europe) and x10 appliances  as needed.

 X10Ping is an application written to monitor network devices by pinging them and
when found down over a period of time powercycle them. It's a light application
with a GUI interface and logging capabilities which just does the job.

## Get started:

   Download the binary installation package from here and install it. Currently supported platforms are Windows 98, 2000 and XP. It could works on other windows distributions however I didn't test on it. Also the source code is written platform independent (with wxWidgets) and could probably be compiled on other platforms such as Linux and MacOS, exception CPing class that will have to be re-written.

After you completed the installation, start the x10ping application.

## Configuration:

1 . First, set on which serial port is your X10 controller installed. To do this go to File->Config and there in the first tab(X10) you will find a select option labelled "X10 Serial Port Number:". There chose your serial port number. Once you press "OK" in config windows you should see a message in the logs saying that port was opened successfully. Else, check you serial port configuration.

2. Plug your device to be monitored: router, pc, whatever can be pinged through an X10 appliance module or similar.

3. Press "ADD Device" button. Fill in first, the ip address or host name for your device. Then next select the X10 id of your appliance and press "OK". Repeat this step (and previous) if you have multiple devices. You can also add multiple hosts set to powercycle by the same x10 appliance.

4. You might want to adjust the "loop" parameters. This can be done from menu 
File->Config and there go under "Worker" tab. The parameters and their relevance
are as follow:

   - "Number of Ping Retries" represents how many time should a ping fail before
   we power cycle the device.
   - "Net Loop Check Delay(ms)" is the delay added after checking once all
   devices in the list. This is represented in milliseconds.
   - "Delay After Switching OFF(ms)" is the delay added after the devices is 
   switched off (the count to ten/five cooling down period).
   - "Delay After Switching ON(ms)" is the delay added after we switch back on
   the device. This has to be calculated such to allow the device to boot back on
   and be responsive to ping before we go on with the monitoring loop. This 
   delay and the previous will only apply to the normal monitoring loop and it
   wont be added to the power on/off option from the right click menu.
   
5. After you completed the configuration press the start monitoring button and 
you are set. Failures to ping advice together will a count down with show up in
red in logs. Also the status is shown in list colours and text (red=down, green=up,
blue=powercycle).

6. You can now minimize the application and this will hide the window and add an 
icon on the task bar. To restore the window do double left click on the icon.

   Other functionalities helpful in setup are the two buttons "Clear Device" and 
"Edit Device". The first will remove one or more selected devices from the list.
The second will let you edit the parameters of one selected device.

   Alternatively you can edit by hand the config file found under the application
install folder, ie "c:\Program Files\x10ping\". There are two files:

   - "x10ping.cfg" with the options you found under File->Config. See example:
   
   [X10]
   SerialPort=4
   X10ControllerModel=1
   
   [Network]
   LoopCheckDelay=2000
   DevicePingRetry=2
   X10DelayAfterOFF=5000
   X10DelayAfterON=10000
   
   - "x10ping_dev.cfg" stores your list of devices. See below an example:
   
   [192.168.1.1]
   X10DeviceId=3
   LastKnownNetDeviceState=1
   
   [192.168.3.1]
   X10DeviceId=2
   LastKnownNetDeviceState=4


## Known bugs:

   There are no known bugs at the moment. However, if you find any please send them to me at i@larryo.org. Any suggestions on how to improve this application are also welcome. 

