#include "StdAfx.h"
#include "utils.h"
#include "msg.h"
#include "about.h"
#include "debug.h"
#include "comm.h"
#include "../res/resource.h"

namespace Common {
	unsigned int c_text_formatting::remove_string_crlf( char* str )
	{
		char* p1 = str;
		char* p2 = str;

		while(*p2){
			if(*p2=='\r' || *p2=='\n'){
				p2++;
			}else{
				*p1++ = *p2++;
			}
		}
		*p1 = '\0';
		return (unsigned int)p1-(unsigned int)str;
	}

	unsigned int c_text_formatting::remove_string_cr(char* str)
	{
		char* p1 = str;
		char* p2 = str;

		while (*p2){
			if (*p2 == '\r'){
				p2++;
			}
			else{
				*p1++ = *p2++;
			}
		}
		*p1 = '\0';
		return (unsigned int)p1 - (unsigned int)str;
	}

	unsigned int c_text_formatting::remove_string_lf( char* str )
	{
		char* p1 = str;
		char* p2 = str;

		while(*p2){
			if(*p2=='\n'){
				p2++;
			}else{
				*p1++ = *p2++;
			}
		}
		*p1 = '\0';
		return (unsigned int)p1-(unsigned int)str;
	}

	unsigned char val_from_char(char c)
	{
		if(c>='0' && c<='9') return c-'0';
		else if(c>='a' && c<='f') return c-'a'+10;
		else if(c>='A' && c<='F') return c-'A'+10;
		else return 0;
	}

	int char_oct_from_chars(const char* str, unsigned char* poct)
	{
		unsigned char oct = 0;
		int i;

		for(i=0; i<3 && (*str>='0' && *str<='7'); i++,str++){
			oct *= 8;
			oct += *str-'0';
		}

		*poct = oct;
		return i;
	}

	int read_integer(const char* str, int* pi)
	{
		int r = 0;
		const char* p = str;

		while (*p >= '0' && *p <= '9'){
			r *= 10;
			r += *p - '0';
			p++;
		}

		*pi = r;
		return (int)p - (int)str;
	}

	unsigned int c_text_formatting::parse_string_escape_char( char* str )
	{
		char* p1 = str;
		char* p2 = str;

		while(*p2){
			if(*p2 == '\\'){
				p2++;
				switch(*p2)
				{
				case '\\':*p1++ = '\\';p2++;break;
				case '\'':*p1++ = '\''; p2++; break;
				case '\"':*p1++ = '\"'; p2++; break;
				case 'b':*p1++  = '\b';p2++;break;
				case 'a':*p1++  = '\a';p2++;break;
				case 'v':*p1++  = '\v';p2++;break;
				case 't':*p1++  = '\t';p2++;break;
				case 'n':*p1++  = '\n';p2++;break;
				case 'r':*p1++  = '\r';p2++;break;
				case 'x'://����Ƿ�Ϊ2��16�����ַ�
					{
						p2++;
						if(*p2 && *(p2+1)){
							if(isxdigit(*p2) && isxdigit(*(p2+1))){
								unsigned char hex = val_from_char(*p2);
								hex = (hex << 4) + val_from_char(*(p2+1));
								*(unsigned char*)p1 = hex;
								p1++;
								p2 += 2;
							}else{
								goto _error;
							}
						}else{
							goto _error;
						}
						break;
					}
				case '\0':
					goto _error;
					break;
				default:
					{
						// 8�����ж�
						if(*p2>='0' && *p2<='7'){
							p2 += char_oct_from_chars(p2, (unsigned char*)p1);
							p1 ++;
							break;
						}
						goto _error;
					}
				}
			}else{
				*p1++ = *p2++;
			}
		}
		*p1 = '\0';
		return 0x80000000|(unsigned int)p1-(unsigned int)str;

_error:
		return (unsigned int)p2-(unsigned int)str & 0x7FFFFFFF;
	}

