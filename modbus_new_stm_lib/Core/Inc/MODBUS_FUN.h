

#ifndef INC_MODBUS_FUN_H_
#define INC_MODBUS_FUN_H_

/*********************************Header files***************************************/

#include "UartRingbuffer.h"
#include  "stdio.h"
#include "string.h"
#include "stdlib.h"
#include"declare_datatype.h"


/*********************************variable declaration***************************************/
typedef enum
{
  MModbusCMD_ReadCoilsStatus = 1,       //done
  MModbusCMD_ReadDiscreteInputs = 2,    //done
  MModbusCMD_ReadHoldingRegisters = 3,  //done
  MModbusCMD_ReadInputRegisters = 4,   //done
  MModbusCMD_WriteSingleCoil = 5,        //done
  MModbusCMD_WriteSingleRegister = 6,    //done
  MModbusCMD_WriteMultipleRegisters = 16  //done

}MModbusCMD_t;

typedef enum
{
  MModBus_16bitOrder_AB = 0,
  MModBus_16bitOrder_BA,

}MModBus_16bitOrder_t;


#define _MMODBUS_RXSIZE           64

typedef struct
{
//  uint16_t              rxIndex;
  uint8_t               rxBuf[_MMODBUS_RXSIZE];
//  uint32_t              rxTime;
//  uint8_t               txBusy;
//  uint32_t              timeout;
  MModBus_16bitOrder_t  byteOrder16;
  //MModBus_32bitOrder_t  byteOrder32;

}MModBus_t;

MModBus_t mmodbus;
//uint8_t Rx_buff[50];
uint8_t txData[8];
//uint8_t CRC_success=0;
//uint8_t slaveAddress=0x11;

/********************************Vriable declaration end*****************************************/

/********************************function declaration*****************************************/
uint8_t mmodbus_readHoldingRegisters8i(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data);
uint8_t mmodbus_readCoils(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data);
uint8_t mmodbus_readDiscreteInputs(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data);
uint8_t mmodbus_readInputRegisters8i(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data);

uint8_t mmodbus_writeCoil(uint8_t slaveAddress, uint16_t number, uint8_t data);
uint8_t mmodbus_writeHoldingRegister16i(uint8_t slaveAddress, uint16_t number, uint16_t data);
uint16_t mmodbus_writeHoldingRegisters16i(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint16_t *data);


void TX_FUN(void);
uint8_t Rx_fun(void);
uint16_t CRC16(uint8_t *puchMsg, uint8_t usDataLen);

/********************************function declaration End*****************************************/
uint8_t mmodbus_readCoils(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data)
{
 // uint8_t txData[8];
  txData[0] = slaveAddress;
  txData[1] = MModbusCMD_ReadCoilsStatus;
  txData[2] = (startnumber & 0xFF00) >> 8;
  txData[3] = (startnumber & 0x00FF);
  txData[4] = (length & 0xFF00) >> 8;
  txData[5] = (length & 0x00FF);
  static uint16_t  crc;
  crc = CRC16(txData, 6);
  txData[6] = (crc & 0xFF00) >> 8;
  txData[7] = (crc & 0x00FF);//swapped
  TX_FUN();
  //Rx_fun();
  uint16_t recLen =  Rx_fun();
  if(recLen == 0)
     return false;
  if(mmodbus.rxBuf[0] != slaveAddress)
     return false;
  if(mmodbus.rxBuf[1] != MModbusCMD_ReadCoilsStatus)
    return false;
   crc = CRC16(mmodbus.rxBuf, mmodbus.rxBuf[2] + 3);
  if(((crc & 0x00FF) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 4]) || (((crc & 0xFF00) >> 8) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 3]))
    return false;
  if(data != NULL)
     memcpy(data, &mmodbus.rxBuf[3], mmodbus.rxBuf[2]);
   return true;

}

//##################################################################################################

