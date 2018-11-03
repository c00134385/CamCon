#include "stdlib.h"
#include "string.h"
#include "integer.h"

#pragma pack(1)



char* float2str(double db_dt)
{
    static char float_str[17];	 //doubleռ��8bytes,���λ��16λ.
    
    sprintf(float_str,"%f",db_dt);
    
    return float_str;
}


/*----------------------------------------------------------------------------*/
/* ��������: bool DecChar2Int(char *in, int *re_int, int k)                   */
/* ��������: ��ʮ���Ƶ��ַ�ת����int                                          */
/*           *in: �����ַ�����ָ��                                            */
/*           *re_int: ����int�����ݵ����ָ��λ��                             */
/*           k: ת�����ַ�λ��, ����Ҫ����"12", ��K=2                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/

bool isdec(char in)
{
	if( (in>=0x30)&&(in<=0x39) )
		return true;
	else
		return false;
}

int isdecstring(const char *in)
{
    char *p= (char*)in;
    char tmp;
    int len=0;
    
    do{
        tmp = *p++;
        //��������
        if( tmp==0 ){
            break;
        //�м���ӷ������ַ�
        }else if( isdec(tmp)==false ){
            len = 0;
            break;
        }else{
            len++;
            if( len>10 ){
                len = 0;    //����5λ����,unsigned long 0~4394967295
            }
        }
    }while(1);
    
    return len;
}

//����,��ĸ,����
bool ischar(char in)
{
    if( ((in>='0')&&(in<='9'))  //������
      ||((in>='a')&&(in<='z'))  //��Сд��ĸ
      ||((in>='A')&&(in<='Z'))  //�Ǵ�д��ĸ
      ||((in=='"'))             //
      ||((in=='.')) )           //С����
    {
        return true;
	}

    return false;        
}

bool isalphabet(char in) {
    if(((in>='a')&&(in<='z')) || ((in>='A')&&(in<='Z'))) {
        return true;
    } else {
        return false;
    }
}

bool isnumber(char in) {
    if((in>='0')&&(in<='9')) {
        return true;
    } else {
        return false;
    }
}

unsigned int DecStr2Int(char *in, unsigned char k)
{
    char i=0;
	unsigned char B[5];
    unsigned int c_int=0;

    if( (k<1)||(k>5) )
        return false;

	B[0] = *in++;
	B[1] = *in++;
	B[2] = *in++;
	B[3] = *in++;
	B[4] = *in++;

	if( isdec(B[0]) ){
		i++;
		if( isdec(B[1]) ){
			i++;
			if( isdec(B[2]) ){
				i++;
				if( isdec(B[3]) ){
					i++;
					if( isdec(B[4]) ){
					    i++;
					}
				}
			}
		}
	}

	if( k>i )
		k = i;

	i = 0;

    if( k==5 )
        c_int = (B[i++]&0x0f)*10000;

    if( k==4 )
		c_int = (B[i++]&0x0f)*1000;

    if( k>=3 )
		c_int += (B[i++]&0x0f)*100;

    if( k>=2 )
		c_int += (B[i++]&0x0f)*10;

    if( k>=1 )
		c_int += (B[i++]&0x0f);


    return c_int;
}

char* Int2DecStr(int num)
{
    static char str[6];
    char *p = &str[5];
    int k;
    bool flag = false;

    *p = '\0';

    if(num < 0)
    {
        flag = true;
        num = -num;
    }

    if(num == 0)
    {
        *(--p) = '0';
        return p;
    }
    
    while(num > 0)
    {
        k = num % 10;
        *(--p) = k + '0';
        num /= 10;
    }

    if(flag)
    {
        *(--p) = '-';
    }

    return p;
        
    
    #if 0
    static char str[6];
    char *p=str;
    int k;
    
    
    k = num/10000;
    if(k>0){
        *p++ = k+'0';
        num = num-k*10000;
    }
    
    k = num/1000;
    if(k>0){
        *p++ = k+'0';
        num = num-k*1000;
    }
    
    k = num/100;
    if(k>0){
        *p++ = k+'0';
        num = num-k*100;
    }
    
    k = num/10;
    if(k>0){
        *p++ = k+'0';
        num = num-k*10;
    }
    
    *p++ = num%10+'0';
    
    *p = '\0';

	return str;
    #endif
}





#if 0
void test_arm_lib(void)
{
    char str_tmp[100];
    char *str_p="1.23456789";
    double double_i;
    float float_i;
    
    sprintf(str_tmp,"%s%d%s%d","bytes of double:",sizeof(double_i)," bytes of float:",sizeof(float_i));
    printf(str_tmp);    printf("\r\n");
	//
	double_i = 1.23456789;
	sprintf(str_tmp,"%s%f","double 1.23456789=",double_i);	printf("\r\n");
	printf(str_tmp);
	//
	sprintf(str_tmp,"%f",double_i);
	double_i = atof(str_tmp);
	sprintf(str_tmp,"%s%f","atof(double)->",double_i);	printf("\r\n");
	printf(str_tmp);

	float_i = 1.23456789;
	sprintf(str_tmp,"%s%f","float 1.23456789=",float_i);	printf("\r\n");
	printf(str_tmp);
	float_i = atof(str_p);
	sprintf(str_tmp,"%s%f","atof(float_i)->",float_i);	printf("\r\n");
	printf(str_tmp);
}
#endif
