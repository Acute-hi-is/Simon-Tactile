# Simon-Tactile

This repository will be used for getting started with the Simon Tactile programmes. 

## 1. Get Python and Arduino
In order to set up your Arduino and Python programmes on your device, have a look at point 2.2 of the report attached in this folder.

## 2. Connecting the keyboard
Connect the keyboard to your USB port, then follow the steps at 3.1 in the Report. After securing the connection, open the "newCode_Serial__1_.ino" arduino code. Then open PyCharm and the "final_code_windows.py". Then go to step 3 and install the necessary drivers for the MADIface. In case you encounter any issues with the audiodevice, you can go to the Acute-procedures repository here on github.

## 3. Running the programme
Open Teensy Loader. Connect the MADIface as explained in step 3.3 in  Press "UPLOAD" on the Arduino app and make sure that the serial monitor is not open. Then go to PyCharm and run the "final_code_windows.py". 

## 4. Using the application
Now that you're all set up, you can start using the app. Place the tactile pad on a participant's abdomen. You are now able to send some commands using the Arduino keyboard. These numbers represent the indexes of the actuators that will vibrate. At the end of each iteration of the code, the participant will have to enter the vibrations they recognized.


