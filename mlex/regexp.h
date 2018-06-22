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

	class MlexRegexp {

	private:
		vector<string> _regExps;
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
				if (validateRegExp(sline))
					_regExps.push_back(sline);
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
				if (validateRegExp(sline)) {
					_regExps.push_back(sline);
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
				switch (re[i])
				{
				case '(':
					sbk++;
					continue;
				case '[':
					mbk++;
					continue;
				case ')':
					sbk--;
					continue;
				case ']':
					mbk--;
				}

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
				_regExps.push_back(re);
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
		string& getRegExp(uint16_t i) {
			
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