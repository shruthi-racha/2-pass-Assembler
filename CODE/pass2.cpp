
// PASS 2 of Assembler...

#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "pass1.cpp"
//#include<conio.h>
unsigned char hexcode[6];
int opno;
FILE *lstf,*symbf;

int il;

// The Opcode / mnemonic Table
/*
class userDefinedVar {
public:
	char varName[100];
	char varType[100];
};

userDefinedVar ud[20];
*/
class optable{
public:
	char name[10];
	unsigned char opcode1;
};

class optable optab[]=
{
	"aaa",0x37,
		"aad",0xD5,"aam",0xD4,
		"aas",0x3F,"cbw",0x98,
		"clc",0xF8,"cld",0xFC,
		"cli",0xFA,"cmc",0xF5,
		"cmpsb",0xA6,"cmpsw",0xA7,
		"cwd", 0x99,"daa", 0x27,
		"das", 0x2F,"hlt", 0xF4,
		"int3", 0xcc,"iret", 0xCF,
		"into", 0xCE,"lahf", 0x9F,
		"lock", 0xF0,"lodsb",0xAC,
		"lodsw", 0xAD,"leave",0xc9,
		"movsb", 0xA4,"movsw", 0xA5,
		"nop", 0x90,"popf", 0x9D,
		"pushf", 0x9C,"rep", 0xF3,
		"repz" , 0xF3,"repe", 0xF3, 
		"repnz", 0xF2,"repne", 0xF2,
		"ret", 0xc3,"retf",0xca,
		"sahf", 0x9E,"scasb", 0xAE,
		"scasw", 0xAF,"stc", 0xF9,
		"std", 0xFD,"sti", 0xFB,
		"stosb", 0xAA,"stosw",0xAB,
		"wait", 0x9B,"xlat", 0xD7,
		"jo", 0x70,"jno", 0x71,
		"jb", 0x71,"jnae", 0x72,
		"jae", 0x73,"jnb",  0x73,
		"je", 0x74,	"jz",   0x74,
		"jne",  0x75,"jnz",  0x75,
		"jbe",  0x76,"jna", 0x76,
		"ja", 0x77,"jnbe", 0x77,
		"js", 0x78,"jns", 0x79,
		"jp", 0x7A,	"jpe",  0x7A, 
		"jnp",  0x7B,"jpo",  0x7B,
		"jl",  0x7C,"jnge",  0x7C,
		"jge",  0x7D,"jnl", 0x7D,
		"jle",  0x7E,"jng",  0x7E,
		"jg",  0x7F,"jnle", 0x7F,
		"jcxz",  0xE3,"loop",  0xE2,
		"loope",  0xE1,	"loopz", 0xE1,
		"loopne" ,0xE0,	"loopnz",0xE0,
		"jmp", 0xEb, "call", 0xE8,
		"push", 0x06,"pop", 0x07,
		"neg", 0xF6,"not", 0xF6,
		"dec", 0xFE,"inc", 0xFE,
		"div", 0xF6,"idiv",  0xF6,
		"mul",  0xF6,"imul", 0xF6,
		"adc", 0x14,"add", 0x04, 
		"and", 0x24,"cmp", 0x3C,
		"or",  0x0C,"sbb", 0x1C,
		"sub",  0x2C,"test",  0xA8,
		"xor", 0x34,"rcl",0xD0,
		"rcr",0xD0,"rol",0xD0,
		"ror",0xD0,"sal", 0xD0,
		"sar", 0xD0,"shl", 0xD0,
		"shr", 0xD0,"in", 0xec,
		"out", 0xee,"xchg",0x86,
		"lds", 0xc5,"lea", 0x8d,
		"les",0xc4
};

//Search Optab for opcode and set opno
void seaOpt(int a,int b)
{
	for(register int i=a;i<b;i++)
	{
		if(!strcmp(Line[il].word[0],optab[i].name))
		{
			opno=i;
			return;
		}
	}
}

void sfputc(int ch,FILE *fptr)
{
	fprintf(lstf,"%02X ",int(ch));
	fputc(ch,objmem);
}

//Get register value
int regval(char reg1[5])
{
	if(strcmp(reg1,"al")==0){return 0;}
	else if(strcmp(reg1,"cl")==0){return 1;}
	else if(strcmp(reg1,"dl")==0){return 2;}
	else if(strcmp(reg1,"bl")==0){return 3;}
	else if(strcmp(reg1,"ah")==0){return 4;}
	else if(strcmp(reg1,"ch")==0){return 5;}
	else if(strcmp(reg1,"dh")==0){return 6;}
	else if(strcmp(reg1,"bh")==0){return 7;}

	else if(strcmp(reg1,"ax")==0){return 0;}
	else if(strcmp(reg1,"cx")==0){return 1;}
	else if(strcmp(reg1,"dx")==0){return 2;}
	else if(strcmp(reg1,"bx")==0){return 3;}
	else if(strcmp(reg1,"sp")==0){return 4;}
	else if(strcmp(reg1,"bp")==0){return 5;}
	else if(strcmp(reg1,"si")==0){return 6;}
	else if(strcmp(reg1,"di")==0){return 7;}

	else if(strcmp(reg1,"es")==0){return 0;}
	else if(strcmp(reg1,"cs")==0){return 1;}
	else if(strcmp(reg1,"ss")==0){return 2;}
	else if(strcmp(reg1,"ds")==0){return 3;}
	else return -1;
}
 //Component function for Encoding Instructions
