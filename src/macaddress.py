#!/usr/bin/python

def getMAC(interface):
  
  # Return the MAC address of interface
  try:
    str = open('/sys/class/net/' + interface + '/address').read()
  except:
    str = "00:00:00:00:00:00"
  return str[0:17]



if __name__ == "__main__":
   print getMAC('enp7s0')

