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
//     uint8_t br_to = 0;   // ��ѡ BR_TO = 0
//     uint8_t br_t1 = 234; // ��Ӧ BR_T1 = 234
//     uint8_t serial_speed = (br_to << 5) | br_t1;
//     rfid_uart_write(0x1F, serial_speed); // д�� SerialSpeedReg
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
//     PcdAntennaOn(); // ������
// }

// void rfid_reset()
// {
//     ets_delay_us(4);
//     rfid_uart_write(CommandReg, CMD_SOFIRESET);
//     while (rfid_uart_read(CommandReg) & 0x10) printf("%x \n",rfid_uart_read(CommandReg));
//         ;
//     rfid_uart_read(CommandReg);
//     ets_delay_us(5);

//     rfid_uart_write(ModeReg, 0x3D);       // ���巢�ͺͽ��ճ���ģʽ ��Mifare��ͨѶ��CRC��ʼֵ0x6363
//     rfid_uart_write(TReloadRegL, 0x1e);   // 16λ��ʱ����λ
//     rfid_uart_write(TReloadRegH, 0x00);   // 16λ��ʱ����λ
//     rfid_uart_write(TModeReg, 0x8D);      // �����ڲ���ʱ��������
//     rfid_uart_write(TPrescalerReg, 0x3E); // ���ö�ʱ����Ƶϵ��
//     rfid_uart_write(TxAutoReg, 0x40);     // ���Ʒ����ź�Ϊ100%ASK

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
// // M1����Ϊ16��������ÿ���������ĸ��飨��0����1����2����3�����
// // ��16��������64���鰴���Ե�ַ���Ϊ��0~63
// // ��0�������Ŀ�0�������Ե�ַ0�飩�����ڴ�ų��̴��룬�Ѿ��̻����ɸ��� 
// // ÿ�������Ŀ�0����1����2Ϊ���ݿ飬�����ڴ������
// // ÿ�������Ŀ�3Ϊ���ƿ飨���Ե�ַΪ:��3����7����11.....����������A����ȡ���ơ�����B��

// /*ȫ�ֱ���*/
// unsigned char CT[2];//������
// unsigned char SN[4]; //����
// unsigned char RFID[16];			//���RFID 
// unsigned char total=0;
// uint8_t KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};
// uint8_t AUDIO_OPEN[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
// unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0xff};
// /*��������*/
// unsigned char status;
// unsigned char s=0x08;

// /************************************************************************************************************
//  * ��������CalulateCRC
//  * ����  ����RC522����CRC16
//  * ����  ��pIndata������CRC16������
//  *         ucLen������CRC16�������ֽڳ���
//  *         pOutData����ż�������ŵ��׵�ַ
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
//  * ��������PcdComMF522
//  * ����  ��ͨ��RC522��ISO14443��ͨѶ
//  * ����  ��ucCommand��RC522������
//  *         pInData��ͨ��RC522���͵���Ƭ������
//  *         ucInLenByte���������ݵ��ֽڳ���
//  *         pOutData�����յ��Ŀ�Ƭ��������
//  *         pOutLenBit���������ݵ�λ����
//  * ����  : ״ֵ̬ MI_OK���ɹ�
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
//        case CMD_AUTHENT:		//Mifare��֤
//           ucIrqEn   = 0x12;		//��������ж�����ErrIEn  ��������ж�IdleIEn
//           ucWaitFor = 0x10;		//��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
//           break;
			 
//        case CMD_TRANSCEIVE:		//���շ��� ���ͽ���
//           ucIrqEn   = 0x77;		//����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
//           ucWaitFor = 0x30;		//Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
//           break;
			 
//        default:
//          break;
//     }
//     rfid_uart_write ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴ 
//     ClearBitMask ( ComIrqReg, 0x80 );				//Set1��λ����ʱ��CommIRqReg������λ����
//     rfid_uart_write ( CommandReg, CMD_IDLE );			//д��������
//     SetBitMask ( FIFOLevelReg, 0x80 );				//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����
    
//     for ( ul = 0; ul < ucInLenByte; ul ++ )
// 		  rfid_uart_write ( FIFODataReg, pInData [ ul ] );    	//д���ݽ�FIFOdata
			
//     rfid_uart_write ( CommandReg, ucCommand );					//д����
   
//     if ( ucCommand == CMD_TRANSCEIVE )
// 			SetBitMask(BitFramingReg,0x80);  				//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч
    
//     ul = 1000;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
		
//     do 														//��֤ ��Ѱ���ȴ�ʱ��	
//     {
//          ucN = rfid_uart_read ( ComIrqReg );						//��ѯ�¼��ж�
//          ul --;
//     } 
// 	while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//�˳�����i=0,��ʱ���жϣ���д��������
		
//     ClearBitMask ( BitFramingReg, 0x80 );					//��������StartSendλ
		
