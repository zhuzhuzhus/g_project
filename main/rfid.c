// #include "rfid.h"
// #include <stdio.h>
// #include <string.h>
// static const char *log = "rfidlog";
// static void _uart_write_and_read(uint8_t w_value, uint8_t *r_value)
// {
//     ESP_LOGD(log, "%s : w_value = %x \n", __func__, w_value);
//     uart_write_bytes(UART_NUM, &w_value, sizeof(uint8_t));
//     ets_delay_us(5*1000);
//     uart_read_bytes(UART_NUM, r_value, sizeof(uint8_t), 1000 / portTICK_PERIOD_MS);
// }
// uint8_t rfid_uart_read(uint8_t addr)
// {
//     uint8_t value;
//     _uart_write_and_read((addr & 0x3f) | 0x80, &value);
//     return value;
// }

// void rfid_uart_write(uint8_t addr, uint8_t value)
// {
//     uint8_t tmp;
//     _uart_write_and_read(addr & 0x3f, &tmp);
//     if (tmp != addr)
//         printf("addr is err addr = 0x%x read = 0x%x!\n",addr,tmp);
//     uart_write_bytes(UART_NUM, &value, sizeof(uint8_t));
// }

// void ClearBitMask(uint8_t ucReg, uint8_t ucMask)
// {
//     uint8_t ucTemp;
//     ucTemp = rfid_uart_read(ucReg);
//     rfid_uart_write(ucReg, ucTemp & (~ucMask)); // clear bit mask
// }

// void SetBitMask ( uint8_t ucReg, uint8_t ucMask )  
// {
//     uint8_t ucTemp;
//     ucTemp = rfid_uart_read ( ucReg );
//     rfid_uart_write ( ucReg, ucTemp | ucMask );         // set bit mask
// }

// void PcdAntennaOn(void)
// {
//     uint8_t uc;
//     uc = rfid_uart_read(TxControlReg);
//     if (!(uc & 0x03))
//         SetBitMask(TxControlReg, 0x03);
// }

// /* set baud rate to 115200*/
// void rfid_set_baudrate()
// {
//     uint8_t br_to = 0;   // 优选 BR_TO = 0
//     uint8_t br_t1 = 234; // 对应 BR_T1 = 234
//     uint8_t serial_speed = (br_to << 5) | br_t1;
//     rfid_uart_write(0x1F, serial_speed); // 写入 SerialSpeedReg
// }

// void rfid_config()
// {

//     ClearBitMask(Status2Reg, 0x08);
//     rfid_uart_write(ModeReg, 0x3D);   // 3F
//     rfid_uart_write(RxSelReg, 0x86);  // 84
//     rfid_uart_write(RFCfgReg, 0x7F);  // 4F
//     rfid_uart_write(TReloadRegL, 30); // tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
//     rfid_uart_write(TReloadRegH, 0);
//     rfid_uart_write(TModeReg, 0x8D);
//     rfid_uart_write(TPrescalerReg, 0x3E);
//     ets_delay_us(2);
//     PcdAntennaOn(); // 开天线
// }

// void rfid_reset()
// {
//     ets_delay_us(4);
//     rfid_uart_write(CommandReg, CMD_SOFIRESET);
//     while (rfid_uart_read(CommandReg) & 0x10) printf("%x \n",rfid_uart_read(CommandReg));
//         ;
//     rfid_uart_read(CommandReg);
//     ets_delay_us(5);

//     rfid_uart_write(ModeReg, 0x3D);       // 定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
//     rfid_uart_write(TReloadRegL, 0x1e);   // 16位定时器低位
//     rfid_uart_write(TReloadRegH, 0x00);   // 16位定时器高位
//     rfid_uart_write(TModeReg, 0x8D);      // 定义内部定时器的设置
//     rfid_uart_write(TPrescalerReg, 0x3E); // 设置定时器分频系数
//     rfid_uart_write(TxAutoReg, 0x40);     // 调制发送信号为100%ASK

// }

// void rfid_init()
// {
//     // rfid_set_baudrate();

//     rfid_reset();
//     // rfid_set_baudrate();
//     rfid_config();
// }




// /*=============================================== RC522 =============================================================*/


