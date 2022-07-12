/*
 * Project name:
     RFid Click (using CR95HF contactless transceiver)
 * Copyright:
     (c) Mikroelektronika, 2013.
 * Revision History:
     20130624:
       - initial release (DO);
 * Description:
     This is a sample program which demonstrates the use of the ST's
     CR95HF contactless transceiver. The CR95HF is used in the applications such 
     as near field communication (NFC) using 13.56 MHz wireless interface.
     In this example, when the RFid tag is put over the RFid Click antenna, 
     the tag ID will be displayed on the Lcd display and sent via UART module.
 * Test configuration:
     MCU:             PIC18F45K22
                      http://ww1.microchip.com/downloads/en/DeviceDoc/41412D.pdf
     Dev.Board:       EasyPIC v7
                      http://www.mikroe.com/easypic/
     Oscillator:      HS-PLL 32.0000 MHz, 8.0000 MHz Crystal
     Ext. Modules:    RFid Click Board - ac:RFid_Click
                      http://www.mikroe.com/click/rfid/
     SW:              mikroC PRO for PIC
                      http://www.mikroe.com/mikroc/pic/
 * NOTES:
     - Place RFid click board in the mikroBUS socket 1 on the EasyPIC v7 board.
     - Place Lcd display in the Lcd display socket and turn on the backlight using the switch SW4.6
     - Short connect RE1 with VCC pin on port headers using jumperwire
 */

// CR95HF Commands Definition
#define    IDN                    0x01
#define    ProtocolSelect         0x02
#define    SendRecv               0x04
#define    Idle                   0x07
#define    RdReg                  0x08
#define    WrReg                  0x09
#define    BaudRate               0x0A
#define    ECHO                   0x55

// RFID Click Connections
sbit SSI_1  at LATA.B2;
sbit SSI_0  at LATE.B1;
sbit IRQ_IN at LATC.B0;
sbit CS     at LATE.B0;

sbit SSI_1_Direction  at TRISA.B2;
sbit SSI_0_Direction  at TRISE.B1;
sbit IRQ_IN_Direction at TRISC.B0;
sbit CS_Direction     at TRISE.B0;
// End RFID Click module connections

// Lcd module connections
sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End Lcd module connections

unsigned short sdata[18];
unsigned short rdata[18];
unsigned short res = 0, dataNum = 0;
unsigned short j = 0, tmp = 0;
char ID[10] = {0};
char txt_hex[3];

// Write command to the CR95HF
void writeCmd(unsigned short cmd, unsigned short dataLen){
  unsigned short i = 0;
  
  CS = 0;
  SPI1_Write(0x00);  // Send cmd to CR95HF
  SPI1_Write(cmd);
  SPI1_Write(dataLen);
  while (dataLen == 0){
    CS = 1;
    break;
  }
  for(i=0; i<dataLen; i++){
    SPI1_Write(sdata[i]);
  }
  CS = 1;
}

// Poll the CR95HF
void readCmd(){
  unsigned short i = 0;

  while(1){
    CS = 0;
    SPI1_Write(0x03);
    res = SPI1_Read(0);
    CS = 1;

    if((res & 0x08) >> 3){
      CS = 0;
      SPI1_Write(0x02);
      res = SPI1_Read(0);
      dataNum = SPI1_Read(0);
      for(i=0; i<dataNum; i++)
        rdata[i] = SPI1_Read(0);
      CS = 1;
      break;
    }
    CS = 1;
    Delay_ms(10);
  }
}

// Calibrate CR95HF device
void Calibration() {
  //TFT_Write_Text("Calibrating CR95HF...", 55, 100);
  sdata[0] = 0x03;
  sdata[1] = 0xA1;
  sdata[2] = 0x00;
  sdata[3] = 0xF8;
  sdata[4] = 0x01;
  sdata[5] = 0x18;
  sdata[6] = 0x00;
  sdata[7] = 0x20;
  sdata[8] = 0x60;
  sdata[9] = 0x60;
  sdata[10] = 0x00;
  sdata[11] = 0x00;
  sdata[12] = 0x3F;
  sdata[13] = 0x01;
  writeCmd(Idle, 0x0E);
  readCmd();

  sdata[11] = 0xFC;
  writeCmd(Idle, 0x0E);
  readCmd();

  sdata[11] = 0x7C;
  writeCmd(Idle, 0x0E);
  readCmd();

  sdata[11] = 0x3C;
  writeCmd(Idle, 0x0E);
  readCmd();

  sdata[11] = 0x5C;
  writeCmd(Idle, 0x0E);
  readCmd();

  sdata[11] = 0x6C;
  writeCmd(Idle, 0x0E);
  readCmd();

  sdata[11] = 0x74;
  writeCmd(Idle, 0x0E);
  readCmd();

  sdata[11] = 0x70;
  writeCmd(Idle, 0x0E);
  readCmd();
}