void coding(int code)
{
	int hex;

	if(Line[il].mod==3)//mode 3 (reg,reg)
	{
		hexcode[1]= hexcode[1]|(Line[il].mod<<6);
		hexcode[1]= hexcode[1]|(regval(Line[il].word[1])<<3);
		hexcode[1]= hexcode[1]|(regval(Line[il].word[3]));
	}

	if(Line[il].mod==0 || Line[il].mod==2 || Line[il].mod==1)//modes 0 and 2
	{
		if(Line[il].mod==1)
			hexcode[1]=hexcode[1]|64;
		else if(Line[il].mod==2)
			hexcode[1]=hexcode[1]|128;
		else if(Line[il].mod==0)
			hexcode[1]=hexcode[1]|0;

		if(code==1)
		{
			if(Line[il].op1.reg8==1)
				hexcode[1]= hexcode[1]|(regval(Line[il].word[1])<<3);
			if(Line[il].op2.reg8==1)
				hexcode[1]= hexcode[1]|(regval(Line[il].word[3])<<3);
			if(Line[il].op1.reg16==1)
				hexcode[1]= hexcode[1]|(regval(Line[il].word[1])<<3);
			if(Line[il].op2.reg16==1)
				hexcode[1]= hexcode[1]|(regval(Line[il].word[3])<<3);
			else
				hexcode[1]= hexcode[1]|0;
		}

		if(Line[il].index)
		{
			if(Line[il].base)
			{
				if(Line[il].base==1 && Line[il].index==1)
					hex=0x00;
				else if(Line[il].base==1 && Line[il].index==2)
					hex=0x01;
				else if(Line[il].base==2 && Line[il].index==1)
					hex=0x02;
				else if(Line[il].base==2 && Line[il].index==2)
					hex=0x03;
			}//both si/di and bx/bp

			else
			{
				if(Line[il].index==1)
					hex=0x04;
				else if(Line[il].index==2)
					hex=0x05;
			}//only index
		}

		else if(Line[il].base)
		{
			if(Line[il].base==1)
				hex=0x06;
			if(Line[il].base==2)
				hex=0x07;
		}//was for only base


		if(Line[il].var==1)
		{
			for(int k=0;k<symnum;k++)//memory reference...
			{
				if(strcmp(Line[il].word[3],symEntry[k].name)==0 || strcmp(Line[il].word[1],symEntry[k].name)==0)
				{
					hex=0x06;
					hexcode[2]=symEntry[k].addr;
					hexcode[3]=symEntry[k].addr/256;

				}
			}
		}

		hexcode[1]=hexcode[1]+hex;

	}

	if(!strcmp(Line[il].word[3],"data"))//if mov xx,data
	{
		hexcode[4]=0x00;
		hexcode[5]=0x00;
	}

	if(Line[il].disp==1) //[si di bx etc. +disp]...
	{
		hexcode[2]=atoi(Line[il].dispval);
		hexcode[3]=0x00;
	}
}

////////////////////////////////////////////////////////////////////////

