import sys, serial, termios, tty, time

SERIAL_PORT = "/dev/ttyUSB0"
BAUD        = 115200

try:
   from serial.tools.list_ports import comports
except:
   comports = None
   control = None
#   control = open("/tmp/SerialTest.txt", "w")

control = serial.Serial(SERIAL_PORT, BAUD)
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
   control.write( cmd.decode("hex") )
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
               sendCommand('72000004000a')
            elif k == 'B':
               sendCommand('720000fb000a')
            elif k == 'C':
               sendCommand('72fb0000000a')
            elif k == 'D':
               sendCommand('72040000000a')
            elif k == '5':
               if inkey() == '~':
                  sendCommand('72000400000a')
            elif k == '6':
               if inkey() == '~':
                  sendCommand('7200fc00000a')
      elif ( i == 'a' ):
         sendCommand('720000004c0a')
      elif ( i == 's' ):
         sendCommand('72000000560a')
      elif ( i == 'd' ):
         sendCommand('72000000520a')
      elif ( i == 'w' ):
         sendCommand('72000000460a')
      elif ( i == ' ' ):
         sendCommand('72000000530a')
      elif ( i == 'r' ):
         reset()
      elif ( i == 'm' ):
         maxReach()
      elif ( i == 'q' ):
         print
         break
      else:
         print("Key not a control")
      time.sleep(0.100)

def reset():
   reset = '613f2b06530a'
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
      print("Opening serial on ttyUSB0. Dis gon b gud.")
  
   reset()
   manualMove()
   control.close()

if __name__=="__main__":
   main()
