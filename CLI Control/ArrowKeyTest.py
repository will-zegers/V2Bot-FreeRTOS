import sys,tty,termios,time

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

def get():
   inkey = _Getch()
   k = inkey()
   while(1):
      if k != '':break
   print(k)

def main():
   for i in range(0,25):
      get()

if __name__=='__main__':
   main()
