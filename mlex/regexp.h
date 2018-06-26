#pragma once

#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

using namespace std;

namespace mlex {

	/**
	 * У����һ��Ӣ����ĸ
	 * @param	c			�ַ�
	 */
	bool isLetter(char c) {
		if ((c >= 'a'&&c <= 'z') || (c >= 'A'&&c <= 'Z')) {
			return true;
		}
		else {
			return false;
		}
	}

	/**
	 * У����һ������������
	 * @param	c			�ַ�
	 */
	bool isNumberic(char c) {
		if ((c >= '0'&&c <= '9')) {
			return true;
		}
		else {
			return false;
		}
	}

	/**
	 * У����һ������ʾ�ַ�
	 * @param	c			�ַ�
	 */
	bool isValidChar(char c) {
		if ((c >= 32) && (c <= 128)) {
			return true;
		}
		else {
			return false;
		}
	}

	/**
	 * У����һ���հ�����
	 * @param	c			�ַ�
	 */
	bool isClosure(char c) {
		if ((c == '*') || (c == '+') || (c == '?')) {
			return true;
		}
		else {
			return false;
		}
	}

	class MlexRegexpContext {
	public:
		string _regExp;
		string _genCode;

		MlexRegexpContext() {
		}

		MlexRegexpContext(string RegExp, string Code) :_regExp(RegExp), _genCode(Code) {
		}
	};

	class MlexRegexp {

	private:
		vector<MlexRegexpContext> _regExps;
		//fstream _inputStream;
		char* _inputString;

	public:
		/** 
		 * ���ַ����г�ʼ��������ʽ
		 * @param	Input			����������ʽ���ַ���
		 */
		MlexRegexp(char* Input) :_inputString(Input) {

			string sline;
			stringbuf sbuf(_inputString);
			istream is(&sbuf);

			while (!is.eof()) {
				
				getline(is, sline);
				if (sline == "")
					continue;

				size_t wspos = min(sline.find_first_of(' '), sline.find_first_of('\t'));
				string part2, part1 = sline.substr(0, wspos);
				if (wspos != size_t(-1)) {
					part2 = sline.substr(wspos, sline.size() - wspos);
					part2 = part2.substr(max(part2.find_first_not_of(' '), part2.find_first_not_of('\t')));
				}
				else {
					part2 = "";
				}

				if (validateRegExp(part1)) {
					_regExps.push_back(MlexRegexpContext(part1, part2));
				}
			}
		}

		/** 
		 * ���ļ��г�ʼ��������ʽ
		 * @param	FileStream		����������ʽ���ļ�
		 */
		MlexRegexp(fstream& FileStream) {

			string sline;

			while (!FileStream.eof()) {

				getline(FileStream, sline);
				if (sline == "")
					continue;

				size_t wspos = min(sline.find_first_of(' '), sline.find_first_of('\t'));
				string part2,part1 = sline.substr(0, wspos);
				if (wspos != size_t(-1)) {
					part2 = sline.substr(wspos, sline.size() - wspos);
					part2 = part2.substr(max(part2.find_first_not_of(' '), part2.find_first_not_of('\t')));
				}
				else {
					part2 = "";
				}
				
				if (validateRegExp(part1)) {
					_regExps.push_back(MlexRegexpContext(part1, part2));
				}
			}
		}

		MlexRegexp() :_inputString(nullptr) {
		}

		/**
		 * �򵥵�У��һ��������ʽ
		 * @param	re			���ʽ����
		 */
		bool validateRegExp(string& re) {

			uint32_t sbk = 0, mbk = 0;
			for (size_t i = 0;i < re.length();i++) {

				//�����ַ����
				//switch (re[i])
				//{
				//case '(':
				//	sbk++;
				//	continue;
				//case '[':
				//	mbk++;
				//	continue;
				//case ')':
				//	sbk--;
				//	continue;
				//case ']':
				//	mbk--;
				//}

				if (!isValidChar(re[i])) {
					throw(string(re + ":�����Ƿ��ַ���"));
				}
			}

			if (sbk != 0) {
				throw(string(re + ":δ�պ�С���š�"));
				return false;
			}

			if (mbk != 0) {
				throw(string(re + ":δ�պ������š�"));
				return false;
			}

			return true;
		}

		/**
		 * ׷��һ��������ʽ
		 * @param	re			���ʽ����
		 */
		bool appendRegExp(string re) {

			if (validateRegExp(re)) {
				_regExps.push_back(MlexRegexpContext(re, ""));
				return true;
			}
			else {
				return false;
			}
		}

		/**
		 * ׷��һ��������ʽ
		 * @param	re			���ʽ����
		 */
		bool appendRegExp(string re, string code) {

			if (validateRegExp(re)) {
				_regExps.push_back(MlexRegexpContext(re, code));
				return true;
			}
			else {
				return false;
			}
		}

		/**
		 * ��ȡ��i��������ʽ
		 * @param	i			���ʽ����
		 */
		MlexRegexpContext& getRegExp(uint16_t i) {
			
			if (i >= _regExps.size()) {
				throw("��ȡ���ʽ���硣");
			}

			return _regExps[i];
		}

		/**
		 * ��ȡ���ʽ����
		 */
		size_t getReCount() {

			return _regExps.size();
		}
	};

}