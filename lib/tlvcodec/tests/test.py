import sys
import os
sys.path.append('../src/')
sys.path.append('../../..')
from encoder import Encoder 
import ctypes

from dataTypes import DataType, get_data_type_struct


# test encoder
encoder = Encoder(199) # device id
encoder.addPacket(DataType.CUSTOM_TYPE_1.value, ctypes.sizeof(get_data_type_struct(DataType.CUSTOM_TYPE_1)), ctypes.c_uint32(111))
encoder.addPacket(DataType.CUSTOM_TYPE_2.value, ctypes.sizeof(ctypes.c_uint32), ctypes.c_uint32(222))
encoder.addPacket(123, 4, ctypes.c_uint32(123))

length, buffer = encoder.wrapupBuffer()

# print the buffer withe the length as hex
print(f'length: {length}')
for i in range(length):
    print(f'{buffer[i]:02d}', end=' ')
print()


def decoder_callback(frameHeader, tlvs):
    print("decoder_callback!")
    pass

# test decoder
decoder = Decoder(decoder_callback) # device id

for i in range(length):
    decoder.decode(buffer[i])