uint8_t mmodbus_readDiscreteInputs(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data)
{
//  uint8_t txData[8];
  txData[0] = slaveAddress;
  txData[1] = MModbusCMD_ReadDiscreteInputs;
  txData[2] = (startnumber & 0xFF00) >> 8;
  txData[3] = (startnumber & 0x00FF);
  txData[4] = (length & 0xFF00) >> 8;
  txData[5] = (length & 0x00FF);
  static uint16_t  crc;
  crc = CRC16(txData, 6);
  txData[6] = (crc & 0xFF00) >> 8;
  txData[7] = (crc & 0x00FF);//swapped
  TX_FUN();
  //Rx_fun();
  uint16_t recLen =  Rx_fun();
  if(recLen == 0)
	 return false;
  if(mmodbus.rxBuf[0] != slaveAddress)
	 return false;
  if(mmodbus.rxBuf[1] != MModbusCMD_ReadDiscreteInputs)
     return false;
  crc = CRC16(mmodbus.rxBuf, mmodbus.rxBuf[2] + 3);
  //if(((crc & 0x00FF) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 3]) || (((crc & 0xFF00) >> 8) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 4]))
  if(((crc & 0x00FF) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 4]) || (((crc & 0xFF00) >> 8) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 3]))
   return false;
  if(data != NULL)
    memcpy(data, &mmodbus.rxBuf[3], mmodbus.rxBuf[2]);
   return true;



}
//##################################################################################################
uint8_t mmodbus_readInputRegisters8i(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data)
{

  txData[0] = slaveAddress;
  txData[1] = MModbusCMD_ReadInputRegisters;
  txData[2] = (startnumber & 0xFF00) >> 8;
  txData[3] = (startnumber & 0x00FF);
  txData[4] = (length & 0xFF00) >> 8;
  txData[5] = (length & 0x00FF);
  static uint16_t  crc;
  crc = CRC16(txData, 6);
  txData[6] = (crc & 0xFF00) >> 8;
  txData[7] = (crc & 0x00FF);//swapped
  TX_FUN();
  //Rx_fun();
   uint16_t recLen =  Rx_fun();
   if(recLen == 0)
	  return false;
   if(mmodbus.rxBuf[0] != slaveAddress)
  	 return false;
   if(mmodbus.rxBuf[1] != MModbusCMD_ReadInputRegisters)
     return false;
   crc = CRC16(mmodbus.rxBuf, mmodbus.rxBuf[2] + 3);
   if(((crc & 0x00FF) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 4]) || (((crc & 0xFF00) >> 8) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 3]))
   //if(((crc & 0x00FF) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 3]) || (((crc & 0xFF00) >> 8) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 4]))
   return false;
   if(data != NULL)
   memcpy(data, &mmodbus.rxBuf[3], mmodbus.rxBuf[2]);
   return true;
}




//##################################################################################################
uint8_t mmodbus_readHoldingRegisters8i(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint8_t *data)
{

  txData[0] = slaveAddress;
  txData[1] = MModbusCMD_ReadHoldingRegisters;
  txData[2] = (startnumber & 0xFF00) >> 8;
  txData[3] = (startnumber & 0x00FF);
  txData[4] = (length & 0xFF00) >> 8;
  txData[5] = (length & 0x00FF);
  static uint16_t  crc;
  crc = CRC16(txData, 6);
  txData[6] = (crc & 0xFF00) >> 8;
  txData[7] = (crc & 0x00FF);//swapped
  TX_FUN();
  Uart_flush();
 // Rx_fun();
  uint16_t recLen =  Rx_fun();
  if(recLen == 0)
      return false;
  if(mmodbus.rxBuf[0] != slaveAddress)
      return false;
  if(mmodbus.rxBuf[1] != MModbusCMD_ReadHoldingRegisters)
      return false;
  crc = CRC16(mmodbus.rxBuf, mmodbus.rxBuf[2] + 3);
  if(((crc & 0x00FF) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 4]) || (((crc & 0xFF00) >> 8) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 3]))
 // if(((crc & 0x00FF) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 3]) || (((crc & 0xFF00) >> 8) != mmodbus.rxBuf[mmodbus.rxBuf[2] + 4]))
      return false;
  if(data != NULL)
    {
      for(uint8_t i=0 ; i<mmodbus.rxBuf[2] ; i+=2)
      {
        uint8_t H = mmodbus.rxBuf[i+3];
        mmodbus.rxBuf[i+3] = mmodbus.rxBuf[i+3+1];
        mmodbus.rxBuf[i+3+1] = H;
      }
      memcpy(data, &mmodbus.rxBuf[3], mmodbus.rxBuf[2]);
    }
  return true;

}

