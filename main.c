/*-------------------------------------------------------
      Data Encryption Standard  56λ��Կ����64λ���� 
				  2011.10
--------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bool.h"   // λ���� 
#include "tables.h"

void BitsCopy(bool *DatOut,bool *DatIn,int Len);  // ���鸴�� 

void ByteToBit(bool *DatOut,char *DatIn,int Num); // �ֽڵ�λ 
void BitToByte(char *DatOut,bool *DatIn,int Num); // λ���ֽ�

void BitToHex(char *DatOut,bool *DatIn,int Num);  // �����Ƶ�ʮ������ 64λ to 4*16�ַ�
void HexToBit(bool *DatOut,char *DatIn,int Num);  // ʮ�����Ƶ������� 

void TablePermute(bool *DatOut,bool *DatIn,const char *Table,int Num); // λ���û����� 
void LoopMove(bool *DatIn,int Len,int Num);     // ѭ������ Len���� Num�ƶ�λ�� 
void Xor(bool *DatA,bool *DatB,int Num);         // ����� 

void S_Change(bool DatOut[32],bool DatIn[48]);   // S�б任 
void F_Change(bool DatIn[32],bool DatKi[48]);    // F����                                  

void SetKey(char KeyIn[8]);                         // ������Կ
void PlayDes(char MesOut[8],char MesIn[8]);       // ִ��DES����
void KickDes(char MesOut[8],char MesIn[8]);             // ִ��DES���� 

 

int main()
{
	int i=0; 
	char MesHex[16]={0};         // 16���ַ��������ڴ�� 64λ16���Ƶ�����
 	char MyKey[8]={0};           // ��ʼ��Կ 8�ֽ�*8
	char YourKey[8]={0};         // ����Ľ�����Կ 8�ֽ�*8
	char MyMessage[8]={0};       // ��ʼ���� 
	FILE *fp, *fpout;
	char str[9] = {0};
	char infilepath[50] = {0};
	char outfilepath[50] = {0};
	char str16[17] = {0};
	char choose;
/*-----------------------------------------------*/

	printf("Input 1 if encrypt, input 2 if decrypt \n");
	scanf("%s", &choose);
	while(choose!='1'&&choose!='2')
	{
		printf("Input 1 if encrypt, input 2 if decrypt \n");
		scanf("%s", &choose);
	}
	getchar();
	
	printf("Input the filepath where content read from:\n");
	gets(infilepath);  
	printf("Input the filepath where save content:\n");
	gets(outfilepath);  
	printf("\n");
	
	if(choose=='1'){
		
		if((fp = fopen(infilepath, "r"))==NULL)  // "D://des.txt"
		{
			printf("can't open read file\n");
			exit(0);
		}
		if((fpout = fopen(outfilepath, "w"))==NULL) //"D://des_out.txt"
		{
			printf("can't open write file\n");
			exit(0);
		}	
	
		printf("Encrypt begin\n"); 
		printf("Please input your Secret Key:\n");
		gets(MyKey);                // ��Կ	
	
		while(MyKey[i]!='\0')        // ������Կ����
		{
			i++;
		}
	
		while(i!=8)                  // ����8 ��ʾ����
		{
			printf("Please input a correct Secret Key!\n");
			gets(MyKey);
			i=0;
			while(MyKey[i]!='\0')    // �ٴμ��
			{
				i++;
			}
		}
	
		SetKey(MyKey);               // ������Կ �õ�����ԿKi	
		
		printf("Your Message is Encrypted!:\n");  // ��Ϣ�Ѽ���
		while(fgets(str,9,fp)!=NULL)
		{
			printf("Every 8 byte in file:\n");
			printf("%s\n", str);
			strncpy(MyMessage, str, 8);
		
			printf("After encrypted:   ");
			PlayDes(MesHex,MyMessage);   // ִ��DES����
			for(i=0;i<16;i++)           
			{
				printf("%c",MesHex[i]);
				fputc(MesHex[i], fpout);
			}
			printf("\n\n");
		}
		fclose(fpout);
		fclose(fp);	
		printf("ALL in file has been encrypted\n");
	}
	
	else if(choose=='2'){
		
		
		printf("Decrypt begin\n"); 
		printf("Please input your Secret Key to Deciphering:\n");  // ��������Կ�Խ���
		gets(YourKey);                                         // �õ���Կ
		SetKey(YourKey);                                       // ������Կ	
		
		if((fp = fopen(infilepath, "r"))==NULL)  // "D://des_out.txt"
		{
			printf("can't open file\n");
			exit(0);
		}	
		if((fpout = fopen(outfilepath, "w"))==NULL)  // D://des_de.txt"
		{
			printf("can't open file\n");
			exit(0);
		}
		
		
		printf("\n");
		while(fgets(str16,17,fp)!=NULL)
		{
			strncpy(MesHex, str16, 16);
			KickDes(MyMessage,MesHex);                     // ���������MyMessage
			for(i=0;i<8;i++)
			{
				printf("%c",MyMessage[i]);
				fputc(MyMessage[i], fpout);
				if(MyMessage[i]=='\n') break;
			}
		}
		printf("\n");		
		printf("Deciphering Over !!\n");                     // ���ܽ���	

//		system("pause");
		
		fclose(fpout);
		fclose(fp);			
		
	}
		
