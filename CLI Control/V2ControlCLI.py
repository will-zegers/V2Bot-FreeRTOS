import sys, serial, termios, tty, time

SERIAL_PORT = "/dev/ttyUSB0"
BAUD = 115200

try:
   from serial.tools.list_ports import comports
   control = serial.Serial(SERIAL_PORT, BAUD)
except:
   comports = None
   control = None

# Thanks to Newb for the Getch code - http://stackoverflow.com/questions/22397289/finding-the-values-of-the-arrow-keys-in-python-why-are-they-triples
class _Getch:
   def __call__(self):
      fd = sys.stdin.fileno()
      old_settings = termios.tcgetattr(fd)
      try:
         tty.setraw(sys.stdin.fileno())
         ch = sys.stdin.read(1)
      finally:
         termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
         return ch

def sendCommand( cmd ):
   control.write( cmd.decode( "hex" ) )
   return

def manualMove( ):
   inkey = _Getch()

   while(1):
      i = inkey()

      if i == '\x1b':
         j = inkey()

         if j == '[':
            k = inkey()

            if k == 'A':
               sendCommand( '7200000300000a' ) # Move arm up
            elif k == 'B':
               sendCommand( '720000fd00000a' ) # Move arm down
            elif k == 'C':
               sendCommand( '72fd000000000a' ) # Rotate right
            elif k == 'D':
               sendCommand( '7203000000000a' ) # Rotate left
            elif k == '5':
               if inkey() == '~':
                  sendCommand( '7200030400000a' ) # Arm forward
            elif k == '6':
               if inkey() == '~':
                  sendCommand( '7200fdfc00000a' ) # Arm back
      elif i == '[':
         sendCommand( '7200000004000a' )
      elif i == ']':
         sendCommand( '72000000fc000a' )
      elif i == 'a':
         sendCommand( '72000000004C0a' ) # platfrom left 
      elif i == 's':
         sendCommand( '7200000000560a' ) # platform back
      elif i == 'd':
         sendCommand( '7200000000520a' ) # platform turn right
      elif i == 'w':
         sendCommand( '7200000000460a' ) # platform forward
      elif i == ' ':
         sendCommand( '7200000000530a' ) # stop platform
      elif i == 'r':
         reset()
      elif i == 'm':
         maxReach()
      elif i == 'u':
         straightUp()
      elif i == 'q':
         print
         break
      else:
         print("Key not a control")

      time.sleep(0.100)

def reset():
   reset = '617d2b067d530a' # reset to default position
   sendCommand( reset )
   return

def maxReach():
   maxReach = '617da27d7d000a'
   sendCommand( maxReach )
   return

def straightUp():
   maxReach = '617d7d7d7d000a'
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
