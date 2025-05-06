#include "stdafx.h"

// c lib for TLV codec
extern "C"
{
#include "src/tlvcodec.h"
}

#include "MessageCenter.h"

extern MessageCenter RoverGlobalMsg;
extern int StopSignDetected; // Indicate that the stop sign is detected
extern float StopSignDetectedConfidence; // Indicate the confidence of the stop sign detection
extern float RoverGlobalCoordX; // GPS x coordinate
extern float RoverGlobalCoordY; // GPS y coordinate
extern bool  TrafficLightStatus; // 0: red, 1: green

void decodeCallback(DecodeErrorCode *error, const FrameHeader *frameHeader, TlvHeader *tlvHeaders, uint8_t **tlvData)
{
    if (*error == NoError)
    {
        // Successfully decoded a message
        // take actions based on the TLV type
        for (size_t i = 0; i < frameHeader->numTlvs; ++i)
        {
            switch (tlvHeaders[i].tlvType)
            {
            case YOLO_OBJECT_DETECTED:
                
                // tlv length error check
                if (tlvHeaders[i].tlvLen != sizeof(Detection))
                {
                    // log error result to a global variable
                    *error = TlvLenError;
                    return;
                } 

                // copy the data to a local variable
                Detection det = *(Detection *)tlvData[i];
                if (det.object == 2){
                    // stop sign detected
                    // add to global variable
                    StopSignDetected++;
                    if (det.confidence > StopSignDetectedConfidence)
                    {
                        StopSignDetectedConfidence = det.confidence;
                    }
                }
                break;
            case GPS_XY_COORDINATE:
                // tlv length error check
                if (tlvHeaders[i].tlvLen != sizeof(gps_xy_coordinate))
                {
                    // log error result to a global variable
                    *error = TlvLenError;
                    return;
                }

                gps_xy_coordinate gps_coord = *(gps_xy_coordinate *)tlvData[i];
                // Process the GPS coordinates
                RoverGlobalCoordX = gps_coord.x;
                RoverGlobalCoordY = gps_coord.y;
                break;
            case TRAFFIC_LIGHT_STATUS:
                // tlv length error check
                if (tlvHeaders[i].tlvLen != sizeof(traffic_light_status))
                {
                    // log error result to a global variable
                    *error = TlvLenError;
                    return;
                }

                traffic_light_status = *(traffic_light_status *)tlvData[i];
                // Process the GPS coordinates
                TrafficLightStatus = traffic_light_status;
                break;
            default:
                // do nothing
                break;
            }
        }
        // Output messegagejust for debugging
#ifdef __DEBUG__
        for (size_t i = 0; i < frameHeader->numTlvs; ++i)
        {
            switch (tlvHeaders[i].tlvType)
            {
            case YOLO_OBJECT_DETECTED:
                // Handle object detection
                Detection det = *(Detection *)tlvData[i];
                
                // Process the detected object
                Serial.print("Detected object: ");
                Serial.print(det.object);
                Serial.print(", Confidence: ");
                Serial.println(det.confidence);
                break;
            default:
                Serial.print("Unknown TLV type: ");
                Serial.println(tlvHeaders[i].tlvType);
                Serial.print("TLV length: ");
                Serial.println(tlvHeaders[i].tlvLen);
                RoverGlobalMsg.addMessage(tlvHeaders[i].tlvType, tlvHeaders[i].tlvLen, tlvData[i]);
                RoverGlobalMsg.addMessage(tlvHeaders[i].tlvType+1, tlvHeaders[i].tlvLen, tlvData[i]);
                break;
            }
        }
#endif
    }
    else
    {
        // log error result to a global variable

#ifdef __DEBUG__
        Serial.print("Error decoding message: ");
        Serial.println(*error);
        Serial.print("total bytes: ");
        Serial.println(frameHeader->numTotalBytes.value);
#endif
    }
}

MessageCenter::MessageCenter()
{
    // Initialize the encoder
    initEncodeDescriptor(&encoder, 256, DEVICE_ID, true);

    // Initialize the decoder; use the callback function to decode the message
    initDecodeDescriptor(&decoder, 512, true, &decodeCallback);
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
    Serial1.begin(9600);
}

void MessageCenter::processingTick()
{
    // Use readBytes to read serial data and put any new data into the decoder
    // The callback function will be called when a message packet is ready
    if (Serial1.available() > 0)
    {
        bytesRead = Serial1.readBytes(serialBuffer, min(Serial1.available(), MAX_SERIAL_BUFFER_LEN));
        decode(&decoder, serialBuffer, bytesRead);
#ifdef __DEBUG__
        for (int i = 0; i < bytesRead; i++)
        {
            Serial.print(serialBuffer[i]);
            Serial.print(" ");
        }
        Serial.println();
#endif
    }

    if (messageCount > 0) // There are messaged added to the encoder. Send them out
    {
        wrapupBuffer(&encoder);
        Serial1.write(encoder.buffer, encoder.bufferIndex);
        resetDescriptor(&encoder); // reset the encoder buffer
        messageCount = 0;
    }
}

void MessageCenter::addMessage(uint32_t tlvType, uint32_t tlvLen, const void *dataAddr)
{
    addTlvPacket(&encoder, tlvType, tlvLen, dataAddr);
    messageCount++;
}
