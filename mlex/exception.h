
#pragma once

#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <stack>

using namespace std;

namespace mlex {

	enum class MlexExceptionLevel {
		Warning,
		Error
	};

	enum class MlexExceptionType {
		InvalidExpression,
		UnrecognizedChar,
		NoMoreItems,
		InvalidInput,
		ErrorInNfaToDfa
	};

	using ExceptionUtype = underlying_type<MlexExceptionType>::type;

	class MlexException {
	public:
		MlexExceptionLevel _level;
		MlexExceptionType _type;
		string _desc;

		MlexException(MlexExceptionLevel Level, MlexExceptionType Type) :_level(Level), _type(Type) {
		}

		MlexException(MlexExceptionLevel Level, MlexExceptionType Type, string Desc) :_level(Level), _type(Type), _desc(Desc) {
		}

		string& translate(MlexExceptionType Type) {

			string human_friendly;
			switch (Type)
			{
			case mlex::MlexExceptionType::InvalidExpression:
				human_friendly = "���ʽ����";
				break;
			case mlex::MlexExceptionType::UnrecognizedChar:
				human_friendly = "�޷�ʶ����ַ���";
				break;
			case mlex::MlexExceptionType::NoMoreItems:
				human_friendly = "����Խ�硣";
				break;
			case mlex::MlexExceptionType::InvalidInput:
				human_friendly = "����������";
				break;
			default:
				human_friendly = "δ֪����";
				break;
			}

			return human_friendly;
		}

		string& toString() {

			string ret;
			switch (_level)
			{
			case MlexExceptionLevel::Warning:
				ret = "���棨" + to_string(ExceptionUtype(_type)) + "��:" + translate(_type) + _desc;
				break;
			case MlexExceptionLevel::Error:
				ret = "����" + to_string(ExceptionUtype(_type)) + "��:" + translate(_type) + _desc;
				break;
			default:
				break;
			}
			
			return ret;
		}
	};

	stack<MlexException> MlexExceptionStack;

}