// //
// // M1卡分为16个扇区，每个扇区由四个块（块0、块1、块2、块3）组成
// // 将16个扇区的64个块按绝对地址编号为：0~63
// // 第0个扇区的块0（即绝对地址0块），用于存放厂商代码，已经固化不可更改 
// // 每个扇区的块0、块1、块2为数据块，可用于存放数据
// // 每个扇区的块3为控制块（绝对地址为:块3、块7、块11.....）包括密码A，存取控制、密码B等

// /*全局变量*/
// unsigned char CT[2];//卡类型
// unsigned char SN[4]; //卡号
// unsigned char RFID[16];			//存放RFID 
// unsigned char total=0;
// uint8_t KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};
// uint8_t AUDIO_OPEN[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
// unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0xff};
// /*函数声明*/
// unsigned char status;
// unsigned char s=0x08;

// /************************************************************************************************************
//  * 函数名：CalulateCRC
//  * 描述  ：用RC522计算CRC16
//  * 输入  ：pIndata，计算CRC16的数组
//  *         ucLen，计算CRC16的数组字节长度
//  *         pOutData，存放计算结果存放的首地址
//  */
// void CalulateCRC ( uint8_t * pIndata, uint8_t ucLen, uint8_t * pOutData )
// {
//     uint8_t uc, ucN;
//     ClearBitMask(DivIrqReg,0x04);
//     rfid_uart_write(CommandReg,CMD_IDLE);
//     SetBitMask(FIFOLevelReg,0x80);
//     for ( uc = 0; uc < ucLen; uc ++)
// 	    rfid_uart_write ( FIFODataReg, * ( pIndata + uc ) );   
//     rfid_uart_write ( CommandReg, CMD_CALCCRC );
//     uc = 0xFF;
//     do 
//     {
//         ucN = rfid_uart_read ( DivIrqReg );
//         uc --;
//     } while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );
		
//     pOutData [ 0 ] = rfid_uart_read ( CRCResultRegL );
//     pOutData [ 1 ] = rfid_uart_read ( CRCResultRegM );
// }


// /**************************************************************************************
//  * 函数名：PcdComMF522
//  * 描述  ：通过RC522和ISO14443卡通讯
//  * 输入  ：ucCommand，RC522命令字
//  *         pInData，通过RC522发送到卡片的数据
//  *         ucInLenByte，发送数据的字节长度
//  *         pOutData，接收到的卡片返回数据
//  *         pOutLenBit，返回数据的位长度
//  * 返回  : 状态值 MI_OK，成功
//  */
// char PcdComMF522 ( uint8_t ucCommand, uint8_t * pInData, uint8_t ucInLenByte, uint8_t * pOutData, uint32_t * pOutLenBit )		
// {
//     char cStatus = MI_ERR;
//     uint8_t ucIrqEn   = 0x00;
//     uint8_t ucWaitFor = 0x00;
//     uint8_t ucLastBits;
//     uint8_t ucN;
//     uint32_t ul;
//     switch ( ucCommand )
//     {
//        case CMD_AUTHENT:		//Mifare认证
//           ucIrqEn   = 0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
//           ucWaitFor = 0x10;		//认证寻卡等待时候 查询空闲中断标志位
//           break;
			 
//        case CMD_TRANSCEIVE:		//接收发送 发送接收
//           ucIrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
//           ucWaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
//           break;
			 
//        default:
//          break;
//     }
//     rfid_uart_write ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反 
//     ClearBitMask ( ComIrqReg, 0x80 );				//Set1该位清零时，CommIRqReg的屏蔽位清零
//     rfid_uart_write ( CommandReg, CMD_IDLE );			//写空闲命令
//     SetBitMask ( FIFOLevelReg, 0x80 );				//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
    
//     for ( ul = 0; ul < ucInLenByte; ul ++ )
// 		  rfid_uart_write ( FIFODataReg, pInData [ ul ] );    	//写数据进FIFOdata
			
//     rfid_uart_write ( CommandReg, ucCommand );					//写命令
   
//     if ( ucCommand == CMD_TRANSCEIVE )
// 			SetBitMask(BitFramingReg,0x80);  				//StartSend置位启动数据发送 该位与收发命令使用时才有效
    
//     ul = 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms
		
//     do 														//认证 与寻卡等待时间	
//     {
//          ucN = rfid_uart_read ( ComIrqReg );						//查询事件中断
//          ul --;
//     } 
// 	while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//退出条件i=0,定时器中断，与写空闲命令
		
//     ClearBitMask ( BitFramingReg, 0x80 );					//清理允许StartSend位
		
