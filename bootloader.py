import time
#import os
import serial
import sys

filename = sys.argv[1]

app_add_range_l = 0x0000
app_add_range_h = 0x2000

bootloader_cmd = {"READ_ID": 0x01, "WRITE_FLASH": 0x04, "CONTINUE": 'C', "STOP": 'F', "RESET": 0x02}

def Parse_hex32(hexfile):

    print('=============== Hello, HEX32 parsing ===============')

    ext_address = 0
    lin_address = 0

    data_memory ={}

    hexfile.seek(0)
    for line in hexfile.readlines():
        byte_count = int(line[1:3], base=16)
        start_address = int(line[3:7], base=16)
        record_type = int(line[7:9], base=16)

        if record_type == 1:  # End record
            pass	
            #print("EOF record")

        elif record_type == 2:  # Extended Segment Address Record
            #print("Extended address")
            lin_address = int(line[9:13], base=16) << 4

        elif record_type == 4:  # Extended Linear Address Record
            #print("Extended linear address")
            ext_address = int(line[9:13], base=16) << 16

        elif record_type == 0:  # Data record
            for i in range(0, int(byte_count / 4)):
               device_memory_address = int((ext_address + start_address + lin_address)/2  + i * 2) # get the real address
               data_bytes = line[9 + i * 8:9 + (i + 1) * 8] #get next 6 bytes and drop phantom, data form is 0xAABBCC
               data =int(data_bytes[6:8] + data_bytes[4:6] + data_bytes[2:4] + data_bytes[0:2], base=16) # adjust endianess
               #print("memory address: %.6x -> data: %.6x" % (device_memory_address, data))
               data_memory[device_memory_address] = data


    return data_memory


if __name__ == "__main__":
    with open(filename) as hexfile:
        data = Parse_hex32(hexfile)
    append_data = bytearray()

    for i in range(app_add_range_l, app_add_range_h, 2):
        if data.has_key(i):
            append_data.append((data[i] >> 16) & 0xFF)
            append_data.append((data[i] >> 8) & 0xFF)
            append_data.append((data[i] ) & 0xFF)
        else:
            append_data.append(0xFF)
            append_data.append(0xFF)
            append_data.append(0xFF)

loop_var = 384 - (len(append_data) % 384)
for i in range(0,loop_var):
    append_data.append(0xFF)

length = len(append_data)
globalrowcount = int(length/384)

ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=1 )
try:
    ser.isOpen()
    print("Serial port is OPEN")
except:
    print("Error")
    quit()

byte_counter = 0
row_counter = 0
if(ser.isOpen()):
    try:
            print("Reading device information")
            ser.write(chr(bootloader_cmd["READ_ID"]))
            time.sleep(0.5)  # Time in seconds.
            print (ser.readline())
            print (ser.readline())
            print (ser.readline())
            time.sleep(0.1)
            print (ser.readline())

            print ("Setting up device for Flash programming")
            ser.write(chr(bootloader_cmd["WRITE_FLASH"]))
            time.sleep(0.5)
            while (ser.readline() != "OK\n"):   # wait until processor acknowledge
                pass

            print ("Device Ready..")
            for i in append_data:
                ser.write(chr(i))
                if byte_counter == 383:
                    row_counter += 1
                    byte_counter = 0
                    #print ("waiting for response")
                    if row_counter == globalrowcount:
                        ser.write((bootloader_cmd["STOP"]))
                        while (ser.readline() != "KO\n"):  # wait until processor acknowledge
                            pass
                    else:
                        ser.write((bootloader_cmd["CONTINUE"]))  # tell micro that we are not finished
                        while (ser.readline() != "OK\n"):  # wait until processor acknowledge
                            pass
                else:
                    byte_counter += 1
            print ("Done programming")
    except Exception:
        print("Error")
        quit()
else:
    print("Cannot open serial port")

