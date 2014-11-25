import sys, serial
try:
   from serial.tools.list_ports import comports
except:
   comports = None

control = serial.Serial("/dev/ttyUSB0", 115200)

reset = '613f517630'
straight = '613e3e3e30'
pwm = '720000A031'
#control.write(straight.decode("hex"))
control.write(reset.decode("hex"))
control.write('\n')

print(control.inWaiting())
control.close()
   