//     if ( ul != 0 )
//     {
// 		if ( ! (( rfid_uart_read ( ErrorReg ) & 0x1B )) )			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
// 		{
// 			cStatus = MI_OK;
// 			if ( ucN & ucIrqEn & 0x01 )					//是否发生定时器中断
// 			  cStatus = MI_NOTAGERR;   
// 			if ( ucCommand == CMD_TRANSCEIVE )
// 			{
// 				ucN = rfid_uart_read ( FIFOLevelReg );			//读FIFO中保存的字节数
// 				ucLastBits = rfid_uart_read ( ControlReg ) & 0x07;	//最后接收到得字节的有效位数
// 				if ( ucLastBits )
// 					* pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
// 				else
// 					* pOutLenBit = ucN * 8;   					//最后接收到的字节整个字节有效
// 				if ( ucN == 0 )	
//                     ucN = 1;    
// 				if ( ucN > MAXRLEN )
// 					ucN = MAXRLEN;   
// 				for ( ul = 0; ul < ucN; ul ++ )
// 				  pOutData [ ul ] = rfid_uart_read ( FIFODataReg );   
// 			}		
//         }
// 			else
// 				cStatus = MI_ERR;   
// 			//printf("ErrorReg=%d\r\n",ErrorReg);
//     }
//    SetBitMask ( ControlReg, 0x80 );           // stop timer now
//    rfid_uart_write ( CommandReg, CMD_IDLE ); 
//    return cStatus;
// }

// /*********************************************************************************************************************
//  * 函数名：PcdRequest 寻卡
//  * 输入  ：ucReq_code，寻卡方式
//  *                   = 0x52，寻感应区内所有符合14443A标准的卡
//  *                   = 0x26，寻未进入休眠状态的卡
//  *         pTagType，卡片类型代码
//  *                   = 0x4400，Mifare_UltraLight
//  *                   = 0x0400，Mifare_One(S50)
//  *                   = 0x0200，Mifare_One(S70)
//  *                   = 0x0800，Mifare_Pro(X))
//  *                   = 0x4403，Mifare_DESFire
//  * 返回  : 状态值  MI_OK，成功
//  */
// char PcdRequest ( uint8_t ucReq_code, uint8_t * pTagType )
// {
//     char cStatus;  
//     uint8_t ucComMF522Buf [ MAXRLEN ]; 
//     uint32_t ulLen;

//     ClearBitMask ( Status2Reg, 0x08 );	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
//     rfid_uart_write ( BitFramingReg, 0x07 );	//	发送的最后一个字节的 七位
//     SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
//     ucComMF522Buf [ 0 ] = ucReq_code;		//存入 卡片命令字
//     cStatus = PcdComMF522( CMD_TRANSCEIVE,ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//寻卡  
//     if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//寻卡成功返回卡类型 
//     {    
//        * pTagType = ucComMF522Buf [ 0 ];
//        * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
//     }
//     else
//      cStatus = MI_ERR;
//     return cStatus;
// }

// /************************************************************************************************************
//  * 函数名：PcdAnticoll 防冲撞：pSnr，卡片序列号，4字节
//  * 返回  : 状态值 MI_OK，成功
//  */
// char PcdAnticoll ( uint8_t * pSnr )
// {
//     char cStatus;
//     uint8_t uc, ucSnr_check = 0;
//     uint8_t ucComMF522Buf [ MAXRLEN ]; 
// 	uint32_t ulLen;
//     ClearBitMask ( Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
//     rfid_uart_write ( BitFramingReg, 0x00);		//清理寄存器 停止收发
//     ClearBitMask ( CollReg, 0x80 );			//清ValuesAfterColl所有接收的位在冲突后被清除
//     ucComMF522Buf [ 0 ] = 0x93;	//卡片防冲突命令
//     ucComMF522Buf [ 1 ] = 0x20;
//     cStatus = PcdComMF522 ( CMD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//与卡片通信
//     if ( cStatus == MI_OK)		//通信成功
//     {
// 		for ( uc = 0; uc < 4; uc ++ )
//         {
//             * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//读出UID
//             ucSnr_check ^= ucComMF522Buf [ uc ];
//         }	
//         if ( ucSnr_check != ucComMF522Buf [ uc ] )
//         		cStatus = MI_ERR;    		 
//     }
//     SetBitMask ( CollReg, 0x80 );
//     return cStatus;
// }

