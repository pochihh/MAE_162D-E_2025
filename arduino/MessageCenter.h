#ifndef MESSAGECENTER_H
#define MESSAGECENTER_H

#define DEVICE_ID 0x02
#define MAX_SERIAL_BUFFER_LEN 1024 

// define the TLV types
#define NO_OBJECT_DETECTED 100
#define YOLO_OBJECT_DETECTED 101

// class BBox(ctypes.Structure):
// _fields_ = [("x", ctypes.c_int), ("y", ctypes.c_int), ("w", ctypes.c_int), ("h", ctypes.c_int)]

// class Detection(ctypes.Structure):
// _fields_ = [("object", ctypes.c_int), ("bbox", BBox), ("confidence", ctypes.c_float)]

struct Detection
{
    int32_t object;
    struct BBox
    {
        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;
    } bbox;
    float confidence;
};

// MessageCenter class
class MessageCenter
{
public:
    MessageCenter();
    ~MessageCenter();

    void init();

    // to be called at every tick
    void processingTick();
    
    // function to be called when a message is received
    // static void decodeCallback(enum DecodeErrorCode *error, const struct FrameHeader *frameHeader, struct TlvHeader *tlvHeaders, uint8_t **tlvData);

    // can be called anytime and the message will be queued in the encoder buffer
    void addMessage(uint32_t tlvType, uint32_t tlvLen, const void *dataAddr);

// private:
    TlvDecodeDescriptor decoder;
    TlvEncodeDescriptor encoder;

    byte serialBuffer[MAX_SERIAL_BUFFER_LEN];
    int bytesRead = 0;
    int messageCount = 0;
    
};


#endif // MESSAGECENTER_H