/*------------------------------------------------*/	
    return 0;
}

/*-------------------------------
 ��DatIn��ʼ�ĳ���λLenλ�Ķ�����
 ���Ƶ�DatOut��
--------------------------------*/
void BitsCopy(bool *DatOut,bool *DatIn,int Len)     // ���鸴�� OK 
{
	int i=0;
	for(i=0;i<Len;i++)
	{
		DatOut[i]=DatIn[i];
	}
}

/*-------------------------------
 �ֽ�ת����λ���� 
 ÿ8�λ�һ���ֽ� ÿ��������һλ
 ��1��ȡ���һλ ��64λ 
--------------------------------*/
void ByteToBit(bool *DatOut,char *DatIn,int Num)       // OK
{
	int i=0;
	for(i=0;i<Num;i++)
	{
		DatOut[i]=(DatIn[i/8]>>(i%8))&0x01;   
	}                                       
}

/*-------------------------------
 λת�����ֽں���
 �ֽ�����ÿ8����һλ
 λÿ�������� ����һ�λ�   
---------------------------------*/
void BitToByte(char *DatOut,bool *DatIn,int Num)        // OK
{
	int i=0;
	for(i=0;i<(Num/8);i++)
	{
		DatOut[i]=0;
	} 
	for(i=0;i<Num;i++)
	{
		DatOut[i/8]|=DatIn[i]<<(i%8);	
	}		
}


/*----------------------------------
 ����������ת��Ϊʮ������
 ��Ҫ16���ַ���ʾ
-----------------------------------*/
void BitToHex(char *DatOut,bool *DatIn,int Num)
{
	int i=0;
	for(i=0;i<Num/4;i++)
	{
		DatOut[i]=0;
	}
	for(i=0;i<Num/4;i++)
	{
		DatOut[i] = DatIn[i*4]+(DatIn[i*4+1]<<1)
					+(DatIn[i*4+2]<<2)+(DatIn[i*4+3]<<3);
		if((DatOut[i]%16)>9)
		{
			DatOut[i]=DatOut[i]%16+'7';       //  ��������9ʱ���� 10-15 to A-F
		}                                     //  ����ַ� 
		else
		{
			DatOut[i]=DatOut[i]%16+'0';       //  ����ַ�	   
		}
	}
	
}

/*---------------------------------------------
 ʮ�������ַ�ת������
----------------------------------------------*/
void HexToBit(bool *DatOut,char *DatIn,int Num)
{
	int i=0;                        // �ַ������� 
	for(i=0;i<Num;i++)
	{
		if((DatIn[i/4])>'9')         //  ����9 
		{
			DatOut[i]=((DatIn[i/4]-'7')>>(i%4))&0x01;   			
		}
		else
		{
			DatOut[i]=((DatIn[i/4]-'0')>>(i%4))&0x01; 	
		} 
	}	
}

// ���û�����  OK
void TablePermute(bool *DatOut,bool *DatIn,const char *Table,int Num)  
{
	int i=0;
	static bool Temp[256]={0};
	for(i=0;i<Num;i++)                // NumΪ�û��ĳ��� 
	{
		Temp[i]=DatIn[Table[i]-1];  // ԭ�������ݰ���Ӧ�ı��ϵ�λ������ 
	}
	BitsCopy(DatOut,Temp,Num);       // �ѻ���Temp��ֵ��� 
} 

// ����Կ����λ
void LoopMove(bool *DatIn,int Len,int Num) // ѭ������ Len���ݳ��� Num�ƶ�λ��
{
	static bool Temp[256]={0};    // ����   OK
	BitsCopy(Temp,DatIn,Num);       // ����������ߵ�Numλ(���Ƴ�ȥ��)����Temp 
	BitsCopy(DatIn,DatIn+Num,Len-Num); // ��������߿�ʼ�ĵ�Num����ԭ���Ŀռ�
	BitsCopy(DatIn+Len-Num,Temp,Num);  // ���������Ƴ�ȥ�����ݼӵ����ұ� 
} 

