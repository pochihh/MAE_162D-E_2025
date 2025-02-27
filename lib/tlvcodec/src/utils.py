import ctypes
import struct
from enum import Enum


__all__ = ['FrameHeader', 'TlvHeader', 'NUM_PADDING_BYTES', 'FRAME_HEADER_MAGIC_NUM']
FRAME_HEADER_MAGIC_NUM = b'\x02\x01\x04\x03\x06\x05\x08\x07'

MSG_BUFFER_SEGMENT_LEN = 32

def NUM_PADDING_BYTES(numTotalBytes):
    return MSG_BUFFER_SEGMENT_LEN - (numTotalBytes & (MSG_BUFFER_SEGMENT_LEN - 1))

class FrameHeader(ctypes.Structure):
    _fields_ = [("magicNum", ctypes.ARRAY(ctypes.c_byte, 8)),
                ("numTotalBytes", ctypes.c_uint32),
                ("checksum", ctypes.c_uint32),
                ("deviceId", ctypes.c_uint32),
                ("frameNum", ctypes.c_uint32),
                ("numTlvs", ctypes.c_uint32)]

# define the TLV header structure
class TlvHeader(ctypes.Structure):
    _fields_ = [("tlvType", ctypes.c_uint32),
                ("tlvLen", ctypes.c_uint32)]