	unsigned int c_text_formatting::str2hex( char* str, unsigned char** ppBuffer,unsigned int buf_size )
	{
		enum{S2H_NULL,S2H_SPACE,S2H_HEX,S2H_END};
		unsigned char hex=0;			//������������ĵ���16����ֵ
		unsigned int count=0;			//���������16���Ƶĸ���
		unsigned char* hexarray;		//����ת����Ľ��
		unsigned char* pba;				//������hexarray��д����
		unsigned char* pp = (unsigned char*)str;	//���������ַ���

		int flag_last=S2H_NULL,flag;				//�ʷ������õ��ı��λ

		if(str==NULL) return 0;
		//������2���ַ�+���ɿհ����һ��16����, ������಻���ܳ���(strlen(str)/2)
		if(*ppBuffer && buf_size>=strlen(str)/2){
			hexarray = *ppBuffer;
		}else{
			hexarray = new unsigned char[strlen(str)/2];
		}
		pba = hexarray;

		for(;;){
			if(*pp == 0) 
				flag = S2H_END;
			else if(isxdigit(*pp)) 
				flag = S2H_HEX;
			else if(*pp==0x20||*pp==0x09||*pp=='\r'||*pp=='\n')
				flag = S2H_SPACE;
			else{
				//printf("�Ƿ��ַ�!\n");
				goto _parse_error;
			}

			switch(flag_last)
			{
			case S2H_HEX:
				{
					if(flag==S2H_HEX){
						hex <<= 4;
						if(isdigit(*pp)) hex += *pp-'0';
						else hex += (*pp|0x20)-87;
						*pba++ = hex;
						count++;
						flag_last = S2H_NULL;
						pp++;
						continue;
					}else{
						//printf("������!\n");
						goto _parse_error;
					}
				}
			case S2H_SPACE:
				{
					if(flag == S2H_SPACE){
						pp++;
						continue;
					}else if(flag == S2H_HEX){
						if(isdigit(*pp)) hex = *pp-'0';
						else hex = (*pp|0x20)-87;  //'a'(97)-->10
						pp++;
						flag_last = S2H_HEX;
						continue;
					}else if(flag == S2H_END){
						goto _exit_for;
					}
				}
			case S2H_NULL:
				{
					if(flag==S2H_HEX){
						if(isdigit(*pp)) hex = *pp-'0';
						else hex = (*pp|0x20)-87;
						pp++;
						flag_last = S2H_HEX;
						continue;
					}else if(flag == S2H_SPACE){
						flag_last = S2H_SPACE;
						pp++;
						continue;;
					}else if(flag==S2H_END){
						goto _exit_for;
					}
				}
			}
		}
	_parse_error:
		if(hexarray != *ppBuffer){
            delete[](unsigned char*)hexarray;
		}
		return 0|((unsigned int)pp-(unsigned int)str);
	_exit_for:
		//printf("������:%d\n",pba-(unsigned int)ba);
		*ppBuffer = hexarray;
		return count|0x80000000;
	}

	static __inline void hex2chs_append_nl(unsigned char** pp, c_text_formatting::newline_type nlt)
	{
		switch(nlt)
		{
		case c_text_formatting::newline_type::NLT_CR:
			**pp = '\r';
			++*pp;
			break;
		case c_text_formatting::newline_type::NLT_LF:
			**pp = '\n';
			++*pp;
			break;
		case c_text_formatting::newline_type::NLT_CRLF:
			**pp = '\r';
			++*pp;
			**pp = '\n';
			++*pp;
			break;
		}
	}