// ��λ���
void Xor(bool *DatA,bool *DatB,int Num)           // �����
{
	int i=0;
	for(i=0;i<Num;i++)
	{
		DatA[i]=DatA[i]^DatB[i];                  // ��� 
	}
} 

// ����48λ ���32λ ��Ri���
void S_Change(bool DatOut[32],bool DatIn[48])     // S�б任
{
	int i,X,Y;                                    // iΪ8��S�� 
	for(i=0,Y=0,X=0;i<8;i++,DatIn+=6,DatOut+=4)   // ÿִ��һ��,��������ƫ��6λ 
	{    										  // ÿִ��һ��,�������ƫ��4λ
		Y=(DatIn[0]<<1)+DatIn[5];                          // af����ڼ���
		X=(DatIn[1]<<3)+(DatIn[2]<<2)+(DatIn[3]<<1)+DatIn[4]; // bcde����ڼ���
		ByteToBit(DatOut,&S_Box[i][Y][X],4);      // ���ҵ��ĵ����ݻ�Ϊ������	
	}
}

// F����
void F_Change(bool DatIn[32],bool DatKi[48])       // F����
{
	static bool MiR[48]={0};             // ����32λͨ��Eѡλ��Ϊ48λ
	TablePermute(MiR,DatIn,E_Table,48); 
	Xor(MiR,DatKi,48);                   // ������Կ���
	S_Change(DatIn,MiR);                 // S�б任
	TablePermute(DatIn,DatIn,P_Table,32);   // P�û������
}



void SetKey(char KeyIn[8])               // ������Կ ��ȡ����ԿKi 
{
	int i=0;
	static bool KeyBit[64]={0};                // ��Կ�����ƴ洢�ռ� 
	static bool *KiL=&KeyBit[0],*KiR=&KeyBit[28];  // ǰ28,��28��56
	ByteToBit(KeyBit,KeyIn,64);                    // ����ԿתΪ�����ƴ���KeyBit 
	TablePermute(KeyBit,KeyBit,PC1_Table,56);      // PC1���û� 56��
	for(i=0;i<16;i++)
	{
		LoopMove(KiL,28,Move_Table[i]);       // ǰ28λ���� 
		LoopMove(KiR,28,Move_Table[i]);	      // ��28λ���� 
	 	TablePermute(SubKey[i],KeyBit,PC2_Table,48); 
	 	// ��ά���� SubKey[i]Ϊÿһ����ʼ��ַ 
	 	// ÿ��һ��λ����PC2�û��� Ki 48λ 
	}		
}

void PlayDes(char MesOut[8],char MesIn[8])  // ִ��DES����
{                                           // �ֽ����� Bin���� Hex��� 
	int i=0;
	static bool MesBit[64]={0};        // ���Ķ����ƴ洢�ռ� 64λ
	static bool Temp[32]={0};
	static bool *MiL=&MesBit[0],*MiR=&MesBit[32]; // ǰ32λ ��32λ 
	ByteToBit(MesBit,MesIn,64);                 // �����Ļ��ɶ����ƴ���MesBit
	TablePermute(MesBit,MesBit,IP_Table,64);    // IP�û� 
	for(i=0;i<16;i++)                       // ����16�� 
	{
		BitsCopy(Temp,MiR,32);            // ��ʱ�洢
		F_Change(MiR,SubKey[i]);           // F�����任
		Xor(MiR,MiL,32);                  // �õ�Ri 
		BitsCopy(MiL,Temp,32);            // �õ�Li 
	}					
	TablePermute(MesBit,MesBit,IPR_Table,64);
	BitToHex(MesOut,MesBit,64);
}

void KickDes(char MesOut[8],char MesIn[8])       // ִ��DES����
{												// Hex���� Bin���� �ֽ���� 
	int i=0;
	static bool MesBit[64]={0};        // ���Ķ����ƴ洢�ռ� 64λ
	static bool Temp[32]={0};
	static bool *MiL=&MesBit[0],*MiR=&MesBit[32]; // ǰ32λ ��32λ
	HexToBit(MesBit,MesIn,64);                 // �����Ļ��ɶ����ƴ���MesBit
	TablePermute(MesBit,MesBit,IP_Table,64);    // IP�û� 
	for(i=15;i>=0;i--)
	{
		BitsCopy(Temp,MiL,32);
		F_Change(MiL,SubKey[i]);
		Xor(MiL,MiR,32);
		BitsCopy(MiR,Temp,32);
	}	
	TablePermute(MesBit,MesBit,IPR_Table,64);
	BitToByte(MesOut,MesBit,64);		
} 