//     if ( ul != 0 )
//     {
// 		if ( ! (( rfid_uart_read ( ErrorReg ) & 0x1B )) )			//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
// 		{
// 			cStatus = MI_OK;
// 			if ( ucN & ucIrqEn & 0x01 )					//�Ƿ�����ʱ���ж�
// 			  cStatus = MI_NOTAGERR;   
// 			if ( ucCommand == CMD_TRANSCEIVE )
// 			{
// 				ucN = rfid_uart_read ( FIFOLevelReg );			//��FIFO�б�����ֽ���
// 				ucLastBits = rfid_uart_read ( ControlReg ) & 0x07;	//�����յ����ֽڵ���Чλ��
// 				if ( ucLastBits )
// 					* pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
// 				else
// 					* pOutLenBit = ucN * 8;   					//�����յ����ֽ������ֽ���Ч
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
//  * ��������PcdRequest Ѱ��
//  * ����  ��ucReq_code��Ѱ����ʽ
//  *                   = 0x52��Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//  *                   = 0x26��Ѱδ��������״̬�Ŀ�
//  *         pTagType����Ƭ���ʹ���
//  *                   = 0x4400��Mifare_UltraLight
//  *                   = 0x0400��Mifare_One(S50)
//  *                   = 0x0200��Mifare_One(S70)
//  *                   = 0x0800��Mifare_Pro(X))
//  *                   = 0x4403��Mifare_DESFire
//  * ����  : ״ֵ̬  MI_OK���ɹ�
//  */
// char PcdRequest ( uint8_t ucReq_code, uint8_t * pTagType )
// {
//     char cStatus;  
//     uint8_t ucComMF522Buf [ MAXRLEN ]; 
//     uint32_t ulLen;

//     ClearBitMask ( Status2Reg, 0x08 );	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
//     rfid_uart_write ( BitFramingReg, 0x07 );	//	���͵����һ���ֽڵ� ��λ
//     SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�
//     ucComMF522Buf [ 0 ] = ucReq_code;		//���� ��Ƭ������
//     cStatus = PcdComMF522( CMD_TRANSCEIVE,ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//Ѱ��  
//     if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//Ѱ���ɹ����ؿ����� 
//     {    
//        * pTagType = ucComMF522Buf [ 0 ];
//        * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
//     }
//     else
//      cStatus = MI_ERR;
//     return cStatus;
// }

// /************************************************************************************************************
//  * ��������PcdAnticoll ����ײ��pSnr����Ƭ���кţ�4�ֽ�
//  * ����  : ״ֵ̬ MI_OK���ɹ�
//  */
// char PcdAnticoll ( uint8_t * pSnr )
// {
//     char cStatus;
//     uint8_t uc, ucSnr_check = 0;
//     uint8_t ucComMF522Buf [ MAXRLEN ]; 
// 	uint32_t ulLen;
//     ClearBitMask ( Status2Reg, 0x08 );		//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
//     rfid_uart_write ( BitFramingReg, 0x00);		//����Ĵ��� ֹͣ�շ�
//     ClearBitMask ( CollReg, 0x80 );			//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����
//     ucComMF522Buf [ 0 ] = 0x93;	//��Ƭ����ͻ����
//     ucComMF522Buf [ 1 ] = 0x20;
//     cStatus = PcdComMF522 ( CMD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//�뿨Ƭͨ��
//     if ( cStatus == MI_OK)		//ͨ�ųɹ�
//     {
// 		for ( uc = 0; uc < 4; uc ++ )
//         {
//             * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//����UID
//             ucSnr_check ^= ucComMF522Buf [ uc ];
//         }	
//         if ( ucSnr_check != ucComMF522Buf [ uc ] )
//         		cStatus = MI_ERR;    		 
//     }
//     SetBitMask ( CollReg, 0x80 );
//     return cStatus;
// }

// /************************************************************************************************************
//  * ��������PcdSelect
//  * ����  ��ѡ����Ƭ
//  * ����  ��pSnr����Ƭ���кţ�4�ֽ�
//  * ����  : ״ֵ̬
//  *         = MI_OK���ɹ�
//  * ����  ���ⲿ����
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
//  * ��������PcdAuthState ��֤��Ƭ����
//  * ����  ��ucAuth_mode��������֤ģʽ
//  *                     = 0x60����֤A��Կ
//  *                     = 0x61����֤B��Կ
//  *         uint8_t ucAddr�����ַ
//  *         pKey������
//  *         pSnr����Ƭ���кţ�4�ֽ�
//  * ����  : ״ֵ̬MI_OK���ɹ�
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
//  * ��������PcdRead��ȡM1��һ������  pData�����������ݣ�16�ֽ�
//  * ����  : ״ֵ̬ MI_OK���ɹ�
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

//     status = PcdRequest(PICC_REQALL,CT);//Ѱ��
//     if(status==MI_OK)//Ѱ���ɹ�
//     {
//         printf("search success \n");
//          status=MI_ERR;
//          status = PcdAnticoll(SN);//����ײ	       
//     }
//     if (status==MI_OK)//���nײ�ɹ�
//     {
//         status=MI_ERR;		
//         status =PcdSelect(SN);
//     }
//     if(status==MI_OK)//�x���ɹ�
//     {
//         status=MI_ERR;
//         status =PcdAuthState(0x60,0x09,KEY,SN);
//      }
// 	if(status==MI_OK)//��C�ɹ�
//     {
//         status=MI_ERR;
//         status=PcdRead(s,RFID);
//     }
//     if(status==MI_OK)//�x���ɹ�
//     {
//         status=MI_ERR;
// 		memset(str,0,10);
// 		memcpy(str,RFID,9); //��ʾ����ID��
// 		return 1;
//    }
   
//   return 0;
// }