// MACROPROCESSOR: For Macro Detection and Expansion...
// Also Removes Comments

//#include<conio.h>
#include<string>
#include<stdio.h>
#include<cctype>
#include<iostream>
#include<string.h>
using namespace std;
int macnum=0;
int errnum=0;
char asmbuf[80];
FILE *source,*temp,*mfp;
char macrofname[20];
unsigned int userDefinedIndex;

class userDefinedVar {
	public:
	char varName[100];
	char varType[100];
} ud[20];

int linenum=0;

class mactab{
	public:
	char name[10];
	int name_size;
	char arglist[5][10];
	long startptr;
	long endptr;
}mac[5];


void while_Null(int& a)
{
	while(asmbuf[a]==' ')
		a++;
}

void while_Char_Present(int& a)
{
	while(isalnum(asmbuf[a]) || asmbuf[a]=='\'' || asmbuf[a]=='+' || asmbuf[a]=='[' || asmbuf[a]=='_' || asmbuf[a]==']')
	{
		if(asmbuf[a]==';')return;
		a++;
	}
}

int charcheck(char buffer[],int &ii,int &i)
{
	if(asmbuf[i]==',')
	{
		return 1;
	}
	while(isalnum(asmbuf[i]) || asmbuf[i]=='\'' || asmbuf[i]=='+' || asmbuf[i]=='[' || asmbuf[i]=='_' || asmbuf[i]==']')
	{
		buffer[ii++] = asmbuf[i++];
		buffer[ii]='\0';
		if(asmbuf[i]==';')return 0;
		else;
	}
	buffer[ii]='\0';
	if(asmbuf[i]==',')
	{
		return 1;
	}
	return 0;
}

int macropro(const char* Fname)
{
	static char charin[80];
	static char newbuf[5][10];
	source=fopen(Fname,"rb");
	mfp = fopen (macrofname, "wb");
	temp=fopen("tempbuf.asm","wb");
	
	while(!feof(source)){
		
		++linenum;
		strcpy(asmbuf,"  ");
		fgets(asmbuf,80,source);
		
		int i=0;
		
		while_Null(i); int i1=i;
		while_Char_Present(i); int i2=i;
		while_Null(i); int i3=i;

		int j=0;
		
		if(asmbuf[i1]==';'){fputs("\n",temp);}//do nothing;
		
		else if((strncmp(asmbuf+i3,"macro",5)==0) )//&& (asmbuf[i3+5]==' '|| asmbuf[i3+5]=='\t'))
		{
			fputs("\n",temp);
			for(int ii=i1; ii<i2; ii++,j++)
			{
				mac[macnum].name[j]=asmbuf[ii];
				mac[macnum].name_size=j+1;
			}
			if(macnum!=0)
			{
				for(i=0;i<macnum;i++)
				{
					if( strcmp(mac[macnum].name , mac[i].name)==0 )
					{
						cout<<"\n\nERROR:Line "<<linenum<<" Redeclaration of macro not allowed"<<endl;
						errnum++;
					}
				}
			}
			int argcount=0;
			for(i=i3+5;i<80;i++)
			{
				int ii=0;
				while_Null(i);
				if(i>=80)break; 
				if(asmbuf[i]=='\0')break;
				charcheck(mac[macnum].arglist[argcount],ii,i);
				if(ii>1) ++argcount;
				if(i>=80)break;
				if(asmbuf[i]=='\0')break;
				
			}
			
			i=0;
			mac[macnum].startptr=ftell(source);
			long curpos;

			fprintf (mfp, "\n\nMacro name = %s\n\n", mac[macnum].name	);
			while(!feof(source))
			{
				
				fgets(asmbuf,80,source);
				fprintf (mfp, "\n%s", asmbuf);
				++linenum;
				i=0;
				while_Null(i);
				int i1=i;
				if(asmbuf[i1]==';'){continue;}//do nothing;
				else
				if(strncmp(asmbuf+i1,"endm",4)==0)
				{
					fputs("\n",temp);
					mac[macnum].endptr=curpos;
					break;
				}
				else
				{
					curpos=ftell(source);
					fputs("\n",temp);
				}
				
			}//while ends
			++macnum;
			
		}//if check for macro ends
		else 
			fputs(asmbuf,temp);
	}//feof(source) ends
	
	fclose(temp);
	fclose(source);
	fclose(mfp);
////////////////////////////////////////////////////////////////////////////	
	source=fopen("tempbuf.asm","rb");
	temp=fopen("tempbuf2.asm","wb");
	FILE *initsource=fopen(Fname,"rb");
/////////////////////////////////////////////
	printf("\nmacnum  %d\n",macnum);
	while(!feof(source)){
		int mac_size;
		strcpy(charin," ");
		fgets(asmbuf,80,source);
		int i=0;
		int called=0;
		int macronum;
		while_Null(i);int i1=i;
		for(int p=0;p<macnum;p++)
		{
			if((strncmp(asmbuf+i1,mac[p].name,mac[p].name_size)==0))
			{
				mac_size=mac[p].name_size;
				called=1;
				macronum=p;
			}
		}
		if(called==1)
		{
			int j=0;
			for(i=i1+mac_size;i<80;i++)
			{
				int ii=0;
				while_Null(i);
				if(i>=80)break;
				if(asmbuf[i]=='\0')break;
				if(asmbuf[i] != '\t'&& asmbuf[i] != ' '&& asmbuf[i] != ',')
				{
					while(isalnum(asmbuf[i]))
					{
						newbuf[j][ii++] = asmbuf[i++];
						newbuf[j][ii]='\0';
						if(asmbuf[i]=='\0')break;
						else;
					}
				}
				newbuf[j][ii]='\0';
				if(ii>1) j++;
				if(i>=80)break;
				if(asmbuf[i]=='\0')break;
			}//end for(i=i1+mac_size;i<80;i++)
			
			fseek(initsource,mac[macronum].startptr,0);
			strcpy(charin,"\0");
			while(ftell(initsource)<mac[macronum].endptr)
			{
				fgets(asmbuf,80,initsource);//fgets(asmbuf,80,initsource);
				fputs(asmbuf,stdout);
				char buffer[20];
				strcpy(charin,"\0");
				for(i=0;i<80;i++)
				{
					int ii=0, comma=0;
					int flag=-1;
					while_Null(i);
					if(i>=80)break;
					if(asmbuf[i]=='\0')break;
					comma=charcheck(buffer,ii,i);
					
					buffer[ii]='\0';
					flag=-1;
					for(int jj=0;jj<5;jj++)
					{
						if(strcmp(mac[macronum].arglist[jj],buffer)==0)
							flag=jj;
					}
					if(flag!=-1)
						strcat(charin,newbuf[flag]);
					else
						strcat(charin,buffer);
					
					if(comma==1)
						strcat(charin,",");
					else
						strcat(charin," ");
					
					if(ii>1) j++;
					if(i>=80)break;
					if(asmbuf[i]=='\0')break;
				}//end for(i=0;i<80;i++)
				strcat(charin,"\n");
				fputs(charin,temp);
			}//end  while(ftell)
			
		}//end if(called)
		else fputs(asmbuf,temp);
	}//end while(!feof(source))

	for (int i = 0; i < macnum; ++i)
	{
		strcpy (ud[userDefinedIndex].varName , mac[i].name);
		strcpy (ud[userDefinedIndex++].varType , "Macro");
	}
	fclose(source);
	fclose(temp);
	fclose(initsource);
	return 0;
}















