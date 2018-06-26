// mlex.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "dfa.h"

#include "gencode\mlex-genc.h"

#include <fstream>

int main()
{

	fstream fs("lex-test\\common.lex",ios::in);

	char* test_strings[] = {
		"joker",
		"Aaaaa",
		"0x1234",
		"0X1234",
		".3",
		"012",
		"123",
		"a.ddf",
		"addf",
		"a_cce",
		"_bcde",
		"+",
		">=",
		"<=",
		"\"ddsf\""
	};

	if (fs.good()) {

		mlex::MlexRegexp regexp(fs);

		mlex::MlexNfa nfa(regexp);

		nfa.convert();

		mlex::MlexDfa dfa(nfa);

		dfa.convert();

		dfa.simplify(false);

		mlex::MlexCodeGen_C genc(dfa);

		fstream genf("lex-test\\genc.c", ios::out | ios::trunc);

		genf << genc.gencode();

		for (int i = 0;i < sizeof(test_strings) / sizeof(char*);i++) {
			mlex::MlexRegexpContext re;
			bool r = dfa.validateString(string(test_strings[i]), re);
			if (r) {
				printf("%s:ƥ����ȷ,���ʽ%s\n", test_strings[i], re._regExp.c_str());
			}
			else {
				printf("%s:ƥ�����\n", test_strings[i]);
			}
		}
	}

    return 0;
}

