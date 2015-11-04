
// PASS 1 of Assembler...

#include "iostream"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;
#include "Macropro.cpp"
int toknum=0;
int symnum=0;
int locctr=0;
int dlocctr=0;
int relocatable;
int reloc[5];
int maxlines;	
int flag1,flag2,flag3;
char elem[20];
char token[20][40];

FILE *curf, *objmem,*intermediate;

// Instruction Operand Flags
void itoa(int value,char* result,int base)
{
//to check the base is valid
	if(base<2 || base>36)
	{
		*result='\0';
	}
	char* ptr=result,*ptr1=result,tmp_char;
	int tmp_value;
	do
	{
	tmp_value=value;
	value/=base;
	*ptr++="Zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35+(tmp_value-value*base)]; 
	}while(value);
	if(tmp_value<0) *ptr++='-';
	*ptr--='\0';
	while(ptr1<ptr)
	{
		tmp_char=*ptr;
		*ptr--=*ptr1;
		*ptr1++=tmp_char;
	}
}

class flags{
	public:
	   unsigned int reg8;
	   unsigned int reg16;
	   unsigned int seg;
	   unsigned int mem;
	   
};


//Line structure to store info about each line
class lines{
public:
	int wordnum;
	unsigned int loc;
	int is_instruc;
	int mod;
	char dispval[20];
	unsigned int disp;
	unsigned int imm;
    unsigned int var;
    unsigned int index;
    unsigned int base;
	unsigned int d;
	unsigned int w;
	char word[20][40];
    flags op1,op2;
};

//class lineinfo lines;
lines Line[50];

//Classification of Instructions
char type1[41][8]={"aaa","aad","aam","aas","cbw","clc","cld","cli","cmc","cmpsb","cmpsw","cwd","daa","das","hlt","int3","iret","into","lahf","lock","lodsb","lodsw","movsb","movsw","nop","popf","pushf","rep","repe","repne","ret","sahf","scasb","scasw","stc","std","sti","stosb","stosw","wait","xlat"};
char type2[36][8]={"jo","jno","jb","jnae","jae","jnb","je","jz","jne","jnz","jbe","jna","ja","jnbe","js","jns","jp","jpe","jnp","jpo","jl","jnge","jge","jnl","jle","jng","jg","jnle","jcxz","loop","loope","loopz","loopne","loopnz","jmp","call"};
char type3[2][6]={"push","pop"};
char type4[2][5]={"neg","not"};
char type5[2][5]={"dec","inc"};
char type6[4][6]={"div","idiv","mul","imul"};
char type7[5]={"int"};
char type8[9][6]={"adc","add","and","cmp","or","sbb","sub","test","xor"};
char type9[8][5]={"rcl","rcr","rol","ror","sal","sar","shl","shr"};
char type10[2][5]={"in","out"};
char type11[6]={"xchg"};
char type12[3][5]={"lds","lea","les"};
char type13[5]={"mov"};


//The Symbol Table
class symtab{
public:
	char name[20];
	unsigned int addr;
	unsigned int size;
	unsigned int value;
	int type; //0=label, 1=symbol(db), 2=symbol(dw), 4=const(equ) 
}symEntry[20];


//Function for Entry into Symbol table
void entSymtab(char *entry,int typ,int siz,int val)
{
	strcpy(symEntry[symnum].name,entry);
	symEntry[symnum].addr=locctr;
	symEntry[symnum].type=typ;
	symEntry[symnum].size=siz;
	symEntry[symnum].value=val;
	symnum++;
}

//Global function to issue Error messages and
//update error count
void errmsg()
{
	cout<<"\n\nERROR: Line "<<linenum<<"  ";
	errnum++;
} 