//##################################################################################################
uint8_t mmodbus_writeCoil(uint8_t slaveAddress, uint16_t number, uint8_t data)
{

  txData[0] = slaveAddress;
  txData[1] = MModbusCMD_WriteSingleCoil;
  txData[2] = (number & 0xFF00) >> 8;
  txData[3] = (number & 0x00FF);
  if(data == 0)
    txData[4] = 0;
  else
    txData[4] = 0xFF;
  txData[5] = 0;
  static uint16_t  crc;
  crc = CRC16(txData, 6);
  txData[6] = (crc & 0xFF00) >> 8;
  txData[7] = (crc & 0x00FF);//swapped
  TX_FUN();
 // Rx_fun();

  uint16_t recLen =  Rx_fun();
  if(recLen == 0)
     return false;
  if(memcmp(txData, mmodbus.rxBuf, 8) == 0)
     return true;
  else
     return false;

}
//##################################################################################################
uint8_t mmodbus_writeHoldingRegister16i(uint8_t slaveAddress, uint16_t number, uint16_t data)
{

  txData[0] = slaveAddress;
  txData[1] = MModbusCMD_WriteSingleRegister;
  txData[2] = (number & 0xFF00) >> 8;
  txData[3] = (number & 0x00FF);
  txData[4] = (data & 0xFF00) >> 8;
  txData[5] = data & 0x00FF;
  static uint16_t  crc;
  crc = CRC16(txData, 6);
  txData[6] = (crc & 0xFF00) >> 8;
  txData[7] = (crc & 0x00FF);//swapped
  TX_FUN();
 // Rx_fun();

  uint16_t recLen =  Rx_fun();
  if(recLen == 0)
     return false;
   if(memcmp(txData, mmodbus.rxBuf, 8) == 0)
     return true;
   else
     return false;

}

//##################################################################################################
uint16_t mmodbus_writeHoldingRegisters16i(uint8_t slaveAddress, uint16_t startnumber, uint16_t length, uint16_t *data)
{
	if (length==1)
	{
		return mmodbus_writeHoldingRegister16i(slaveAddress, startnumber, data[0]);
	}
	else
	{
	  uint8_t txData[7 + length * 2 + 2];
	  txData[0] = slaveAddress;
	  txData[1] = MModbusCMD_WriteMultipleRegisters;
	  txData[2] = (startnumber & 0xFF00) >> 8;
	  txData[3] = (startnumber & 0x00FF);
	  txData[4] = (length & 0xFF00) >> 8;
	  txData[5] = (length & 0x00FF);
	  txData[6] = (length * 2);
	  uint8_t tmp1[2],tmp2[2];
	  for(uint16_t i=0 ; i<length ; i++)
	  {
      switch(mmodbus.byteOrder16)
      {
        case MModBus_16bitOrder_AB:
        memcpy(tmp1, &data[i], 2);
        tmp2[0] = tmp1[1];
        tmp2[1] = tmp1[0];
        memcpy(&txData[7 + i * 2], tmp2, 2);
        break;
        default:
        memcpy(tmp1, &data[i], 2);
        tmp2[0] = tmp1[0];
        tmp2[1] = tmp1[1];
        memcpy(&txData[7 + i * 2], tmp2, 2);
        break;
      }
	  }
	  static uint16_t crc;
	  crc = CRC16(txData, 7 + length * 2);
	  txData[7 + length * 2 + 0] = (crc & 0xFF00) >> 8;//swapped
	  txData[7 + length * 2 + 1] = (crc & 0x00FF);
	  TX_FUN();
	 // Rx_fun();

	  uint16_t recLen =  Rx_fun();
	  if(recLen == 0)
		return false;
	  crc = CRC16(txData, 6);
	  txData[6] = (crc & 0x00FF);
	  txData[7] = (crc & 0xFF00) >> 8;
	  if(memcmp(txData, mmodbus.rxBuf, 8) == 0)
		return true;
	  else
		return false;

	}

}

