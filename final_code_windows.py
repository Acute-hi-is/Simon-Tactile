# final code 22.03.2024 12:37

import serial
# Configure serial port
ser = serial.Serial('COM3', 9600)

# Save commands to a text file
file_name = "simons_tactile.txt"

# Read an array of integers from the keyboard, one element at a time
print("How many sensors do you want to vibrate?")
n = int(ser.readline().decode().strip())
print(n)
array = []

print("Enter the elements one by one:")
for _ in range(n):
    element = int(ser.readline().decode().strip())
    array.append(element)
    print(element)

print("Commands read from keyboard:", array)

# ivan test code

# Finnur Pind 18.03.2021
# Pilot test for audio-tactile experiment using the L5 display
# !/usr/bin/env python3
"""Play a sine signal."""
import argparse
import sys
import numpy as np
import sounddevice as sd
import time
import msvcrt
import keyboard as kb


########## READ INPUT DATA ###########
# Mainly which audio device to use...
def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text


parser = argparse.ArgumentParser(add_help=False)
parser.add_argument(
    '-l', '--list-devices', action='store_true',
    help='show list of audio devices and exit')
args, remaining = parser.parse_known_args()
if args.list_devices:
    print(sd.query_devices())
    parser.exit(0)
parser = argparse.ArgumentParser(
    description=__doc__,
    formatter_class=argparse.RawDescriptionHelpFormatter,
    parents=[parser])
parser.add_argument(
    '-d', '--device', type=int_or_str,
    help='output device (numeric ID or substring)')
args = parser.parse_args(remaining)

f = 0
a = 0
start_idx = 0


############################################################


# Playback function
def callback(outdata, frames, time, status):
    if status:
        print(status, file=sys.stderr)
    global start_idx
    t = (start_idx + np.arange(frames)) / samplerate
    t = t.reshape(-1, 1)  # Change from row vector to column vector?

    # To play the same sine wave to both output channels use this:
    outdata[:] = a * np.sin(2 * np.pi * f * t)  # Compute the sine wave
    start_idx += frames


samplerate = sd.query_devices(args.device, 'output')['default_samplerate']

# Close the database connection

########## EXPERIMENT PARAMETERS #########
##CHANGE EVERY TIME
participantID = 1

nTrials = 4  # Number of iterations per condition, should be divisible by 4 (the code assumes 4 blocks of trials, i.e. 3 breaks
nTestTrials = 1  # Number of iterations per condition
doStartupTest = 1  # Play a signal through all motors to make sure everything is working
stimuliDuration = 0.2  # duration of stimulis (in secs)
delayBetweenStimuli = 0.05  # delay between stimuli 1 and stimuli 2
responseTimeWindow = 5  # time window for response
delayAfterResponse = 0.5  # time after response is given until next is played
###########################################


if doStartupTest:
    print('Running startup test...')
    nCoils = 32
    # all_channels = list(range(nCoils))
    f = 100
    a = 0.1
    i = 0
    noMotors = int(n-1)
    # we want n motors to vibrate separately, not altogether

if doStartupTest:
    print('Running startup test...')
    nCoils = 32
    # all_channels = list(range(nCoils))
    f = 100
    a = 0.1
    i = 0
    noMotors = int(n-1)
    # we want n motors to vibrate separately, not altogether


for element in array:
    print('Startup test, motor:', element)

    asio_ch = sd.AsioSettings(channel_selectors=[element])
    # start_idx = 0

    with sd.OutputStream(device=8, channels=1, callback=callback,
                          samplerate=samplerate, extra_settings=asio_ch):
        time.sleep(1)
        sd.stop()
file_name = "simons_tactile.txt"

# Read an array of integers from the keyboard, one element at a time
print("Where did you feel the vibrations? Please enter them in order.")
arrayResults= []

print("Enter the elements one by one:")
for _ in range(n):
    element2 = int(ser.readline().decode().strip())
    arrayResults.append(element2)
    print(element2)

print("You felt the vibrations in the following order:", arrayResults)


result = array == arrayResults
if (result == True):
    print ('You are right!')
if (result == False):
    print ('You are wrong!')

with open(file_name, 'w') as file:
    file.write("Commands read from keyboard:\n")
    for element in array:
        file.write(f"{element}\n")
    file.write("Vibrations felt by the user:\n")
    for element2 in arrayResults:
        file.write(f"{element2}\n")
    file.write(f"{result}\n")
print(f"Results have been saved to {file_name}")