void dectohex(int decival,char resulta[])
{
	char hexconv[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	if(decival<16)
	{
		resulta[3]=hexconv[decival];
	}
	else
	{
	if(decival<256)
	{
		int rema;
		int tempa;
		tempa=decival/16;
		rema=decival%16;
		resulta[2]=hexconv[tempa];
		resulta[3]=hexconv[rema];
	}
	}
}

void interf(int line)
{
	char resulta[5]={'0','0','0','0','\0'};
	char putin[40];
	fprintf(intermediate,"\n %4d  ",line);
	dectohex(Line[line].loc,resulta);
	fprintf(intermediate,"%s  ",resulta);
	strcpy(putin,"\0");
	for(int p=0;p<Line[line].wordnum;p++)
	{
		strcat(putin,Line[line].word[p]);
	}
	fprintf(intermediate,"\t\t\t\t");
	fprintf(intermediate," %s",putin);
}

//Function converts a hex value to decimal
int hextodec(char elem[5])
{
	int p,count=0,iter=0;
	int value=0;
	int final=0;
	p=strlen(elem)-2;
	while(isalnum(elem[p]) && p>-1)
	{
		count=(int)pow(16,iter);
		if(elem[p]=='0')
		{
			value=0;
		}
		if(elem[p]=='1')
		{
			value=1;
		}
		if(elem[p]=='2')
		{
			value=2;
		}
		if(elem[p]=='3')
		{
			value=3;
		}
		if(elem[p]=='4')
		{
			value=4;
		}
		if(elem[p]=='5')
		{
			value=5;
		}
		if(elem[p]=='6')
		{
			value=6;
		}
		if(elem[p]=='7')
		{
			value=7;
		}
		if(elem[p]=='8')
		{
			value=8;
		}
		if(elem[p]=='9')
		{
			value=9;
		}
		if(elem[p]=='a')
		{
			value=10;
		}
		if(elem[p]=='b')
		{
			value=11;
		}
		if(elem[p]=='c')
		{
			value=12;
		}
		if(elem[p]=='d')
		{
			value=13;
		}
		if(elem[p]=='e')
		{
			value=14;
		}
		if(elem[p]=='f')
		{
			value=15;
		}
		final=final+value*count;
		
		p--;
		iter++;
	}
	return final;
}

//Binary to Decimal
int bintodec(char elem[5])
{
	int p,count=0,iter=0;
	int value=0;
	int final=0;
	p=strlen(elem)-2;
	while(isalnum(elem[p]) && p>-1)
	{
		count=(int)pow(2,iter);
		if(elem[p]=='0')
		{
			value=0;
		}
		if(elem[p]=='1')
		{
			value=1;
		}
		
		final=final+value*count;
		
		p--;
		iter++;
	}
	return final;
}

//Check if First token is a valid instruction
int isValidInstruction(char tok[10])
{
	int i;
	
	for(i=0;i<41;++i)
	{
		if(!strcmp(type1[i],tok))
		{
			return 1;
		}
	}
	for(i=0;i<36;++i)
	{
		if(!strcmp(type2[i],tok))
		{
			return 2;
		}
	}
	for(i=0;i<2;++i)
	{
		if(!strcmp(type3[i],tok))
		{
			return 3;
		}
	}
	for(i=0;i<2;++i)
	{
		if(!strcmp(type4[i],tok))
		{
			
			return 4;
		}
	}
	for(i=0;i<2;++i)
	{
		if(!strcmp(type5[i],tok))
		{
			
			return 5;
		}
	}
	for(i=0;i<4;++i)
	{
		if(!strcmp(type6[i],tok))
		{
			
			return 6;
		}
	}
	if(!strcmp(type7,tok))
	{
		
		return 7;
	}
	for(i=0;i<9;++i)
	{
		if(!strcmp(type8[i],tok))
		{

			return 8;
		}
	}
	for(i=0;i<8;++i)
	{
		if(!strcmp(type9[i],tok))
		{
			
			return 9;
		}
	}
	for(i=0;i<2;++i)
	{
		if(!strcmp(type10[i],tok))
		{
			
			return 10;
		}
	}
	if(!strcmp(type11,tok))
	{
		
		return 11;
	}
	
	for(i=0;i<3;++i)
	{
		if(!strcmp(type12[i],tok))
		{
			
			return 12;
		}
	}
	
	if(!strcmp(type13,tok))
	{
		return 13;
	}
	return 0;
}

//    Classify / Identify Line
int check()
{
	if(isValidInstruction(token[0]))return 0;

	if((strcmp(token[0],"end")==0)||(strcmp(token[0],"loop")==0)||(strcmp(token[0],"assume")==0)||(strcmp(token[0],"code")==0)||(strcmp(token[0],"data")==0))
		return 5;
	if(!strcmp(token[1],"equ"))
	{  entSymtab(token[0],4,0,atoi(token[2]));return 4;}
	else if(!strcmp(token[1],"endp"))return 5;
	else if(strcmp(token[1],":")==0 || !strcmp(token[1],"proc")) {
		entSymtab(token[0],0,0,0); 

		if (strcmp (token[1], ":") == 0) {
			strcpy (ud[userDefinedIndex].varName, token[0]);
			strcpy (ud[userDefinedIndex++].varType, "Label");
		}

		else if (strcmp (token[1], "proc") == 0) {
			strcpy (ud[userDefinedIndex].varName, token[0]);
			strcpy (ud[userDefinedIndex++].varType, "Procedure");
		}

		return 4;
	} //Label
	for(int i=0;i<symnum;i++){
		if(strcmp(token[0],symEntry[i].name)==0)
		{
			errmsg();cout<<symEntry[i].name<<" already exists";
			return -1;
		}
	}
	if(!strcmp(token[1],"db") || !strcmp(token[1],"dw"))
		return 1;
	if(!strcmp(token[0]," ") || !strcmp(token[0],"  ") || !strcmp(token[0],"\0"))
		return 5;
	errmsg();cout<<"Illegal Symbol/Instruction";
	return 5;
}


// Tokenize/split line into tokens
void tokenize()
{
	toknum=0;
	for(register int i=0;i<80;i++)
	{
starting:
	int ii=0;
	while_Null(i); //if space is encountered

	if(i>=80)break;
	if(asmbuf[i]=='\0') return;
	if(asmbuf[i]==',')
	{
		i++;
		strcpy(token[toknum],",");
		toknum++;
		continue;
	}
	if(asmbuf[i]==':')
	{
		i++;
		strcpy(token[toknum],":");
		toknum++;
		continue;
	}
	while(isalnum(asmbuf[i]) || asmbuf[i]=='\'' || asmbuf[i]=='+' || asmbuf[i]=='[' || asmbuf[i]=='_' || asmbuf[i]==']')
	{
		token[toknum][ii]='\0';
		if(asmbuf[i]=='[')
		{
			while(asmbuf[i]!=']')
			{
				if(asmbuf[i]==' ')i++;
				token[toknum][ii]=asmbuf[i];
				i++; ii++;token[toknum][ii]='\0';
			}
			token[toknum][ii]=']';
			i++; ii++;token[toknum][ii]='\0';
			break;
		}
		
		if(asmbuf[i]=='\'')
		{
			while(asmbuf[i]!='$')
			{
				token[toknum][ii]=asmbuf[i];
				i++; ii++;token[toknum][ii]='\0';
			}
			token[toknum][ii]='$';
			i++; ii++;
			token[toknum][ii]='\'';
			i++; ii++;token[toknum][ii]='\0';
			break;
		}
		
		if(asmbuf[i]=='(')
		{
			i++;
			while(asmbuf[i]!=')')
			{
				if(asmbuf[i]==' ')i++;
				token[toknum][ii]=asmbuf[i];
				i++; ii++;token[toknum][ii]='\0';
			}
			i++; ii++;token[toknum][ii]='\0';
			break;
		}
		
		token[toknum][ii++] = asmbuf[i++];
		token[toknum][ii]='\0';
		if(asmbuf[i]=='\0')return;
		
	}
	token[toknum][ii]='\0';
	
	if(asmbuf[i]==',')
	{
		toknum++;
		i++;
		strcpy(token[toknum],",");
		toknum++;
		goto starting;
	}
	if(asmbuf[i]==':')
	{
		toknum++;
		i++;
		strcpy(token[toknum],":");
		toknum++;
		goto starting;
	}
	if(ii>1)
		toknum++;
	if(i>=80)break;
	if(asmbuf[i]=='\0')return;
	}
}

//To check if paarameter is a Register

int isreg(char a[5])
{
	if(!(strcmp(a,"al")) || !(strcmp(a,"bl"))|| !(strcmp(a,"cl"))|| !(strcmp(a,"dl")))
		return 1;
	else if(!(strcmp(a,"ah")) || !(strcmp(a,"bh"))|| !(strcmp(a,"ch"))|| !(strcmp(a,"dh")))
		return 1;
	else if(!(strcmp(a,"ax")) || !(strcmp(a,"bx"))|| !(strcmp(a,"cx"))|| !(strcmp(a,"dx")))
		return 2;
	if(!(strcmp(a,"sp")) || !(strcmp(a,"bp")))
		return 3;
	else if(!(strcmp(a,"si"))|| !(strcmp(a,"di")))
		return 4;
	else if(!strcmp(a,"cs"))return 5;
	else if(!strcmp(a,"ds"))return 6;
	else if(!strcmp(a,"es"))return 7;
	else if(!strcmp(a,"ss"))return 8;
	else return 0;
}

int dbsiz( int n)
{
	if(n>0 && n<=8)n=1;
	else if(n>8 && n<=16)n=2;
	else if(n>16 && n<=32)n=3;
	else if(n>32 && n<=64)n=4;
	return n;
}

void inparenth(int flag,char buf[20],int &i)
{
	int code=0;
// Try doing it.    --i;
	if(flag!=3)
	{
		if(!strcmp(buf,"bx"))
		{
			if(flag==1)
				locctr+=2;
			Line[linenum].base=1;
		}
		else if(!strcmp(buf,"bp"))
		{
			if(flag==1)
				locctr+=2;
			Line[linenum].base=2;
		}//base reg
		else if(!strcmp(buf,"si"))
		{
			if(flag==1)
				locctr+=2;
			Line[linenum].index=1;
		}
		else if(!strcmp(buf,"di"))
		{
			if(flag==1)
				locctr+=2;
			Line[linenum].index=2;
		}//index reg
		code=1;
	}
	
	if(!isalpha(buf[0]))
	{
		if(buf[i]=='h')
		{	
			if(strlen(buf)<=3)locctr+=2;//immediate 8
			else locctr+=2;
			Line[linenum].disp=1;//immediate 16
		}
		else if(buf[i]=='b')
		{
			if(strlen(buf)<=9)locctr+=2;//immediate 8
			else locctr+=2;
			Line[linenum].disp=1;//immediate 16
		}
		else if(atoi(buf)>255)
		{
			locctr+=2;//cout<<"ERROR: Line "<<linenum<<" : Constant too large";
		}

		else if(atoi(buf)<255 && atoi(buf)>0) 
		{
			locctr+=2;
			Line[linenum].disp=1;
		}//imm8
		code=1;
	}

	else if(code==0)
	{

		errmsg();
		cout<<"Undefined Symbol(s) ";
	}
}


void splitparenth(char buf1[20],char buf2[20],char buf3[20],int &i1,int &i2,int &i3,int &i)
{
	while(elem[i]!=']')
	{			
		flag1=1;
		if(elem[i]=='+')
		{
			i++;
			flag2=1;
			while(elem[i]!=']')
			{
				if(elem[i]=='+')
				{
					i++;
					flag3=1;
					while(elem[i]!=']' && elem[i]!='+')
					{
						buf3[i3++]=elem[i++];
						buf3[i3]='\0';
					}
					return;
				}
				
				buf2[i2++]=elem[i++];
				buf2[i2]='\0';
			}
			return;
		}
		
		buf1[i1++]=elem[i++];
		buf1[i1]='\0';
	}
	return;
}


// If token is not Register function Identifies if it is memory or
// immediate and sets mod reg disp imm and other fields . . .

void tokType(char element[20],int typ)
{
	strcpy(elem,element);
	register int i=0,i1=0,i2=0,i3=0,ii=0;
	int k=0;//iii=0;
	flag1=0;flag2=0;flag3=0;
	char buf[20],buf1[20],buf2[20],buf3[20];
	
	if(!strcmp(elem,"data"))
	{
		if(isreg(token[1]))
            locctr+=3;
        
		Line[linenum].imm=1;

        relocatable++;        
		reloc[relocatable]=locctr;
		return;
	}
	
	for(k=0;k<symnum;k++)//mem
	{
		if(strcmp(elem,symEntry[k].name)==0)
		{
			if(symEntry[k].type==4)
			{
				locctr+=4;
                		itoa(symEntry[k].value, (token[3]),10);
                		itoa(symEntry[k].value, Line[linenum].word[3],10);
                		itoa(symEntry[k].value, elem,10);
				break;
			}

			Line[linenum].mod=0;
			Line[linenum].var=1;

			if(typ==1)
			{
				if(symEntry[k].type==2)
					Line[linenum].w=1;
				if(!isreg(token[3]) && isalpha(token[3][0]))
				{
					errmsg();
					cout<<"Illegal operand types";
					return;
				}
				if(symEntry[k].type==1 && isreg(token[3])>1)
				{
					errmsg();
					cout<<"Operand type mismatch";
					return;
				}
				if(symEntry[k].type==2 && isreg(token[3])==1)
				{
					errmsg();
					cout<<"Operand type mismatch";
					return;
				}
				
				Line[linenum].op1.mem=1;
				
				if(isreg(token[3]))
				{					
					locctr+=4;
				}
				else 
				{
					if(symEntry[k].type==2)
					{
						Line[linenum].w=1;
					}
					locctr+=5 ;
				}
				return;
			}
			
			if(typ==3)
			{
				if(!isreg(token[1]))
				{
					errmsg();
					cout<<"Illegal operand types";
					return;
				}
				
				if(symEntry[k].type==1 && isreg(token[1])>1)
				{
					errmsg();
					cout<<"Operand type mismatch";
					return;
				}
				if(symEntry[k].type==2 && isreg(token[1])==1)
				{
					errmsg();
					cout<<"Operand type mismatch";
					return;
				}
				Line[linenum].op2.mem=1;
				Line[linenum].var=1;

                locctr+=4;

				if(symEntry[k].type==2)
					Line[linenum].w=1;
				return;
			}
			
			//return;
		}
		//mem ref in symtab
	}
	
	if(typ==3)
	{
		if(strcmp(elem,"offset")==0)
		{
			int p=0;
            Line[linenum].imm=1;

			for(k=0;k<symnum;k++)//mem
			{
				if(strcmp(token[4],symEntry[k].name)==0)
				{
					p=1;
					if(Line[linenum].op1.reg8==1)
					{
						locctr+=2;
					}
					else
					{
						locctr+=3;
					}
					
					itoa(symEntry[k].addr,token[4],10);
					itoa(symEntry[k].addr,Line[linenum].word[4],10);
					
					return;//break;
				}
			}
			if(p==0)
			{
				errmsg();
				cout<<"Undefined Symbol"<<endl;
				return;
			}
			
		}
		
		else if(elem[i]=='[')//mem
		{
			i++;
            Line[linenum].op2.mem=1;
			splitparenth(buf1,buf2,buf3,i1,i2,i3,i);
		
		}

		if(flag3==1)
		{
			inparenth(3,buf3,i3);

		}
				
		if(flag2==1)
		{
			inparenth(2,buf2,i2);
		}
		
		if(flag1==1)
		{
			Line[linenum].w=1;
			Line[linenum].op2.mem=1;
			inparenth(1,buf1,i1);
		}
		
		else if(!isalpha(elem[0]) && typ==3)// && !flag1 && !flag2 && !flag3)  //immediate
		{
			strcpy(buf,elem);
			ii=strlen(elem)-1;
						
			for(k=0;k<symnum;k++)//mem
			{
				if(strcmp(token[1],symEntry[k].name)==0)
				{
					
					if(buf[ii]=='h')
					{
						
						if(hextodec(buf)<=255)//immediate 8
						{
							Line[linenum].imm=1;
						}
						
						else
						{
							Line[linenum].imm=2;
							locctr+=1;
						}
						
					}
					
					else if(buf[ii]=='b')
					{
						if(bintodec(buf)<=255)
						{
							Line[linenum].imm=1;
						}
						else 
						{							
							Line[linenum].imm=2;
							locctr+=1;
						}
					}
					
					else if(atoi(buf)<=255)
					{						
						Line[linenum].imm=1;
					}
					
					else
					{
						Line[linenum].imm=2;
						locctr+=1;
					}
					return;
				}
			}
			
			if(!strcmp(token[0],"mov")) //mov...
			{
				if(isreg(token[1])) //|| Line[linenum].w==1)
				{
					locctr+=1;
				}

				if(buf[ii]=='h')
				{
					if(hextodec(buf)<=255)//immediate 8
					{
						if(Line[linenum].op1.reg8==1)
                            locctr+=1;
						else
                            locctr+=2;
						Line[linenum].imm=1;
					}
					
					else
					{
						locctr+=2;//immediate 16
						Line[linenum].imm=2;
					}
				}
				
				else if(buf[ii]=='b')
				{
					if(strlen(buf)<=9)
					{
						if(Line[linenum].op1.reg8==1) locctr+=1;
						else locctr+=2;//immediate 8
						Line[linenum].imm=1;
					}
					else 
					{
						locctr+=2;//immediate 16
						Line[linenum].imm=2;
					}
				}
				
				else if(atoi(buf)<=255)
				{
					if(Line[linenum].op1.reg8==1)
						locctr+=1;
					else 
						locctr+=2;
					Line[linenum].imm=1;
				}
				
				else
				{
					locctr+=2;
					Line[linenum].imm=2;
				}
			}
						
			else //not mov ...
			{
				
				if(isreg(token[1])) //|| Line[linenum].w==1)
				{
					locctr+=2;
					Line[linenum].imm=1;
				}
				
				if(buf[ii]=='h')
				{
					
					if(strlen(buf)<=3)//immediate 8
					{
						locctr+=1;
						Line[linenum].imm=1;
					}
					
					else 
					{
						locctr+=2;//immediate 16
						Line[linenum].imm=2;
					}
				}
				
				else if(buf[ii]=='b')
				{
					if(strlen(buf)<=9)
					{
						
						locctr+=1;//immediate 8
						Line[linenum].imm=1;
					}
					else
					{
						locctr+=2;//immediate 16
						Line[linenum].imm=2;
					}
				}
				
				else if(atoi(buf)<=255)
				{
					
					locctr+=1;
					Line[linenum].imm=1;
					
				}
				
				else
				{
					locctr+=2;
					Line[linenum].imm=2;
				}
			}
		}
		
		else
		{
			errmsg();
			cout<<"Illegal/Undefined Symbol(s) ";
		}
	}
	
	else if(typ==1)
	{
		if(strcmp(elem,"offset")==0)
		{
			errmsg();
			cout<<" Illegal immediate "<<endl;
		}
		
		else if(elem[i]=='[')//mem
		{
			i++;Line[linenum].op1.mem=1;
			splitparenth(buf1,buf2,buf3,i1,i2,i3,i);
		}
		
		if(flag3==1)
		{
			inparenth(3,buf3,i3);
		}
		
		if(flag2==1)
		{
			inparenth(2,buf2,i2);
		}
		
		if(flag1==1)
		{
			Line[linenum].w=1;
			Line[linenum].op1.mem=1;
			inparenth(1,buf1,i1);
		}
		
		else
		{
			errmsg();
			cout<<"Illegal/Undefined Symbol(s) ";
		}
	}
	
	else
	{
		errmsg();
		cout<<"Illegal/Undefined Symbol(s) ";
	}
		
	if(!strcmp(token[3],"offset")) 
		Line[linenum].imm=2;
	if(flag1==1 && flag2==1 && flag3==1)
		Line[linenum].mod=2;
	else if(flag1==1 && flag2==1 &&flag3==0 && Line[linenum].disp==1)
		Line[linenum].mod=2;
	else if(flag1==1 && flag2==1 &&flag3==0 && Line[linenum].disp==0)
		Line[linenum].mod=0;							
	else if(flag1==1 && flag2==0 &&flag3==0)
		Line[linenum].mod=0;
	
	i=0;ii=0;
	if(Line[linenum].disp==1)
	{
		if(!isalpha(buf1[0]))
			while(!isalpha(buf1[i])){ Line[linenum].dispval[ii++]=buf1[i++];}
			else if(!isalpha(buf2[0]))
				while(!isalpha(buf2[i])){ Line[linenum].dispval[ii++]=buf2[i++];}
				else if(!isalpha(buf3[0]))
					while(!isalpha(buf3[i])){ Line[linenum].dispval[ii++]=buf3[i++];}
	}
	
}

////////////////////////////////////////////////////////////////////////////////

int handleMAX(int cod)
{
	if(isreg(token[1])==5)
	{
		errmsg();
		cout<<"Invalid Operand in instruction";
		return 0;//exit(0);
	}
	
	else if(isreg(token[1])==1 && (isreg(token[3])==2 || isreg(token[1])==3 || isreg(token[3])==4))
	{
		errmsg();
		cout<<"Operand types do not match...";
		return 0;//exit(0);
	}
	else if(isreg(token[3])==1 && (isreg(token[3])==2 || isreg(token[1])==3 || isreg(token[3])==4))
	{
		errmsg();
		cout<<"Operand types do not match...";
		return 0;//exit(0);
	}

	else if(isreg(token[3])==1 && isreg(token[1])==1)
	{
		locctr+=2;
		Line[linenum].op1.reg8=1;
		Line[linenum].op2.reg8=1;

        	Line[linenum].mod=3;

        	return 1;
	}

	else if(isreg(token[1]) && !isreg(token[3]))
	{
		Line[linenum].d=1;

		if(isreg(token[1])==1)
		{
			Line[linenum].op1.reg8=1;
			tokType(token[3],3);
		}

		else
		{
			Line[linenum].w=1;

			if(isreg(token[1])==5 || isreg(token[1])==6)
			{
				Line[linenum].op1.seg=1;
			}
			Line[linenum].op1.reg16=1;
			tokType(token[3],3);
		}
		return 1;

	}

	else if(isreg(token[3]) && !isreg(token[1]))
	{
		Line[linenum].d=0;
		if(isreg(token[3])==1)
		{
			Line[linenum].op2.reg8=1;
			tokType(token[1],1);
		}
		else
		{
			Line[linenum].w=1;

			if(isreg(token[3])==5 || isreg(token[3])==6)
			{
				Line[linenum].op2.seg=1;
			}
			Line[linenum].op2.reg16=1;
			tokType(token[1],1);
		}
		return 1;
	}

	else if(isreg(token[1])!=0 && isreg(token[3])!=0 && isreg(token[1])!=1 && isreg(token[3])!=1)
	{
		locctr+=2;
		if(cod==1)
		{
			if(isreg(token[1])==5 || isreg(token[1])==6)
			{
				Line[linenum].op1.seg=1;
			}
			if(isreg(token[3])==5 || isreg(token[3])==6)
			{
				Line[linenum].op2.seg=1;
			}
		}
		Line[linenum].op1.reg16=1;
		Line[linenum].op2.reg16=1;
		Line[linenum].w=1;
		Line[linenum].mod=3;
		return 1;
	}

	else
	{
		tokType(token[3],3);
		tokType(token[1],1);
		Line[linenum].op1.mem=1;
		return 1;
	}


	   errmsg();
	return 0;
}

void handleNoop()
{
	locctr+=1;
	if(!strcmp(token[0],"aad") || !strcmp(token[0],"aam"))
		locctr+=1;
}

void handleJump()
{
	for(int k=0;k<symnum;k++)
	{
		if(!strcmp(token[1],symEntry[k].name) && symEntry[k].type==0)
		{
			locctr+=2;
			Line[linenum].var=1;
		}
	}
	if(!(Line[linenum].var==1))
	{
		if(!strcmp(token[0],"jmp") || !strcmp(token[0],"call"))
			locctr+=3;
		else
			locctr+=2;
	}
}

void handlePushPop()
{
	if(isreg(token[1])==1)
	{
		errmsg();
		cout<<"Illegal size\n";
	}
	else if(!strcmp(token[1],"cs"))locctr+=1;
	else if(!strcmp(token[1],"ds"))locctr+=1;
	else if(!strcmp(token[1],"es"))locctr+=1;
	else if(!strcmp(token[1],"ss"))locctr+=1;
	else if(isreg(token[1])>1) locctr+=1;
	else tokType(token[1],1); 
}

void handleNegNot()
{
	tokType(token[1],3); 
}

void handleDecInc()
{
	if(isreg(token[1])>1)
	{
		Line[linenum].w=1;
		locctr+=1;
		return;
	}

	tokType(token[1],3); 
}

void handleMulDiv()
{
	if(isreg(token[1])==0)
		tokType(token[1],1); 
	else
	{
		if(isreg(token[1])==1)
		{
			Line[linenum].op2.reg8=1;
		}
		
		else if(isreg(token[1])>1)
		{
			if(isreg(token[1])==5 || isreg(token[1])==6)
				
			{
				Line[linenum].op2.seg=1;
			}
			
			Line[linenum].op2.reg16=1;
			Line[linenum].w=1;
		}
		
		locctr+=2;
		
	}
	
}

void handleInt()
{
	locctr+=2;
}


void handleRotShift()
{
	if(!strcmp(token[3],"1") || !strcmp(token[3],"cl"))
	{
		if(isreg(token[1])==1)
		{
			Line[linenum].op1.reg8=1;
			locctr+=2;
		}
		else if(isreg(token[1])>1)
		{
			Line[linenum].op1.reg16=1;
			locctr+=2;
		}
		else
		{
			Line[linenum].op1.mem=1;
			tokType(token[1],1);
		}
		return;
	}
	
	else //immediate
	{
		Line[linenum].imm=1;
		char buf[20];
		strcpy(buf,token[3]);
		
		
		if(isreg(token[1])==1)
		{
			Line[linenum].op1.reg8=1;
			locctr+=atoi(token[3])*2;
			
		}
		else if(isreg(token[1])>1)
		{
			Line[linenum].op1.reg16=1;
			locctr+=atoi(token[3])*2;
			
		}
		
		else if(isreg(token[1])==1 && atoi(token[3])>8)
		{
			errmsg();
			cout<<"Count out of range";
		}
		
		else if(atoi(token[3])<=8)
		{
			Line[linenum].op1.mem=1;
			for(int i=0;i<atoi(token[3]);i++)
			{
				tokType(token[1],1);
				locctr+=1;
			}
		}
		else 
		{
			errmsg();
			cout<<" Count out of range";
		}
	}
	
}

void handleInOut()
{
	if(!strcmp(token[0],"in"))
	{
		if(!strcmp(token[1],"al")||!strcmp(token[1],"ax"))
		{
			if(isreg(token[1])==1)Line[linenum].op1.reg8=1;			
			else Line[linenum].op1.reg16=1;
			if(isreg(token[3]))
			{
				if(!strcmp(token[3],"dx"))
				{
					locctr+=1;
					Line[linenum].op2.reg16=1;
				}
				else
				{
					errmsg();
					cout<<" Only dx expected Line "<<linenum<<endl;
				}
			}
			else
			{
				locctr+=2;
				Line[linenum].imm=1;
			}
		}
		else 
		{
			errmsg();
			cout <<" Only al or ax allowed Line "<<endl;
		}
	}
	
	else if(!strcmp(token[0],"out"))
	{
		
		if(!strcmp(token[3],"al")||!strcmp(token[3],"ax"))
		{
			if(isreg(token[1])==1)Line[linenum].op2.reg8=1;			
			else Line[linenum].op2.reg16=1;
			
			if(isreg(token[1]))
			{
				if(!strcmp(token[1],"dx"))
				{
					locctr+=1;
					Line[linenum].op1.reg16=1;
				}
				else
				{
					errmsg();
					cout<<" Only dx expected Line "<<endl;
				}
			}
			else
			{
				locctr+=2;
				Line[linenum].imm=1;
			}
		}
		
		else
		{
			errmsg();
			cout <<" Only al or ax allowed Line "<<linenum;
		}
	}
	
}


int handleLoad()
{
	if(isreg(token[1])>1 && isreg(token[3])==0)
	{
		
		if(isreg(token[1])==1)
		{
			Line[linenum].op1.reg8=1;
			tokType(token[3],3); 
		}
		
		else
		{
			Line[linenum].w=1;
			tokType(token[3],3); 
		}
		return 1;
		
	}
	
	else if(isreg(token[3])>1 && isreg(token[1])==0)
	{
		if(isreg(token[3])==1)
		{
			Line[linenum].op2.reg8=1;
			tokType(token[1],1); 
		}
		else
		{
			Line[linenum].w=1;
			tokType(token[1],1); 
		}
		return 1;
	}
	
	errmsg();
	
	locctr+=1;
	return 0;
	
}

///////////////////////////////////////////////////////////////////////////////

void decideinstruc(int flag)
{
	
	switch(flag)
	{
        case 1:    
		handleNoop();
		break;
		
	case 2:
		handleJump();
		break;
		
	case 3:
		handlePushPop();
		break;
		
	case 4:
		handleNegNot();
		break;
		
	case 5:
		handleDecInc();
		break;
		
	case 6:
		handleMulDiv();
		break;
		
	case 7:
		handleInt();
		break;
		
        case 8:
                handleMAX(2);//ArithLogic
                break;
			  
		  case 9:
			  handleRotShift();
			  break;
			  
		  case 10:
			  handleInOut();
			  break;
			  
		  case 11:
			  handleMAX(3);//xchg
			  break;
			  
		  case 12:
			  handleLoad();
			  break;   
			  
		  case 13:
			  handleMAX(1);//mov
			  break;
	}
}

void decideother(int flag)
{
	   switch(flag)
	   {
	   case 1:
		   //Unknown Label.......
		   
		   if(strcmp(token[1],"db")==0)
		   {
			   strcpy (ud[userDefinedIndex].varName, token[0]);
			   strcpy (ud[userDefinedIndex++].varType, "Data Byte");
			   
			   if(token[2][0]=='\'')
			   {
				   entSymtab(token[0],1,strlen(token[2])-2,0);
				   locctr+=(strlen(token[2])-2);
			   }

			   else if(!strncmp(token[3],"dup",3))//strncmp(token[3],"dup",3)==0)
			   {
				   entSymtab(token[0],1,atoi(token[2]),0);
				   locctr+=atoi(token[2]);

				   if(!strcmp(token[4],"?"))
				   {
					   strcpy(token[4],"00");				   
					   strcpy(Line[linenum].word[4],"00");
				   }   				   
			   }
			   
			   else if(toknum<4) 
			   {
				   entSymtab(token[0],1,1,0);
				   locctr+=1;
			   }

			   else if(!strcmp(token[3],","))
			   {
				   int j=0,i;
				   for(i=2;i<=toknum;i+=2,j++)
                                        ;
				   entSymtab(token[0],1,j,0);
                                           for(i=2;i<=toknum;i+=2) {
					   locctr+=1;
				   }
				   
			   }
                          
			   
		   }

		   else if(strcmp(token[1],"dw")==0)
		   {
			   strcpy (ud[userDefinedIndex].varName, token[0]);
			   strcpy (ud[userDefinedIndex++].varType, "Data Word");
			   if(!strncmp(token[3],"dup",3))
			   {
				   entSymtab(token[0],2,dbsiz(atoi(token[2])),0 );
				   locctr+=2*atoi(token[2]);
			   }
			   else if(toknum<4) 
			   {
				   entSymtab(token[0],2,1,0);
				   locctr+=2;
			   }
			   else if(!strcmp(token[3],","))  
			   {
				   int j=0,i;
				   for(i=2;i<=toknum;i+=2,j++);
				   
				   entSymtab(token[0],2,j,0);

				   for(i=2;i<=toknum;i+=2)
				   {
					   locctr+=2;
				   }
			   }
		   }
		   break;
		   
	   default:
		   break;
		   
	   }
}

void resettokens()
{
	for(int i=0;i<20;i++)
		strcpy(token[i],"\0");
}


void setlineinfo()
{
	Line[linenum].wordnum=toknum;
	Line[linenum].mod=10;// >5
	Line[linenum].d=10;
	Line[linenum].loc=locctr;
	Line[linenum].is_instruc=0;
	strcpy(Line[linenum].dispval,"\0");
	
	for(int i=0;i<20;i++)
		strcpy(Line[linenum].word[i],"\0");
	
	for(int i=0;i<toknum;i++)
		strcpy(Line[linenum].word[i],token[i]);
	
	Line[linenum].op1.reg8=0;
	Line[linenum].op1.reg16=0;
	Line[linenum].disp=0;
	Line[linenum].op1.seg=0;
	Line[linenum].op1.mem=0;
	Line[linenum].imm=0;
	Line[linenum].var=0;
	Line[linenum].index=0;
	Line[linenum].base=0;
	Line[linenum].op2.reg8=0;
	Line[linenum].op2.reg16=0;
	Line[linenum].op2.seg=0;
	Line[linenum].op2.mem=0;
	Line[linenum].w=0;
	
}

void addzeros()
{
	fputc(0x00,objmem);
	fputc(0x00,objmem);
}

int pass1(const char *listname)
{
	register int i;
	locctr=0;
	linenum=0;
	toknum=0;

	curf=fopen("tempbuf2.asm","rb");

	while(!feof(curf))
	{
		int flag;
		fgets(asmbuf,80,curf);linenum++;
		resettokens();
		tokenize();
		setlineinfo();
		flag=-1;
		if(strcmp(token[0],"data")==0 && strcmp(token[1],"segment")==0)
		{
			entSymtab(token[0],0,0,0);
			while(!(!(strcmp(token[0],"data")) && !(strcmp(token[1],"ends"))))
			{
				resettokens();
				flag=-1;
				fgets(asmbuf,80,curf);linenum++;
				tokenize();
				setlineinfo();
				flag=check();

				decideother(flag);

				if(feof(curf)){errmsg();cout<<"  End of data segment not found\n";exit(0);}
			}
			resettokens();
			dlocctr=locctr;
			locctr=0;
		}

		flag=check();
		if(flag==0)
		{
			Line[linenum].is_instruc=1;
			flag=isValidInstruction(token[0]);
			decideinstruc(flag);
		}

	}//(!feof)

	maxlines=linenum;

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

	}
	else extra=0;

	int t=dlocctr+extra+locctr;

	int s=t/512;
	int lmod=(t)/512+2;

	int m=dlocctr%16;
	m=(extra+dlocctr)/16;

	int seg;
	if(dlocctr%16 != 0)
		seg=dlocctr/16+1;
	else seg=dlocctr/16 ;

	fputc(0x4d,objmem);
	fputc(0x5a,objmem);

	fputc(t,objmem);//t
	fputc(s,objmem);

	fputc(lmod,objmem);//lmod
	fputc(0x00,objmem);

	fputc(relocatable,objmem);//6
	fputc(0x00,objmem);//7

	fputc(0x20,objmem);
	fputc(0x00,objmem);

	addzeros();//0a  //0b

	fputc(0xff,objmem);
	fputc(0xff,objmem);

	addzeros();//0e //0f

	addzeros();//10 //11

	addzeros();

	addzeros();

	fputc(m,objmem);//16
	fputc(0x00,objmem);//17

	fputc(0x3e,objmem);
	fputc(0x00,objmem);

	addzeros();//ia //ib

	for(i=0;i<6;i++)
	{
		fputc(0x00,objmem);
	}

	fputs("by SUMIassm",objmem);

    for(i=0;i<12;i++) {
		fputc(0x00,objmem);
	}

    for(i=0;i<relocatable;i++) {
		fputc(reloc[i]+1,objmem);
		fputc(0x00,objmem);
		fputc(seg,objmem);
		fputc(0x00,objmem);
	}

	for(i=0;i<512-(62+4*relocatable);i++)
	{
		fputc(0x00,objmem);
	}

	fclose(curf);
	for(int pp=1;pp<maxlines;pp++)
	{
		interf(pp);
	}
	return 0;
}



