// mlex_genc_test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

extern "C" { 
	bool mlex_read(char* s);
}

char* test_strings[] = {
	"joker",
	"jokes",
	"jokex",
	"jccc",
	"Aa",
	"AbAcAd",
	"Aaaaa",
	"AbAcAAA",
	".3",
	"0.12",
	"0.123",
	"a.ddd",
	"acdqr",
	"cds4",
	"cdsdd",
	"_bcde"
};

int main()
{
	for (int i = 0;i < sizeof(test_strings) / sizeof(char*);i++) {
		if (mlex_read(test_strings[i]))
			printf("%sƥ����ȷ\n", test_strings[i]);
		else
			printf("%sƥ��ʧ��\n", test_strings[i]);
	}

    return 0;
}