void encodemov()
{
	if(Line[il].op1.seg)//seg reg cs/ds etc.
	{
		hexcode[0]=hexcode[0]|0x8e;
		coding(1);
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);
		return;
	}

	else if(Line[il].op2.seg)
	{
		hexcode[0]=hexcode[0]|0x8c;
		coding(1);
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);
		return;
	}

	else if(Line[il].imm)//immediate separate case
	{
		if(!strcmp(Line[il].word[3],"data"))
		{
			if(Line[il].op1.reg16)
			{
				hexcode[0]=hexcode[0]|0xb8;
				hexcode[0]=hexcode[0]+regval(Line[il].word[1]);
			}
			coding(1);
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[4],objmem);
			sfputc(hexcode[5],objmem);

			return;
		}

		if(!strcmp(Line[il].word[3],"offset"))
		{
			hexcode[4]=atoi(Line[il].word[4]);
			hexcode[5]=atoi(Line[il].word[4])/256;
		}
		else
		{
			if(Line[il].word[3][strlen(Line[il].word[3])-1]=='h')
			{
				if(hextodec(Line[il].word[3])>255)
					hexcode[5]=(hextodec(Line[il].word[3])/256);
				hexcode[4]=hextodec(Line[il].word[3]);
			}
			else
			{
				if(Line[il].word[3][0]=='\'')
					hexcode[4]=Line[il].word[3][1];
				else
					hexcode[4]=atoi(Line[il].word[3]);
				hexcode[5]=atoi(Line[il].word[3])/256;
			}
		}

		if(Line[il].op1.mem && !Line[il].op1.reg8 &&!Line[il].op1.reg16)
		{

			//if(Line[il].w==1)
			hexcode[0]=hexcode[0]|0xc7;
			//else
			//	hexcode[0]=hexcode[0]|0xc6;

			coding(1);
			hexcode[1]=hexcode[1]|0;
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[1],objmem);

			if(Line[il].disp==1 || Line[il].var==1)
			{
				sfputc(hexcode[2],objmem);
				sfputc(hexcode[3],objmem);
			}

			sfputc(hexcode[4],objmem);
			if(Line[il].imm>1 || Line[il].w)//Line[il].var==0)
				sfputc(hexcode[5],objmem);
			return;
		}

		else if(Line[il].op1.reg8)
		{
			hexcode[0]=hexcode[0]|0xb0;
			hexcode[0]=hexcode[0]+regval(Line[il].word[1]);
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[4],objmem);
		}
		else if(Line[il].op1.reg16)
		{
			hexcode[0]=hexcode[0]|0xb8;
			hexcode[0]=hexcode[0]+regval(Line[il].word[1]);
			sfputc(hexcode[0],objmem);
			if(!strcmp(Line[il].word[3],"offset"))
			{
				sfputc(atoi(Line[il].word[4]),objmem);
			}
			else
			{
				sfputc(atoi(Line[il].word[3]),objmem);
			}
			sfputc(hexcode[5],objmem);
		}

		return;
	}//immediate ends

	else if(Line[il].op1.reg8==1 && Line[il].op2.reg8==1)
	{
		hexcode[0]=hexcode[0]|0x8a;
		coding(1);
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);
		return;
	}

	else if(Line[il].op1.reg16==1 && Line[il].op2.reg16==1)
	{
		hexcode[0]=hexcode[0]|0x8b;
		coding(1);
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);
		return;
	}//both regs...

	//one reg one mem...
	else if(Line[il].op1.reg16==1 || Line[il].op2.reg16==1|| Line[il].op1.mem || Line[il].op2.mem)
	{

		if(Line[il].op1.reg8==1 && Line[il].op2.mem==1)
		{
			hexcode[0]=hexcode[0]|0x8a;

		}
		else if(Line[il].op1.reg16==1 && Line[il].op2.mem==1)
		{
			hexcode[0]=hexcode[0]|0x8b;

		}
		else if(Line[il].op2.reg8==1 && Line[il].op1.mem==1)
		{
			hexcode[0]=hexcode[0]|0x88;

		}
		else if(Line[il].op2.reg16==1 && Line[il].op1.mem==1)
		{
			hexcode[0]=hexcode[0]|0x89;

		}

		coding(1);
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);
		if(Line[il].disp || Line[il].var)
		{
			sfputc(hexcode[2],objmem);
			sfputc(hexcode[3],objmem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////

void encodearith()
{

	if(Line[il].imm)//immediate separate case
	{

		if(!strcmp(Line[il].word[3],"offset"))
		{
			hexcode[4]=atoi(Line[il].word[4]);
			hexcode[5]=atoi(Line[il].word[4])/256;
		}
		else
		{
			if(Line[il].word[3][strlen(Line[il].word[3])-1]=='h')
			{
				if(hextodec(Line[il].word[3])>255)
					hexcode[5]=(hextodec(Line[il].word[3])/256);
				hexcode[4]=hextodec(Line[il].word[3]);
			}
			else
			{
				if(Line[il].word[3][0]=='\'')
					hexcode[4]=Line[il].word[3][1];
				else
					hexcode[4]=atoi(Line[il].word[3]);

				hexcode[5]=atoi(Line[il].word[3])/256;
			}
		}

		if(Line[il].w==1)
		{
			if(Line[il].imm==1)
				hexcode[0]=hexcode[0]|(0x83);/////
			else if(Line[il].imm==2)
				hexcode[0]=hexcode[0]|(0x81);/////
		}

		else
			hexcode[0]=hexcode[0]|0x80;

		if(!strcmp(Line[il].word[0],"adc"))
			hexcode[1]=hexcode[1]|16;
		if(!strcmp(Line[il].word[0],"add"))
			hexcode[1]=hexcode[1]|0;
		if(!strcmp(Line[il].word[0],"and"))
			hexcode[1]=hexcode[1]|32;
		if(!strcmp(Line[il].word[0],"cmp"))
			hexcode[1]=hexcode[1]|56;
		if(!strcmp(Line[il].word[0],"or"))
			hexcode[1]=hexcode[1]|8;
		if(!strcmp(Line[il].word[0],"sbb"))
			hexcode[1]=hexcode[1]|24;
		if(!strcmp(Line[il].word[0],"sub"))
			hexcode[1]=hexcode[1]|40;
		if(!strcmp(Line[il].word[0],"test"))
			hexcode[1]=hexcode[1]|0;
		if(!strcmp(Line[il].word[0],"xor"))
			hexcode[1]=hexcode[1]|48;

		if(Line[il].op1.mem)
		{
			coding(1);
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[1],objmem);
			if(Line[il].disp ||  Line[il].var)
			{
				sfputc(hexcode[2],objmem);
				sfputc(hexcode[3],objmem);
			}
			sfputc(hexcode[4],objmem);
			if(Line[il].imm==2)
				sfputc(hexcode[5],objmem);
			return;
		}

		if(Line[il].op1.reg8)
		{
			hexcode[1]=hexcode[1]|192;
			hexcode[1]=hexcode[1]|regval(Line[il].word[1]);
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[1],objmem);
			sfputc(hexcode[4],objmem);
		}
		else if(Line[il].op1.reg16)
		{
			hexcode[1]=hexcode[1]|192;
			hexcode[1]=hexcode[1]|regval(Line[il].word[1]);
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[1],objmem);
			sfputc(hexcode[4],objmem);
			if(Line[il].imm>1)
				sfputc(hexcode[5],objmem);

		}

		return;
	}// <--- end of immediate...

	//one reg one mem...
	else if(Line[il].op1.reg16==1 || Line[il].op2.reg16==1|| Line[il].op1.mem || Line[il].op2.mem
	|| Line[il].op1.reg8==1 || Line[il].op2.reg8==1)
	{

		if(Line[il].op1.reg8==1)// && Line[il].op2.mem==1)
		{
			hexcode[0]=hexcode[0]|(optab[opno].opcode1-2);

		}
		else if(Line[il].op1.reg16==1)// && Line[il].op2.mem==1)
		{
			hexcode[0]=hexcode[0]|(optab[opno].opcode1-1);

		}
		else if(Line[il].op2.reg8==1)// && Line[il].op1.mem==1)
		{
			hexcode[0]=hexcode[0]|(optab[opno].opcode1-4);

		}
		else if(Line[il].op2.reg16==1)// && Line[il].op1.mem==1)
		{
			hexcode[0]=hexcode[0]|(optab[opno].opcode1-3);

		}

		coding(1);

		if((Line[il].op1.reg8==1 && Line[il].op2.reg8==1) || (Line[il].op1.reg16==1 && Line[il].op2.reg16==1))
		{
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[1],objmem);
		}
		else
		{
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[1],objmem);
			if(Line[il].disp || Line[il].var)
			{
				sfputc(hexcode[2],objmem);
				sfputc(hexcode[3],objmem);
			}
		}
	}

}

