#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decode196(char *str);
void decode200(char *str);
void Rnd();
void copy(char *s, char *s1, int len);


struct RandomNext {
	double		result;
	unsigned int	next;
} s_rand;

void main(int argc, char **argv)
{
	if (argv[1] == NULL || strlen(argv[1]) < 5){
		printf("N点虚拟主机解密工具1.0 \n支持1.9和2.0版本 by M\n用法:N_Decode.exe pass \n");
		exit(0);
	}
	else if (strlen(argv[1]) > 0x300){
		printf("Fail!\n");
		exit(0);
	}
	else if (strstr(argv[1], "@") && strlen(argv[1]) > 26){
		char * str = *(argv + 1) + 26; /* 截取字符串 */
		decode196(str);
	}
	else{
		decode200(argv[1]);
	}
}


void decode196(char *str){

	int tmp, tmp2, str_len = strlen(str), str_len2 = str_len / 2;
	int *pass1 = (int *)calloc(str_len2, sizeof(int));

	char *result = (char *)calloc(str_len2, sizeof(char));
	int	div = 0xD, FLAG = 0;
	int	MOD = 0;

	for (int i = 0; i < str_len2; i++){

		tmp = (*(str++) - 0x40) * 0x10;
		FLAG = 0;
		if (!(tmp >= 0xFF)){
			tmp = (tmp >> 4);   /* 取位； */
			FLAG = 1;
		}
		tmp2 = *(str++) & 0x0F;         /* 取位 */
		if (FLAG)
			pass1[i] = tmp * 0x10 + tmp2;
		else
			pass1[i] = tmp + tmp2;
	}

	MOD = *(pass1 + str_len2 - 1);          /* 最后一位 */

	for (int i = str_len2 - 2; 0 <= i; i--){
		tmp = *(pass1 + i) + (MOD * 0x100);
		*(pass1 + i) = tmp / div;
		MOD = tmp % div;
		//printf("%02X,%02X,%02X\n", tmp, MOD, *(pass1 + i));
	}

	s_rand.next = 0x0805E86;

	for (int i = 0; i < str_len2 - 1; i++){
		tmp = pass1[i];
		if (tmp < 0x20 || tmp > 0x7e ){
			printf("Fail!\n");
			exit(0);
			/* #不是字母 */
		}

		Rnd(); /* 取数据； */
		tmp -= 0x20;
		tmp2 = (int)(s_rand.result * 96.0);
		tmp2 = tmp - tmp2;

		if ((tmp2 & 0xFFFF)>0x7FFF){
		 tmp2 += 0X5F;
		 }
		 tmp2 += 0X20;


		result[i] = tmp2;
	}
	char **pass2 = (char **)calloc(3, sizeof(char**));
	pass2[0] = strtok(result, "@");
	if (strlen(pass2[0]) == str_len2 - 1){
		printf("Fail!\n");
		exit(0);
	}
	pass2[1] = strtok(NULL, "@");
	pass2[2] = strtok(NULL, "@");
	printf("\nPassword[*]:");
	for (int i = 0; i < atoi(*(pass2 + 2)) * 2; i++, i++){
		printf("%c", pass2[1][i]);
	}
	printf("\n");
	free(pass2);
}


void decode200(char *str){
	int passlen = strlen(str) - 6;
	if (!passlen){
		printf("\nPassword[*]:NULL");
		exit(0);
	}
	*(str + passlen) = 0x00;

	int div = strtol(str + passlen - 3, NULL, 16);
	if (!div){
		printf("Fail!\n");
		exit(0);
	}

	char *find = str + passlen - 3;

	char *szBuf = (char *)calloc(4, sizeof(char));
	copy(szBuf, str + 4, 3);

	if (strtol(szBuf, NULL, 16) != div){
		printf("Fail!\n");
		exit(0);
	}
	char	*len = NULL;
	int	len1 = 0;
	printf("\nPassword[*]:");
	for (int i = 0; i < (passlen); i += 3){
		len = strstr(str + i, find);
		if (len){
			len1 = len - (str + i);
			copy(szBuf, str + i, len1);
			i += len1;
			len1 = strtol(szBuf, NULL, 16);
			if (!len1)
				break;
			printf("%c", (char)(len1 / div));
		}
		else{
			break;
		}
	}
	free(szBuf);
}


void Rnd(){
	unsigned int	_ecx = 0x2bc03 * s_rand.next;
	unsigned int	_eax = 0xFFC39EC3;
	unsigned int	_edx = 0X00FFFFFF;

	_eax -= _ecx;
	_eax &= _edx;

	s_rand.result = (double)_eax * 5.960464E-08;
	s_rand.next = _eax;
}


void copy(char *s, char *s1, int len){
	for (int i = 0; i < (int)strlen(s); i++)
		*(s + i) = 0x0;

	for (int i = 0; i < len; i++)
		*(s + i) = *(s1 + i);
}