// Select the RF communication protocol (ISO/IEC 14443-A)
void Select_ISO_IEC_14443_A_Protocol() {
  sdata[0] = 0x02;
  sdata[1] = 0x00;
  writeCmd(ProtocolSelect, 2);
  readCmd();

  // Clear read and write buffers
  for(j=0; j<18; j++ ){
    rdata[j] = 0;
    sdata[j] = 0;
  }
}

// Configure IndexMod & Gain
void IndexMod_Gain() {
  sdata[0] = 0x09;
  sdata[1] = 0x04;
  sdata[2] = 0x68;
  sdata[3] = 0x01;
  sdata[4] = 0x01;
  sdata[5] = 0x50;
  writeCmd(WrReg, 6);
  readCmd();
}

// Configure Auto FDet
void AutoFDet() {
  sdata[0] = 0x09;
  sdata[1] = 0x04;
  sdata[2] = 0x0A;
  sdata[3] = 0x01;
  sdata[4] = 0x02;
  sdata[5] = 0xA1;
  writeCmd(WrReg, 6);
  readCmd();
}

// Read the tag ID
char GetNFCTag() {
  sdata[0] = 0x26;
  sdata[1] = 0x07;
  writeCmd(SendRecv , 2);
  readCmd();

  sdata[0] = 0x93;
  sdata[1] = 0x20;
  sdata[2] = 0x08;
  writeCmd(SendRecv , 3);
  readCmd();

  if(res == 0x80) {
    for( j = 1; j < dataNum - 3; j++) {
      ByteToHex(rdata[j], txt_hex);
      strcat(ID, txt_hex);
      LATD.B0 = 1;
    }
    ID[10] = 0;
    return 1;
  }
  else
  {
    LATD.B0 = 0;
    return 0;
  }
}

// Initialize MCU and peripherals
void MCU_Init(){
  // Configure RFid Click pins
  ANSELA = 0;
  ANSELB = 0;
  ANSELC = 0;
  ANSELD = 0;
  ANSELE = 0;
  SLRCON = 0;
  
  CS_Direction = 0;
  IRQ_IN_Direction = 0;
  SSI_0_Direction  = 0;
  SSI_1_Direction  = 0;
  
  IRQ_IN = 1;
  CS = 1;
  // Set in SPI mode + Jumper wire workaround required, see header notes
  SSI_1 = 0;
  SSI_0 = 1;
  // Initialize SPI module
  SPI1_Init_Advanced(_SPI_MASTER_OSC_DIV16, _SPI_DATA_SAMPLE_MIDDLE, _SPI_CLK_IDLE_LOW, _SPI_LOW_2_HIGH);
  UART1_Init(9600);
  Lcd_Init();                           // Initialize Lcd
  Lcd_Cmd(_LCD_CLEAR);                  // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF);             // Cursor off
  Lcd_Out(1,1,"RFid example");          // Write text in first row
  UART1_Write_Text("RFid example\r\n");
}

// Get Echo reponse from CR95HF
char EchoResponse() {
  CS = 0;
    SPI1_Write(0x00);  // Send cmd to CR95HF
    SPI1_Write(ECHO);
  CS = 1;
  while(1){
    CS = 0;
      SPI1_Write(0x03);
      tmp = SPI1_Read(1);
    CS = 1;

    if((tmp & 0x08) >> 3){
      CS = 0;
      SPI1_Write(0x02);
      tmp = SPI1_Read(1);
      CS = 1;
      if(tmp == ECHO){
        return 1;
      }
      return 0;
    }
  }
}

void main() {
  MCU_Init();                           // Initialize MCU and peripherals
  while (!EchoResponse()) {             // Until CR95HF is detected
    IRQ_IN = 0;                         //   put IRQ_IN pin at low level
    Delay_ms(10);
    IRQ_IN = 1;                         //   put IRQ_IN pin at low level
    Delay_ms(10);
  }
  Lcd_Out(2, 1, "Calibration...");      // Write text in first row
  UART1_Write_Text("Calibration...\r\n");
  // Configure RFid
  Calibration();
  IndexMod_Gain();
  AutoFDet();
  Select_ISO_IEC_14443_A_Protocol();
  // Ready for TAG scanning
  Lcd_Cmd(_LCD_CLEAR);
  Lcd_Out(1, 1, "Tag ID:       ");      // Write text in first row
  UART1_Write_Text("RFid Ready\r\n");
  while(1){
    if(GetNFCTag()){                    // Get tag ID
      Lcd_Out(2,1, &ID);                // Display tag on Lcd
      UART1_Write_Text("Tag ID:");
      UART1_Write_Text(ID);
      UART1_Write(13);
      UART1_Write(10);
      // Clear ID buffer
      for(j=0; j<10; j++){
        ID[j] = 0;
      }
      Delay_ms(500);
    }
  }
}
