#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string

int vDir = 1;
int tDir = 1;

int lastDtemp = 0;
int lastPtemp = 0;
int lastFspeed = 0;
int lastGear = 0;

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(9);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);

  CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_16MHZ);

  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
}

void loop()
{
  if (!digitalRead(CAN0_INT))                        // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)

    if ((rxId & 0x80000000) == 0x80000000)    // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);

    if (rxId == 0x215) {
      if (rxBuf[4] == 0x38) {
        if(lastGear != 1) {
          lastGear = 1;
          Serial.println("DRIVE");
        }
      } else if (rxBuf[4] == 0xB8) {
        if(lastGear != -1) {
          lastGear = -1;
          Serial.println("REVERSE");
        }
      }
    }
    else if (rxId == 0x105) {
      if (rxBuf[0] == 0x09) {
        vDir = 1;
      } else if (rxBuf[0] == 0x08) {
        vDir = -1;
      } else {
        if (rxBuf[0] == 0x0B) {
          vDir = 1;
          Serial.print("VOL:");
          Serial.println(vDir);
        } else if (rxBuf[0] == 0x0A) {
          vDir = -1;
          Serial.print("VOL:");
          Serial.println(vDir);
        }
      }

      if (rxBuf[1] == 0x16) {
        tDir = 1;
      } else if (rxBuf[1] == 0x12) {
        tDir = -1;
      } else {
        if (rxBuf[1] == 0x9E) {
          tDir = 1;
          Serial.print("TUN:");
          Serial.println(tDir);
        } else if (rxBuf[1] == 0x9A) {
          tDir = -1;
          Serial.print("TUN:");
          Serial.println(tDir);
        }
      }
    } else if (rxId == 0x245) {
      if (rxBuf[2] == 0x18) {
        Serial.println("REW:prs");
      } else if (rxBuf[2] == 0x10) {
        Serial.println("REW:rls");
      } if (rxBuf[2] == 0x60) {
        Serial.println("FOR:prs");
      } else if (rxBuf[2] == 0x40) {
        Serial.println("FOR:rls");
      } else if(rxBuf[6] == 0x41 && rxBuf[7] == 0x10) {
        Serial.println("NAV:prs");
      } else if(rxBuf[6] == 0x41 && rxBuf[7] == 0x00) {
        Serial.println("NAV:rls");
        Serial.println("CAM:rls");
        Serial.println("TEL:rls");
      } else if(rxBuf[6] == 0x41 && rxBuf[7] == 0x80) {
        Serial.println("CAM:prs");
      } else if(rxBuf[6] == 0x41 && rxBuf[7] == 0x20) {
        Serial.println("TEL:prs");
      } 
      
      if(rxBuf[0] == 0x08) {
        Serial.println("PWR:prs");
        delay(50);
        Serial.println("PWR:rls");
      }

      if(rxBuf[1] != lastPtemp) {
        lastPtemp = rxBuf[1];
        Serial.print("PT:");
        Serial.println(lastPtemp);
      }

      if(rxBuf[3] != lastFspeed) {
        lastFspeed = rxBuf[3];
        Serial.print("FS:");
        Serial.println(lastFspeed);
      }
    } else if (rxId == 0x195) {
      if(rxBuf[0] == 0xBE) {
        Serial.println("nums_all:rls");
      } else if(rxBuf[0] == 0xA2) {
        Serial.println("1:prs");
      } else if(rxBuf[0] == 0xA4) {
        Serial.println("2:prs");
      } else if(rxBuf[0] == 0xA6) {
        Serial.println("3:prs");
      } else if(rxBuf[0] == 0xA8) {
        Serial.println("4:prs");
      } else if(rxBuf[0] == 0xAA) {
        Serial.println("5:prs");
      } else if(rxBuf[0] == 0xAC) {
        Serial.println("6:prs");
      } else if(rxBuf[0] == 0xAE) {
        Serial.println("7:prs");
      } else if(rxBuf[0] == 0xB0) {
        Serial.println("8:prs");
      } else if(rxBuf[0] == 0xB2) {
        Serial.println("9:prs");
      } else if(rxBuf[0] == 0xB4) {
        Serial.println("*:prs");
      } else if(rxBuf[0] == 0xB6) {
        Serial.println("0:prs");
      } else if(rxBuf[0] == 0xB8) {
        Serial.println("#:prs");
      } 
      
      if(rxBuf[4] == 0x2E) {
        Serial.println("EXIT:prs");
      } else if(rxBuf[4] == 0x2A) {
        Serial.println("EXIT:rls");
      } else if(rxBuf[2] == 0xC0) {
        Serial.println("OK:prs");
      } else if(rxBuf[2] == 0x80) {
        Serial.println("OK:rls");
      }

      if(rxBuf[1] != lastDtemp) {
        lastDtemp = rxBuf[1];
        Serial.print("DT:");
        Serial.println(lastDtemp);
      }
    }
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
