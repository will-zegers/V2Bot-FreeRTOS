import sys, serial, termios, tty, time

SERIAL_PORT = "/dev/ttyUSB0"
BAUD = 115200

try:
   from serial.tools.list_ports import comports
   control = serial.Serial(SERIAL_PORT, BAUD)
except:
   comports = None
   control = None

def sendCommand( cmd ):
   control.write( cmd.decode( "hex" ) )
   return

def armValidate( ):

   print( "Moving up" )

def reset():
   reset = '613f2b06530a' # reset to default position
   sendCommand( reset )
   return

def maxReach():
   maxReach = '613f513f000a'
   sendCommand( maxReach )
   return

def main():
   if ( control == None ):
      print( "No COM ports found" )
      exit()
   else:
      print( "Opening serial on ttyUSB0. Dis gon b gud." )
   reset()
   manualMove()
   control.close()
   
if __name__ == "__main__":
   main()
