// mlex.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "gencode\mlex-genc.h"
#include "docopt\docopt.h"

//mlex --lex LEX_FILE --matchfile FILE_NAME
//--matchfile FILE_NAME	��ָ���ļ�ƥ�䡣

static const char USAGE[] =
R"(mlex

    Usage:
      mlex --lex LEX_FILE --genlang LANG [--genre|--genlex] --output OUTPUT
	  
    Options:
		-h --help				��ʾ������
		--lex LEX_FILE			LEX�����ļ���
		--genlang LANG			ָ���������ԡ�[default:C]
		--genre					��������ƥ����롣
		--genlex				���ɴʷ��������롣
		--output OUTPUT			����ļ���
)";

#include <fstream>

int main(int argc, const char** argv)
{

	std::map<std::string, docopt::value> args
		= docopt::docopt(USAGE,
		{ argv + 1, argv + argc },
			true,
			"mlex 1.0.1");

	fstream lexFile, outputFile;
	string genLang;
	bool deep;

	//���ȴ�lex�ļ�
	try {
		lexFile.open(args["--lex"].asString(), ios::in);
	}
	catch (exception e) {
		cout << "�����ļ���Ч��" << e.what() << endl;
		exit(0);
	}

	if(args.count("--genlang")==1){
		
		//��ȡ��������
		genLang = args["--genlang"].asString();
		if (genLang != "C") {
			cout << "��֧�ֵ���������:" << genLang << endl;
		}

		//��ȡ��������
		if (args.count("--genre") == 1) {
			deep = true;
		}
		else {
			deep = false;
		}

		//������ļ�
		try {
			outputFile.open(args["--output"].asString(), ios::out | ios::trunc);
		}
		catch (exception e) {
			cout << "����ļ���Ч��" << e.what() << endl;
			exit(0);
		}

		try {

			//��ʼ��������ʽ����
			mlex::MlexRegexp regexp(lexFile);

			//������ʽת��ΪNFA
			mlex::MlexNfa nfa(regexp);
			nfa.convert();

			//NFAת��ΪDFA
			mlex::MlexDfa dfa(nfa);
			dfa.convert();

			//��DFA���м�
			dfa.simplify(deep);

			if (genLang == "C") {
				mlex::MlexCodeGen_C genc(dfa);
				outputFile << genc.gencode();
				outputFile.close();
			}
		}
		catch(mlex::MlexException e){

			while (!mlex::MlexExceptionStack.empty()) {
				mlex::MlexException es = mlex::MlexExceptionStack.top();
				mlex::MlexExceptionStack.pop();
				cout << es.toString() << endl;
			}
			cout << e.toString() << endl;
			exit(0);
		}
		catch(exception e){

			while (!mlex::MlexExceptionStack.empty()) {
				mlex::MlexException es = mlex::MlexExceptionStack.top();
				mlex::MlexExceptionStack.pop();
				cout << es.toString() << endl;
			}
			cout << e.what() << endl;
			exit(0);
		}

		cout << "�����ļ����ɳɹ���" << endl;
	}
	else {

	}

    return 0;
}

