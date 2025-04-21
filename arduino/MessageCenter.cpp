#include <stdint.h>
#include <stdlib.h>
#include <Arduino.h>

// c lib for TLV codec
extern "C"
{
#include "src/tlvcodec.h"
}

#include "MessageCenter.h"

MessageCenter::MessageCenter()
{
    // Initialize the encoder
    initEncodeDescriptor(&encoder, 1024, DEVICE_ID, true);

    // Initialize the decoder; use the callback function to decode the message
    initDecodeDescriptor(&decoder, 1024, true, &MessageCenter::decodeCallback);
}

MessageCenter::~MessageCenter()
{
    // Release the encoder and decoder descriptors
    releaseEncodeDescriptor(&encoder);
    releaseDecodeDescriptor(&decoder);
}

void MessageCenter::init()
{
    // set up serial communication
    Serial1.begin(115200);
}

void MessageCenter::processingTick()
{
    // Use readBytes to read serial data and put any new data into the decoder
    // The callback function will be called when a message packet is ready
    if (Serial1.available() > 0)
    {
        bytesRead = Serial1.readBytes(serialBuffer, min(Serial1.available(), MAX_SERIAL_BUFFER_LEN));
        decode(&decoder, serialBuffer, bytesRead);
    }

    if (messageCount > 0) // There are messaged added to the encoder. Send them out
    {
        wrapupBuffer(&encoder);
        Serial1.write(encoder.buffer, encoder.bufferIndex);
        resetDescriptor(&encoder); // reset the encoder buffer
        messageCount = 0;
    }
}

void MessageCenter::decodeCallback(DecodeErrorCode *error, const FrameHeader *frameHeader, TlvHeader *tlvHeaders, uint8_t **tlvData)
{
    if(*error == NoError)
    {
        // Successfully decoded a message
        // Serial.println("Message received:");
        // Serial.print("Frame Number: ");
        // Serial.println(frameHeader->frameNum);
        // Serial.print("Number of TLVs: ");
        // Serial.println(frameHeader->numTlvs);

        // take actions based on the TLV type
        for (size_t i = 0; i < frameHeader->numTlvs; ++i)
        {
            switch(tlvHeaders[i].tlvType)
            {
                case STOP_SIGN_DETECTION:
                    // Handle stop sign detection
                    Serial.println("Stop sign detected");
                    break;
                case TRAFFIC_LIGHT_DETECTION:
                    // Handle traffic light detection
                    Serial.println("Traffic light detected");
                    break;
                default:
                    Serial.print("Unknown TLV type: ");
                    Serial.println(tlvHeaders[i].tlvType);
                    break;
            }
        }
    }
    else
    {
        Serial.print("Error decoding message: ");
        Serial.println(*error);
    }
}

void MessageCenter::addMessage(uint32_t tlvType, uint32_t tlvLen, const void *dataAddr)
{
    addTlvPacket(&encoder, tlvType, tlvLen, dataAddr);
    messageCount++;
}