// /************************************************************************************************************
//  * 函数名：PcdSelect
//  * 描述  ：选定卡片
//  * 输入  ：pSnr，卡片序列号，4字节
//  * 返回  : 状态值
//  *         = MI_OK，成功
//  * 调用  ：外部调用
//  */
// char PcdSelect ( uint8_t * pSnr )
// {
//     char 	ucN;
//     uint8_t 		uc;
// 	uint8_t 		ucComMF522Buf [ MAXRLEN ]; 
//     uint32_t  	ulLen;
//     ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;
//     ucComMF522Buf [ 1 ] = 0x70;
//     ucComMF522Buf [ 6 ] = 0;
//     for ( uc = 0; uc < 4; uc ++ )
//     {
//     	ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
//     	ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
//     }	
//     CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );
//     ClearBitMask ( Status2Reg, 0x08 );
//     ucN = PcdComMF522 ( CMD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );
//     if ( ( ucN == MI_OK ) && ( ulLen == 0x18 ) )
//       ucN = MI_OK;  
//     else
//       ucN = MI_ERR;    
//     return ucN;
// }

// /************************************************************************************************************
//  * 函数名：PcdAuthState 验证卡片密码
//  * 输入  ：ucAuth_mode，密码验证模式
//  *                     = 0x60，验证A密钥
//  *                     = 0x61，验证B密钥
//  *         uint8_t ucAddr，块地址
//  *         pKey，密码
//  *         pSnr，卡片序列号，4字节
//  * 返回  : 状态值MI_OK，成功
//  */
// char PcdAuthState ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr )
// {
//     char cStatus;
// 	uint8_t uc, ucComMF522Buf [ MAXRLEN ];
//     uint32_t ulLen;
//     ucComMF522Buf [ 0 ] = ucAuth_mode;
//     ucComMF522Buf [ 1 ] = ucAddr;
//     for ( uc = 0; uc < 6; uc ++ )
// 	    ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );   
//     for ( uc = 0; uc < 6; uc ++ )
// 	    ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );   
//     cStatus = PcdComMF522 ( CMD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );
//     if ( ( cStatus != MI_OK ) || ( ! ( rfid_uart_read ( Status2Reg ) & 0x08 ) ) )
// 		{
// 			if(cStatus != MI_OK)
// 				printf("666");		
// 			else
// 				printf("888");
// 			cStatus = MI_ERR; 
// 		}
//     return cStatus;
// }

// /**********************************************************************************************
//  * 函数名：PcdRead读取M1卡一块数据  pData，读出的数据，16字节
//  * 返回  : 状态值 MI_OK，成功
//  */
// char PcdRead ( uint8_t ucAddr, uint8_t * pData )
// {
//     char cStatus;
// 	uint8_t uc, ucComMF522Buf [ MAXRLEN ]; 
//     uint32_t ulLen;

//     ucComMF522Buf [ 0 ] = PICC_READ;
//     ucComMF522Buf [ 1 ] = ucAddr;
//     CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
//     cStatus = PcdComMF522 (CMD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen);
//     if ( ( cStatus == MI_OK ) && ( ulLen == 0x90 ) )
//     {
// 		for ( uc = 0; uc < 16; uc ++ )
//         * ( pData + uc ) = ucComMF522Buf [ uc ];   
//     }
//     else
//       cStatus = MI_ERR;   
//     return cStatus;
// }

// bool RC522_READ(uint8_t * str)
// {
//     unsigned char status;

//     status = PcdRequest(PICC_REQALL,CT);//寻卡
//     if(status==MI_OK)//寻卡成功
//     {
//         printf("search success \n");
//          status=MI_ERR;
//          status = PcdAnticoll(SN);//防冲撞	       
//     }
//     if (status==MI_OK)//防衝撞成功
//     {
//         status=MI_ERR;		
//         status =PcdSelect(SN);
//     }
//     if(status==MI_OK)//選卡成功
//     {
//         status=MI_ERR;
//         status =PcdAuthState(0x60,0x09,KEY,SN);
//      }
// 	if(status==MI_OK)//驗證成功
//     {
//         status=MI_ERR;
//         status=PcdRead(s,RFID);
//     }
//     if(status==MI_OK)//讀卡成功
//     {
//         status=MI_ERR;
// 		memset(str,0,10);
// 		memcpy(str,RFID,9); //显示卡的ID号
// 		return 1;
//    }
   
//   return 0;
// }