//##################################################################################################

/**
 @brief Table of CRC values for higher–order byte

 */
static const unsigned char auchCRCHi[] = { 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
		0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 };

/**
 @brief Table of CRC values for low–order byte

 */
static const unsigned char auchCRCLo[] = { 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03,
		0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C,
		0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
		0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE,
		0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17,
		0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30,
		0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35,
		0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
		0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B,
		0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24,
		0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21,
		0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6,
		0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
		0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8,
		0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD,
		0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2,
		0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53,
		0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
		0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59,
		0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E,
		0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47,
		0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40 };

/*
 *  End of CRC values
 */

/*
 *  Below Function of CRC Check
 */

uint16_t CRC16(uint8_t *puchMsg, uint8_t usDataLen) {
	uint8_t uchCRCHi = 0xFF; /* high byte of CRC initialized */
	uint8_t uchCRCLo = 0xFF; /* low byte of CRC initialized */
	uint8_t uIndex; /* will index into CRC lookup table */
	while (usDataLen--) /* pass through message buffer */
	{
		uIndex = uchCRCHi ^ *puchMsg++; /* calculate the CRC */
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	//uint8_t crc = uchCRCHi << 8 | uchCRCLo;
	return (uchCRCHi << 8 | uchCRCLo);
	//return crc;
}

//##################################################################################################
/*
 * Below Function Represents the CRC Calculation of Response frame to be send and Add CRC bytes to the Response Frame
 */

void TX_FUN() {
	//if(GPIO_PIN_3 >=0)
	//{
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	//HAL_Delay(1);
	//}
	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	for (int x = 0; x < 8; x++)                 // response Frame through UART
	{
		Uart_write(txData[x]);
	//printf("%x ",txData[x]);
	}
	//if(GPIO_PIN_3 >=0)
	//{
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	//HAL_Delay(1);
	//}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}


//##################################################################################################
uint8_t Rx_fun() {
	memset(mmodbus.rxBuf, 0, sizeof(mmodbus.rxBuf)); // clear the Buffer for next commmand
	//memset(Resp_buff, 0, sizeof(Resp_buff)); // clear the buffer for next response to be send

	uint8_t command_len = 0;
	while (IsDataAvailable()) {
		//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		mmodbus.rxBuf[command_len] = Uart_read();
		HAL_Delay(100);
		//printf("%x ", Rx_buff[command_len]);
		command_len++;
	}


//	memset(Rx_buff, 0, sizeof(Rx_buff)); // clear the Buffer for next commmand
//		//memset(Resp_buff, 0, sizeof(Resp_buff)); // clear the buffer for next response to be send
//
//		uint8_t command_len = 0;
//		while (IsDataAvailable()) {
//			Rx_buff[command_len] = Uart_read();
//			HAL_Delay(1);
//			//printf("%x ", Rx_buff[command_len]);
//			command_len++;
//		}

	 if(command_len != 0)
		  {
			//  command_len=0;
		//  for(int j=0;mmodbus.rxBuf[j]!='\0';j++)
			for(int j=0;j<15;j++)
					  {
								printf("%x",mmodbus.rxBuf[j]);
					  }
			  printf("\n");
		  }

	  return command_len;
}


#endif /* INC_MODBUS_FUN_H_ */
