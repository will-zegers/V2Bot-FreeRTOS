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

def platformValidate( ):

   print( "Forward" )
   sendCommand( '72000000460a' ) # platfrom forward
   time.sleep(2)
   sendCommand( '72000000530a' ) # stop platform
   time.sleep(2)

   print( "Right" )
   sendCommand( '72000000520a' ) # platform turn right
   time.sleep(2)
   sendCommand( '72000000530a' ) # stop platform
   time.sleep(2)

   print( "Left" )
   sendCommand( '720000004c0a' ) # platform turn left
   time.sleep(2)
   sendCommand( '72000000530a' ) # stop platform
   time.sleep(2)

   print( "Reverse" )
   sendCommand ('72000000560a' )
   time.sleep(2)
   sendCommand( '72000000530a' ) # stop platform
   time.sleep(2)

def reset():
   reset = '613f2b06530a' # reset to default position
   sendCommand( reset )
   return

def main():
   if ( control == None ):
      print( "No COM ports found" )
      exit()
   else:
      print( "Opening serial on ttyUSB0. Dis gon b gud." )
   reset()
   platformValidate()
   control.close()
   
if __name__ == "__main__":
   main()