	char* c_text_formatting::hex2chs( unsigned char* hexarray,int length,char* buf,int buf_size, enum newline_type nlt )
	{
		char* buffer=NULL;
		unsigned char* p;
		int total_length; // �����������ܳ���,����pchָ���ж�
		int cnt_n = 0;

		//�������ϸ���������ֵĸ���
		//��ʵ������������ƿռ�
		do{
			int i;
			for(i=0; i<length; i++){
				if(hexarray[i]=='\r' || hexarray[i]=='\n'){
					cnt_n++;
				}
			}
		}while(0);

		total_length = (length-cnt_n)*1 //��'\r','\n'
			+ cnt_n * (nlt == NLT_CRLF ? 2 : 1)
			+ 1;

		if(total_length<=buf_size && buf){
			buffer = buf;
		}else{
			buffer = new char[total_length];
		}

		p = (unsigned char*)buffer;
		do{
			int i, step;
			for(i=0; i<length; i+=step+1){
				step=0;
				if(hexarray[i]=='\r'){
					hex2chs_append_nl(&p, nlt);
					if(i<length-1 && hexarray[i+1]=='\n'){
						step++;
						if( (i<length-2 && hexarray[i+2]=='\r') 
							&& ((i==length-3) || (i<length-3 && hexarray[i+3]!='\n')) )
						{
							step++;
						}
					}
				}
				else if(hexarray[i]=='\n'){
					hex2chs_append_nl(&p, nlt);
				}
				else if(hexarray[i]==0){
					// �����Ҫ"������"����ʾ'\0', ���������ﴦ��
					// ����ǰ����϶�'\0'�����ļ���
				}
				else{
					*p++ = hexarray[i];
				}
			}
		}while((0));

		*p++ = '\0';
		return buffer;
	}

	char* c_text_formatting::hex2str(unsigned char* hexarray, int* length, int linecch, int start, char* buf, int buf_size, enum newline_type nlt)
	{
		char* buffer = NULL;
		char* pb = NULL;
		int count = start;
		int total_length;
		int k;
		int nltsz = nlt == newline_type::NLT_CRLF ? 2 : 1;

		//2013-01-17���¼������:
		//	ÿ�ֽ�ռ��2��ASCII+1���ո�:length*3
		//  �����ַ�ռ��:length/linecch*nltsz
		if (linecch){
			total_length = *length * 3 + *length / linecch * nltsz + 1 + nltsz;//+1:���1��'\0';+nltsz:�����ǵ�1��\r\n
		}
		else{
			total_length = *length * 3 + 1 + nltsz;//+1:���1��'\0';+2:�����ǵ�1��\r\n
		}
		if (buf_size >= total_length && buf){
			buffer = buf;
		}
		else{
			buffer = new char[total_length];
		}
		for (k = 0, pb = buffer; k < *length; k++){
			sprintf(pb, "%02X ", hexarray[k]);
			pb += 3;
			//���д���
			if (linecch && ++count == linecch){
				hex2chs_append_nl((unsigned char**)&pb, nlt);
				count = 0;
			}
		}
		*pb = '\0';
		*length = pb - buffer;
		return buffer;
	}

	void set_clipboard_data(const char* str)
	{
		if (!str || !*str) return;

		if (OpenClipboard(NULL)){
			HGLOBAL hMem = NULL;
			int len;

			len = strlen(str) + 1;
			hMem = GlobalAlloc(GHND, len);
			if (hMem){
				char* pmem = (char*)GlobalLock(hMem);
				EmptyClipboard();
				memcpy(pmem, str, len);
				SetClipboardData(CF_TEXT, hMem);
			}
			CloseClipboard();
			if (hMem){
				GlobalFree(hMem);
			}
		}
	}

	void split_string(std::vector<std::string>* vec, const char* str, char delimiter)
	{
		const char* p = str;
		std::string tmp;
		for (;;){
			if (*p){
				if (*p != delimiter){
					tmp += *p;
					p++;
					continue;
				}
				else{
					vec->push_back(tmp);
					tmp = "";
					p++;
					continue;
				}
			}
			else{
				if (tmp.size()) vec->push_back(tmp);
				break;
			}
		}
	}
}