///////////////////////////////////////////////////////////////////////////

void encoderotshift()
{
	int i=0;
	if(atoi(Line[il].word[3])==1)
	{

		if(Line[il].op1.reg8)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1);
			hexcode[1]=hexcode[1]|192;
		}
		else if(Line[il].op1.reg16)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|1);
			hexcode[1]=hexcode[1]|192;
		}
		else if(Line[il].op1.mem && Line[il].w)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|1);
		}
		else if(Line[il].op1.mem && Line[il].w==0)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1);
		}
	}

	if(!strcmp(Line[il].word[3],"cl"))
	{

		if(Line[il].op1.reg8)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|2);
			hexcode[1]=hexcode[1]|192;

		}
		else if(Line[il].op1.reg16)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|3);
			hexcode[1]=hexcode[1]|192;

		}
		else if(Line[il].op1.mem && Line[il].w)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|3);
		}
		else if(Line[il].op1.mem && Line[il].w==0)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|2);
		}
	}

	else if(atoi(Line[il].word[3])>1)//immediate
	{

		if(Line[il].op1.reg8)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|0);
			hexcode[1]=hexcode[1]|192;

		}
		else if(Line[il].op1.reg16)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|1);
			hexcode[1]=hexcode[1]|192;

		}
		else if(Line[il].op1.mem && Line[il].w)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|1);
		}
		else if(Line[il].op1.mem && Line[il].w==0)
		{
			coding(1);
			hexcode[0]=(optab[opno].opcode1|0);
		}
	}

	if(isreg(Line[il].word[1]))
	{

		if(Line[il].op1.reg8==1)
			hexcode[1]=hexcode[1]|(regval(Line[il].word[1])|0);

		if(Line[il].op1.reg16==1)
			hexcode[1]=hexcode[1]|(regval(Line[il].word[1])|0);

	}

	if(!strcmp(Line[il].word[0],"rcl"))
		hexcode[1]=hexcode[1]|16;
	if(!strcmp(Line[il].word[0],"rcr"))
		hexcode[1]=hexcode[1]|24;
	if(!strcmp(Line[il].word[0],"rol"))
		hexcode[1]=hexcode[1]|0;
	if(!strcmp(Line[il].word[0],"ror"))
		hexcode[1]=hexcode[1]|8;
	if(!strcmp(Line[il].word[0],"sal"))
		hexcode[1]=hexcode[1]|32;
	if(!strcmp(Line[il].word[0],"sar"))
		hexcode[1]=hexcode[1]|56;
	if(!strcmp(Line[il].word[0],"shl"))
		hexcode[1]=hexcode[1]|32;
	if(!strcmp(Line[il].word[0],"shr"))
		hexcode[1]=hexcode[1]|40;

	if(atoi(Line[il].word[3])>1 && strcmp(Line[il].word[3],"cl"))
	{
		for(i=0;i<atoi(Line[il].word[3]);i++)
		{
			sfputc(hexcode[0],objmem);
			sfputc(hexcode[1],objmem);// hexcode [ 1 ] . . .
			if(Line[il].disp || Line[il].var)
			{
				sfputc(hexcode[2],objmem);
				sfputc(hexcode[3],objmem);
			}

		}
		return;
	}

	else
	{
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);// hexcode [ 1 ] . . .

		if(Line[il].disp || Line[il].var)
		{
			sfputc(hexcode[2],objmem);
			sfputc(hexcode[3],objmem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////

void encodeinout()
{

	if(!strcmp(optab[opno].name,"in"))
	{
		if(strcmp(Line[il].word[1],"al"))
		{
			if(strcmp(Line[il].word[3],"dx"))
				sfputc(0xee,objmem);
		}

		else if(strcmp(Line[il].word[1],"ax"))
		{
			if(strcmp(Line[il].word[3],"dx"))
				sfputc(0xef,objmem);
		}

		else if(Line[il].imm)
		{
			if(strcmp(Line[il].word[1],"al"))
			{
				sfputc(0xe6,objmem);
				sfputc(atoi(Line[il].word[3]),objmem);
			}
			else if(strcmp(Line[il].word[1],"ax"))
			{
				sfputc(0xe7,objmem);
				sfputc(atoi(Line[il].word[3]),objmem);
				if(Line[il].imm==2)
					sfputc(atoi(Line[il].word[3])/256,objmem);
			}

		}

	}

	else if(!strcmp(optab[opno].name,"out"))
	{
		if(strcmp(Line[il].word[3],"al"))
		{
			if(strcmp(Line[il].word[1],"dx"))
				sfputc(0xee,objmem);
		}

		else if(strcmp(Line[il].word[3],"ax"))
		{
			if(strcmp(Line[il].word[1],"dx"))
				sfputc(0xef,objmem);
		}

		else if(Line[il].imm)
		{
			if(strcmp(Line[il].word[3],"al"))
			{
				sfputc(0xe6,objmem);
				sfputc(atoi(Line[il].word[1]),objmem);
			}
			else if(strcmp(Line[il].word[3],"ax"))
			{
				sfputc(0xe7,objmem);
				sfputc(atoi(Line[il].word[1]),objmem);
				if(Line[il].imm==2)
					sfputc(atoi(Line[il].word[1])/256,objmem);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////

void encodexchg()
{

	if(Line[il].op1.reg8==1)// && Line[il].op2.mem==1)
	{
		hexcode[0]=hexcode[0]|(0x86);

	}
	else if(Line[il].op1.reg16==1)// && Line[il].op2.mem==1)
	{
		hexcode[0]=hexcode[0]|(0x87);

	}
	else if(Line[il].op2.reg8==1)// && Line[il].op1.mem==1)
	{
		hexcode[0]=hexcode[0]|(0x86);

	}
	else if(Line[il].op2.reg16==1)// && Line[il].op1.mem==1)
	{
		hexcode[0]=hexcode[0]|(0x87);

	}

	if((Line[il].op1.reg8==1 && Line[il].op2.reg8==1) ||
		(Line[il].op1.reg16==1 && Line[il].op2.reg16==1))
	{
		//Line[il].mod=3;
		coding(1);
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);
	}
	else
	{
		coding(1);
		sfputc(hexcode[0],objmem);
		sfputc(hexcode[1],objmem);
		if(Line[il].disp || Line[il].var)
		{
			sfputc(hexcode[2],objmem);
			sfputc(hexcode[3],objmem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////

void encodeload()
{
	coding(1);
	hexcode[0]=hexcode[0]|optab[opno].opcode1;

	sfputc(hexcode[0],objmem);
	sfputc(hexcode[1],objmem);
	if(Line[il].disp)
	{
		sfputc(hexcode[2],objmem);
		sfputc(hexcode[3],objmem);
	}
}

//  Add / Append extra bytes to data segment to fill a paragraph
void paragraph()
{
	int extra;
	if(dlocctr%16!=0)
	{
		if(dlocctr<16)
			extra=16-dlocctr;
		else
		{
			extra=dlocctr%16;
			extra=16-extra;
		}

		for(int i=0;i<extra;i++)
			sfputc(0x00,objmem);
	}
}


//Encode Instructions : based on a case structure
void encode(int flag)
{

	register int i;
	char r_m=0;
	for(int j=0;j<6;j++)
		hexcode[j]=0;

	seaOpt(0,114);

	switch(flag)
	{

	case 1:

		// 0 operand instructions
		sfputc(optab[opno].opcode1,objmem);

		if(!strcmp(Line[il].word[0],"aad") || !strcmp(Line[il].word[0],"aam"))
			sfputc(0x0a,objmem);

		if(Line[il].word[1][0]!=' ')
		{
			for(i=0;i<110;i++)
			{
				if(!strcmp(Line[il].word[1],optab[i].name))
				{
					cout<<Line[il].word[1];
					sfputc(optab[i].opcode1,objmem);
					break;
				}
			}
		}

		break;

	case 2:

		//jmp call etc.

		sfputc(optab[opno].opcode1,objmem);
		for(i=0;i<symnum;i++)
		{
			if(!strcmp(symEntry[i].name,Line[il].word[1]))
			{
				unsigned char offset;
				if(symEntry[i].addr>Line[il].loc)
				{
					offset=symEntry[i].addr-(Line[il].loc+2);
					if(!strcmp(Line[il].word[0],"call"))
						offset=offset-1;
					sfputc(offset,objmem);
					if(!strcmp(Line[il].word[0],"jmp"))
						sfputc(0x90,objmem);

				}
				else
				{
					if(!strcmp(Line[il].word[0],"jmp"))//  || !strcmp(Line[il].word[0],"loop"))
						offset=symEntry[i].addr-(Line[il].loc+2);
					else
						offset=symEntry[i].addr-(Line[il].loc+2);

					sfputc(offset,objmem);

				}
			}
		}

		if(!strcmp(Line[il].word[0],"call"))
			sfputc(0x00,objmem);

		break;

	case 3: // push pop...

		if(!strcmp(Line[il].word[0],"push"))
		{
			if(!strcmp(Line[il].word[1],"cs"))sfputc(0x0e,objmem);
			else if(!strcmp(Line[il].word[1],"ds"))sfputc(0x1e,objmem);
			else if(!strcmp(Line[il].word[1],"es"))sfputc(0x06,objmem);
			else if(!strcmp(Line[il].word[1],"ss"))sfputc(0x16,objmem);
			else if(isreg(Line[il].word[1]) )
			{
				char hex=0x50;
				hex=hex+regval(Line[il].word[1]);
				sfputc(hex,objmem);
			}
		}
		else if(!strcmp(Line[il].word[0],"pop"))
		{
			if(!strcmp(Line[il].word[1],"ds"))sfputc(0x1f,objmem);
			else if(!strcmp(Line[il].word[1],"es"))sfputc(0x07,objmem);
			else if(!strcmp(Line[il].word[1],"ss"))sfputc(0x17,objmem);
			else if(isreg(Line[il].word[1]) )
			{
				char hex=0x58;
				hex=hex+regval(Line[il].word[1]);
				sfputc(hex,objmem);
			}
		}

		break;

	case 4://neg not

		if(isreg(Line[il].word[1])==1)
		{int p=3;sfputc(0xf6,objmem); r_m=r_m|(p<<6);}
		else if(isreg(Line[il].word[1]))
		{int p=3;sfputc(0xf7,objmem); r_m=r_m|(p<<6);}
		else if(Line[il].op1.mem && !Line[il].w)
			sfputc(0xf6,objmem);
		else if(Line[il].op1.mem && Line[il].w)
			sfputc(0xf7,objmem);
		if(!strcmp(Line[il].word[0],"neg"))
		{
			int p=3;
			r_m=r_m|p<<3;
			r_m=r_m|regval(Line[il].word[1]);
			sfputc(r_m,objmem);
		}

		else if(!strcmp(Line[il].word[0],"not"))
		{
			int p=2;
			r_m=r_m|p<<3;
			r_m=r_m|regval(Line[il].word[1]);
			sfputc(r_m,objmem);

		}

		break;

	case 5:
		//dec inc

		for(i=0;i<6;i++)
			hexcode[i]='\0';
		if(!strcmp(Line[il].word[0],"dec"))
		{
			if(isreg(Line[il].word[1]))
			{
				sfputc(0x48+regval(Line[il].word[1]),objmem);
				return;
			}

			coding(0);
			if(Line[il].w==0)
				sfputc(0xfe,objmem);
			else
				sfputc(0xff,objmem);

			hexcode[1]=hexcode[1]|8;

			sfputc(hexcode[1],objmem);
			if(Line[il].disp==1)
			{
				sfputc(hexcode[2],objmem);
				sfputc(hexcode[3],objmem);
			}
		}

		else if(!strcmp(Line[il].word[0],"inc"))
		{
			if(isreg(Line[il].word[1]))
			{
				sfputc(0x40+regval(Line[il].word[1]),objmem);
				return;
			}

			coding(0);
			if(Line[il].w==0)
				sfputc(0xfe,objmem);
			else
				sfputc(0xff,objmem);

			hexcode[1]=hexcode[1]|0;

			sfputc(hexcode[1],objmem);
			if(Line[il].disp==1)
			{
				sfputc(hexcode[2],objmem);
				sfputc(hexcode[3],objmem);
			}
		}

		break;

		  case 6:
			  // Mul Div. . .

			  if(Line[il].w == 1)
			  {
				  hexcode[0]=optab[opno].opcode1|1;
			  }
			  else
				  hexcode[0]=optab[opno].opcode1;
			  sfputc(hexcode[0],objmem);

			  if(!strcmp(Line[il].word[0],"mul"))
				  hexcode[1]=hexcode[1]=32;

			  if(!strcmp(Line[il].word[0],"div"))
				  hexcode[1]=hexcode[1]=48;

			  if(!strcmp(Line[il].word[0],"imul"))
				  hexcode[1]=hexcode[1]=40;

			  if(!strcmp(Line[il].word[0],"idiv"))
				  hexcode[1]=hexcode[1]=56;

			  if(isreg(Line[il].word[1]))
			  {
				  hexcode[1]=hexcode[1]|192;
				  hexcode[1]=hexcode[1]|regval(Line[il].word[1]);
				  sfputc(hexcode[1],objmem);
			  }
			  else
			  {
				  coding(1);
				  sfputc(hexcode[1],objmem);
			  }

			  if(Line[il].disp==1 || Line[il].var) // By default disp is always 16 bit
			  {
				  sfputc(hexcode[2],objmem);
				  sfputc(hexcode[3],objmem);
			  }

			  break;

		  case 7:
			  // int
			  sfputc(0xcd,objmem);
			  sfputc(hextodec(Line[il].word[1]),objmem);
			  break;

		  case 8:
			  //ArithmeticLogical();
			  encodearith();
			  break;

		  case 9:
			  encoderotshift();
			  break;

		  case 10:
			  encodeinout();
			  break;

		  case 11:
			  encodexchg();
			  break;

		  case 12:
			  encodeload();
			  break;

		  case 13:
			  // mov
			  encodemov();
			  break;
	}
}

//Encode Data Segment portion
void encodeother()
{

	if(!strcmp(Line[il].word[1],"db") || !strcmp(Line[il].word[1],"dw"))
	{

		for(int i=0;i<=symnum;i++)
		{
			if(!strcmp(Line[il].word[0],symEntry[i].name))
			{
				if(Line[il].word[2][0]=='\'')
				{
					int t=1;
					while(Line[il].word[2][t] != '\'')
					{
						sfputc(Line[il].word[2][t++],objmem);
					}
				}

				else if (Line[il].wordnum==3)
				{
					sfputc(atoi(Line[il].word[2]),objmem);
					if(!strcmp(Line[il].word[1],"dw"))//word --> xx 00
					{
						sfputc(0x00,objmem);
					}

				}
				else if(!strncmp(Line[il].word[3],"dup",3))
				{
					if(!strcmp(Line[il].word[1],"dw"))
					{
						for(i=0;i<atoi(Line[il].word[2]);i++)
						{
							sfputc(atoi(Line[il].word[4]),objmem);
							sfputc(0x00,objmem);
						}

					}
					else if(!strcmp(Line[il].word[1],"db"))
					{
						for(i=0;i<atoi(Line[il].word[2]);i++)
						{
							sfputc(atoi(Line[il].word[4]),objmem);
						}
					}
				}
				
				else
				{
					int count=0;
					while(count<(Line[il].wordnum-1))//symEntry[i].size)
					{
						count+=2;
						sfputc(atoi(Line[il].word[count]),objmem);
						if(!strcmp(Line[il].word[1],"dw"))
							sfputc(atoi(Line[il].word[count])/256,objmem);
					}
				}
			}		
		}
	}
}


//Enter into Listing file
void Listing(int line)
{
	fprintf(lstf,"\n  %4d    ",line);
	fprintf(lstf,"%04d    ",Line[line].loc);
}


int main(int argc,char *argv[])
{
	char fname[20],symbname[20],listname[20],execname[20],inter[20];
	char outstr[40],outsymb[400];
	
	cout<<"\n  8086 Assembler Ver 1.0";
	cout<<"\n  _____________________________________________________________\n";
	cout<<"\n Enter file name to be assembled with extension:\n";
	cin>>fname;
	system("clear");
	/*if(argc<2)
	{
		//cout<<"\nParameters missing...";
		///cout<<"\nPlease type 'start.exe' followed by filename with .asm extension";
		//cout<<"\n\nFORMAT:\n"<<"\n   start.exe  filename.asm	\n";
		exit(0);
	}
	else
		strcpy(fname,argv[1]);*/

	for(unsigned int i=0;i<strlen(fname);i++)
	{
		fname[i]=tolower(fname[i]);
	}
	strcpy(inter,fname);
	strcpy(execname,fname);
	strcpy(symbname,fname);
	strcpy(listname,fname);
	strcpy(macrofname,fname);
	int r=strlen(fname)-4;
	if(fname[r]=='.' && fname[r+1]=='a' && fname[r+2]=='s' && fname[r+3]=='m')
	{
		execname[r++]='.';
		execname[r++]='e';
		execname[r++]='x';
		execname[r++]='e';

		r=strlen(fname)-4;
		listname[r++]='.';
		listname[r++]='l';
		listname[r++]='s';
		listname[r++]='t';

		r=strlen(fname)-4;
		inter[r++]='.';
		inter[r++]='t';
		inter[r++]='e';
		inter[r++]='m';

		r=strlen(fname)-4;
		macrofname[r++]='.';
		macrofname[r++]='m';
		macrofname[r++]='c';
		macrofname[r++]='r';

		r=strlen(fname)-4;
		symbname[r++]='.';
		symbname[r++]='s';
		symbname[r++]='y';
		symbname[r++]='m';
	}

	/*else
	{
		cout<<"\nERROR: Unknown Format...";
		cout<<"\nPlease type 'state.exe' followed by filename with .asm extension";
		cout<<"\n\nFORMAT:\n"<<"\n   state.exe  xxx.asm	\n";
		exit(0);

	}*/
	cout<<fname<< "assembled!\n";
	if((objmem=fopen(fname,"r"))==NULL)
	{
		cout<<"\n\nERROR: No such file exists\n";
		return 1;
	}
	fclose(objmem);

	macropro(fname);//Send to Macro processor...

	objmem=fopen(listname,"wb");
	lstf=fopen(listname,"wb");
	symbf=fopen(symbname,"wb");
	intermediate=fopen(inter,"wb");
	pass1(listname);//Send for Pass 1...
	linenum=0;
	toknum=0;
	fprintf(symbf,"name \t addr \t type \t size \t value\n");
		for(int k=0;k<symnum;k++)
		{
			strcpy(outsymb,"\0");
			strcat(outsymb,symEntry[k].name);
			strcat(outsymb,"\0");

			fprintf(symbf,"%s",outsymb);
			fprintf(symbf,"\t %d",symEntry[k].addr);
			fprintf(symbf,"\t %d",symEntry[k].type);

			fprintf(symbf,"\t %d",symEntry[k].size);
			fprintf(symbf,"\t %d",symEntry[k].value);
			fprintf(symbf,"\n");
		}
	fclose(symbf);


	if(errnum<1)//If no error from Pass 1 go into Pass 2...
	{
		//fprintf(lstf,"     Sumi Assembler Ver DCH-67");
		fprintf(lstf,"\n     LIST FILE: %s  for  %s \n\n",listname,fname);
		for(il=1;il<maxlines;il++)
		{
			Listing(il);

			if(strcmp(Line[il].word[0],"code")==0 && strcmp(Line[il].word[1],"segment")==0)
			{
				paragraph();
			}

			if(Line[il].is_instruc==1)
			{
				int type=isValidInstruction(Line[il].word[0]);
				encode(type);
			}
			else if(Line[il].word[0][0]!=' ')
			{
				encodeother();
			}

			strcpy(outstr,"\0");
			for(int p=0;p<Line[il].wordnum;p++)
			{
				strcat(outstr,Line[il].word[p]);
				strcat(outstr," ");
			}
			fprintf(lstf,"\t\t\t");
			fprintf(lstf,"%s",outstr);

			for(int ci=0;ci<6;ci++)
				hexcode[ci]=0;
		}
		unlink("tempbuf.asm");
		unlink("tempbuf2.asm");
	}
	cout<<"\nNo of errors encountered: "<<errnum<<endl;
	if(errnum<1)
		cout<<"\n\nFiles created\n\n1)Listfile:"<<listname<<"\n2)Symbol tabel:"<<symbname<<"\n3)Macro file:"<<macrofname<<"\n4)Intermediate file:"<<inter<<"\n\n";
	else cout<<listname<<" could not be created...\n\n";
	fclose(objmem);
	fprintf (lstf, "\n\n*********%s", "User defined variables and types.**********\n");
	for (int i =0; i < userDefinedIndex; i++)//++i 
{
		fprintf (lstf, "\n%-20s%s\n", ud[i].varName , ud[i].varType );
}
	fprintf (lstf, "\n\nNumber of lines  = %d\n\n", maxlines);
	fprintf (lstf, "\n\nNumber of errors = %d\n\n", errnum);
	fclose(lstf);
	fclose(intermediate);
	//getch();
	return 0;

}//end main
