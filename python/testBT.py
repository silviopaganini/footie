import serial
import time

ser = serial.Serial(
    port='/dev/rfcomm0',
    baudrate=9600,timeout=1
)

time.sleep(5)
#ser.open()

ser.write("send")

while True:
    out = ""
#    while ser.inWaiting() > 0:
#        num = ser.inWaiting()
#        out += ser.read(num)
#        time.sleep(0.4)
#    if out != "":
#        print out
    
    try:
        w =  ser.inWaiting()
        #print w
        if w > 0:
            print ser.readline()
    except Exception:
        continue
#    time.sleep(0.01)
