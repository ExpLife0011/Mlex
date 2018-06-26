#pragma once

#include <stack>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

#include "regexp.h"

namespace mlex {

	//״̬������������ȷ��״̬��ΨһID
	static uint32_t _stateIdCounter = 0;

	//�����빤��ջ�е�������ʽ�����
	enum class MlexReOpTypes {
		contact = 0,
		select,
		closure,
		lbracket,
		rbracket,
		opmax
	};

	enum class MlexNfaWorkEleTypes {
		StateDiagram = 0,
		Operator
	};

	/**
	 * ����ջ�е�Ԫ��
	 * ������
	 *		״̬ͼ����
	 *		�������
	 */
	class MlexNfaWorkElement {
	public:
		MlexNfaWorkEleTypes _eleType;

		MlexNfaWorkElement(MlexNfaWorkEleTypes Type) 
			:_eleType(Type) {
		}
	};

	/**
	 * ������ʽ�������
	 */
	class MlexNfaReOperator :public MlexNfaWorkElement {
		
	public:
		MlexReOpTypes _opType;
		uint16_t _num_1;
		uint16_t _num_2;

		/**
		 * ��ʼ�����Ӻ�ѡ�������
		 */
		MlexNfaReOperator(MlexReOpTypes Type) 
			:_opType(Type), MlexNfaWorkElement(MlexNfaWorkEleTypes::Operator) {
		}

		/**
		 * ��ʼ���հ������
		 * @param	MinTime			���ٱհ�����
		 * @param	MaxTime			���հ�����
		 * Ŀǰֻ֧��[0,+��),[1,+��)��{0,1}�εıհ�
		 * ��Ϊ��֪����ô�Ѵ�����Ϣת����DFA
		 */
		MlexNfaReOperator(uint16_t MinTime, uint16_t MaxTime) 
			:_opType(MlexReOpTypes::closure), _num_1(MinTime), _num_2(MaxTime), MlexNfaWorkElement(MlexNfaWorkEleTypes::Operator) {
		}
	};

	class MlexClosureCount {
	public:
		uint16_t min;
		uint16_t max;

		MlexClosureCount(uint16_t Min, uint16_t Max) 
			:min(Min), max(Max) {
		}
	};

	//����һ��NFA״̬
	class MlexNfaState{
	
	private:
		//ת̬ת����
		//multimap<char, shared_ptr<MlexNfaState>> _moveMap;
		//��������һ����ʵ�����д��ǿմ���״̬ת����ֻ����һ����
		vector<shared_ptr<MlexNfaState>> _multiMap;
		pair<char, shared_ptr<MlexNfaState>> _singleMap;
		//״̬ID
		uint32_t _stateId;
		//״̬�Ħ�-closure
		vector<shared_ptr<MlexNfaState>> _emptyClosure;

	public:
		//ȷ����ǰ״̬�Ƿ���NFA����̬
		bool _final;
		//��ǰ״̬�������������ַ�
		MlexRegexpContext _oldre;

		/**
		 * ��ʼ��һ��NFA״̬
		 */
		MlexNfaState() {
			_final = false;
			_stateId = _stateIdCounter++;
		}

		/**
		 * ��ȡNFA״̬ID
		 */
		uint32_t getStateId() {
			return _stateId;
		}

		/**
		 * ���һ����������������Input
		 * @param	Input		�����ַ�
		 * @param	State		ת�����״̬
		 */
		void addMove(char Input, shared_ptr<MlexNfaState> State) {
			pair<char, shared_ptr<MlexNfaState>> newMove(Input, shared_ptr<MlexNfaState>(State));
			if (Input != 0) {
				_singleMap = newMove;
			}
			else {
				_multiMap.emplace_back(newMove.second);
			}
		}

		/**
		 * ���һ�����հ������Ļ�
		 * @param	Input		�����ַ�
		 * @param	State		ת�����״̬
		 * @param	CCount		�հ�������Ϣ
		 */
		void addMove(char Input, shared_ptr<MlexNfaState> State, MlexClosureCount& CCount) {
			pair<char, shared_ptr<MlexNfaState>> newMove(Input, shared_ptr<MlexNfaState>(State));
			if (Input != 0) {
				_singleMap = newMove;
			}
			else {
				_multiMap.emplace_back(newMove.second);
			}
		}

		/**
		 * ��ȡ����Input����������еĻ�
		 * @param	Input		�����ַ�
		 */
		vector<shared_ptr<MlexNfaState>> getMove(char Input) {
			
			if (Input == 0) {
				return _multiMap;
			}
			else {
				vector<shared_ptr<MlexNfaState>> moveVector;
				if (_singleMap.second) {
					if ((_singleMap.first == Input) || (_singleMap.first == 1)) {
						moveVector.emplace_back(shared_ptr<MlexNfaState>(_singleMap.second));
					}
				}
				return moveVector;
			}
		}

		/**
		 * ��ȡ״̬�Ħ�-closure
		 */
		vector<shared_ptr<MlexNfaState>>& emptyClosure() {

			//����Ѿ��������
			if (_emptyClosure.size() != 0) {
				return _emptyClosure;
			}

			vector<shared_ptr<MlexNfaState>> v_first = getMove(0);
			stack<shared_ptr<MlexNfaState>> s_prep;

			//��ֱ�ӦŻ���Ӧ�ı߹ൽstack��
			for (auto iter:v_first) {
				s_prep.emplace(iter);
			}
			//���ϵ���es�е�Ԫ��
			while (!s_prep.empty()) {
				//��ȡջ��Ԫ�ص����ЦŻ���Ӧ�ı�
				shared_ptr<MlexNfaState> top = s_prep.top();
				s_prep.pop();

				v_first = top->getMove(0);
				_emptyClosure.emplace_back(top);

				for (auto iter:v_first) {
					s_prep.emplace(iter);
				}
			}
			return _emptyClosure;
		}
	};

	//����һ��NFA״̬ת��ͼ
	class MlexNfaStateDiagram :public MlexNfaWorkElement {

	private:
		shared_ptr<MlexNfaState> _startState;
		shared_ptr<MlexNfaState> _endState;

	public:
		friend class MlexDfa;

		/**
		 * ����NFA״̬ת��ͼ
		 * @param	StartState		��̬
		 * @param	EndState		��̬
		 */
		MlexNfaStateDiagram(shared_ptr<MlexNfaState> StartState, shared_ptr<MlexNfaState> EndState) 
			:_startState(shared_ptr<MlexNfaState>(StartState)), _endState(shared_ptr<MlexNfaState>(EndState)), MlexNfaWorkElement(MlexNfaWorkEleTypes::StateDiagram) {
		}

		/**
		 * ��ȡ��̬
		 */
		shared_ptr<MlexNfaState> getStartState() {
			return _startState;
		}

		/**
		 * ��ȡ��̬
		 */
		shared_ptr<MlexNfaState> getEndState() {
			return _endState;
		}

		/**
		 * ����״̬ͼ����������
		 * @param	One			һ��
		 * @param	Another		��һ��
		 * @param	join_front	��һ����һ��ǰ��
		 * 
		 * @retval	�µ�״̬ͼ
		 */
		static shared_ptr<MlexNfaStateDiagram> contact(shared_ptr<MlexNfaStateDiagram> One, shared_ptr<MlexNfaStateDiagram> Another, bool join_front) {
			if (join_front) {
				//shared_ptr<MlexNfaState> myStartState = shared_ptr<MlexNfaState>(One->_startState);
				//One._startState = Another._startState;
				//One->_startState.swap(Another->_startState);
				//Another._endState.addMove(0, myStartState);
			}
			else {
				One->_endState->addMove(0, Another->_startState);
				One->_endState = Another->_endState;
			}

			return shared_ptr<MlexNfaStateDiagram>(new MlexNfaStateDiagram(One->_startState,One->_endState));
		}

		/**
		 * ����״̬ͼ��ѡ������
		 * @param	Branchs		ѡ����������з�֧
		 *
		 * @retval	�µ�״̬ͼ
		 */
		static shared_ptr<MlexNfaStateDiagram> select(vector<shared_ptr<MlexNfaStateDiagram>>Branchs) {
			auto tmpStartState= shared_ptr<MlexNfaState>(new MlexNfaState());
			auto tmpEndState= shared_ptr<MlexNfaState>(new MlexNfaState());

			for (auto iter : Branchs) {
				tmpStartState->addMove(0, iter->_startState);
				iter->_endState->addMove(0, tmpEndState);
			}

			return shared_ptr<MlexNfaStateDiagram>(new MlexNfaStateDiagram(tmpStartState, tmpEndState));
		}

		/**
		 * ����״̬ͼ�ıհ�����
		 * @param	One			�����������״̬
		 * @param	CCount		�հ�����
		 *
		 * @retval	�µ�״̬ͼ
		 */
		static shared_ptr<MlexNfaStateDiagram> closure(shared_ptr<MlexNfaStateDiagram> One,MlexClosureCount& CCount) {
			auto tmpStartState = shared_ptr<MlexNfaState>(new MlexNfaState());
			auto tmpEndState = shared_ptr<MlexNfaState>(new MlexNfaState());


			tmpStartState->addMove(0, One->_startState);
			One->_endState->addMove(0, tmpEndState);

			if (CCount.max > 1)
				One->_endState->addMove(0, One->_startState);
			if (CCount.min == 0)
				tmpStartState->addMove(0, tmpEndState);

			return shared_ptr<MlexNfaStateDiagram>(new MlexNfaStateDiagram(tmpStartState, tmpEndState));
		}
	};

	class MlexNfa {

	private:
		//���е�������ʽ
		MlexRegexp& _regExps;
		//״̬ͼ����
		vector<shared_ptr<MlexNfaStateDiagram>> _dirgramVector;

		/**
		 * ����һ��������״̬ͼ��������̬��->Input->����̬��
		 * @param	Input		�����ַ�
		 */
		shared_ptr<MlexNfaStateDiagram> createBasicStateDiagram(char Input) {
			auto tmpStartState = shared_ptr<MlexNfaState>(new MlexNfaState());
			auto tmpEndState = shared_ptr<MlexNfaState>(new MlexNfaState());
	
			tmpStartState->addMove(Input, tmpEndState);

			return shared_ptr<MlexNfaStateDiagram>(new MlexNfaStateDiagram(tmpStartState, tmpEndState));
		}

		vector<char> readSelect(string s, size_t& pos) {
			vector<char> cv;
			char end, start;

			while (s[pos] != ']') {
				start = s[pos];
				pos++;
				
				if (s[pos] == '-') {
					pos++;
					end = s[pos];
					pos++;
				}
				else {
					end = start;
				}

				//ƥ��ʧ��
				if ((!isLetter(start) && !isNumberic(start)) ||
					(!isLetter(end) && !isNumberic(end))) {
					return cv;
				}

				for (char i = start;i <= end;i++) {
					cv.emplace_back(i);
				}

				//ȥ��
			}

			return cv;
		}

		/**
		 * ������������
		 * @param	stateDiagrams		״̬ͼ��ջ
		 */
		bool handlerContact(stack<shared_ptr<MlexNfaStateDiagram>>& stateDiagrams) {
			shared_ptr<MlexNfaStateDiagram> sd1 = move(stateDiagrams.top());
			if (sd1 == false) {
				return false;
			}
			stateDiagrams.pop();
			shared_ptr<MlexNfaStateDiagram> sd2 = move(stateDiagrams.top());
			if (sd2 == false) {
				return false;
			}
			stateDiagrams.pop();
			shared_ptr<MlexNfaStateDiagram> sd = move(MlexNfaStateDiagram::contact(sd2, sd1, false));
			stateDiagrams.emplace(sd);
			return true;
		}

		/**
		 * ����ѡ������
		 * @param	stateDiagrams		״̬ͼ��ջ
		 * @param	selects				ѡ���֧
		 */
		bool handlerSelect(stack<shared_ptr<MlexNfaStateDiagram>>& stateDiagrams, vector<shared_ptr<MlexNfaStateDiagram>>& selects) {
			if (selects.size() == 0)
				return false;
			shared_ptr<MlexNfaStateDiagram> newSd = move(MlexNfaStateDiagram::select(selects));
			selects.clear();
			stateDiagrams.emplace(newSd);
			return true;
		}

		/**
		 * ����հ�����
		 * @param	stateDiagrams		״̬ͼ��ջ
		 * @param	op					���������Ҫ���еĴ�����Ϣ
		 */
		bool handlerClosure(stack<shared_ptr<MlexNfaStateDiagram>>& stateDiagrams,shared_ptr<MlexNfaReOperator>& op) {
			shared_ptr<MlexNfaStateDiagram> sd = move(stateDiagrams.top());
			if (sd == false) {
				return false;
			}
			stateDiagrams.pop();
			MlexClosureCount count = MlexClosureCount(op->_num_1, op->_num_2);
			shared_ptr<MlexNfaStateDiagram> newSd = move(MlexNfaStateDiagram::closure(sd, count));
			stateDiagrams.emplace(newSd);
			return true;
		}

		/**
		 * �������ȼ����ڵ���op�����������
		 * @param	op			��ǰ�����
		 * @param	opStack		�����ջ
		 * @param	workStack	����ջ
		 */
		void pushOperator(shared_ptr<MlexNfaReOperator>op, stack<shared_ptr<MlexNfaReOperator>>& opStack, vector<shared_ptr<MlexNfaWorkElement>>& workStack) {
			//���ȼ���ߣ��������бհ�
			if (op->_opType == MlexReOpTypes::closure) {
				while (!opStack.empty()) {
					auto top_op = opStack.top();
					if (top_op->_opType < MlexReOpTypes::closure || top_op->_opType == MlexReOpTypes::lbracket)
						break;

					opStack.pop();
					workStack.emplace_back(top_op);
				}
				opStack.emplace(op);
				return;
			}

			//����Ԥ�㣬ֻ��Ҫ������������ӷ�
			if (op->_opType == MlexReOpTypes::contact) {
				shared_ptr<MlexNfaReOperator> top_op;
				while (!opStack.empty()) {
					top_op = opStack.top();
					if (top_op->_opType < MlexReOpTypes::contact || top_op->_opType == MlexReOpTypes::lbracket)
						break;

					opStack.pop();
					workStack.emplace_back(top_op);
				}
				opStack.emplace(op);
				return;
			}

			//ѡ�����㣬��������������
			if (op->_opType == MlexReOpTypes::select) {
				shared_ptr<MlexNfaReOperator> top_op;
				while (!opStack.empty()) {
					top_op = opStack.top();
					if (top_op->_opType < MlexReOpTypes::select || top_op->_opType == MlexReOpTypes::lbracket)
						break;

					opStack.pop();
					workStack.emplace_back(top_op);
				}
				opStack.emplace(op);
				return;
			}

			if (op->_opType == MlexReOpTypes::lbracket) {
				opStack.emplace(op);
				return;
			}

			if (op->_opType == MlexReOpTypes::rbracket) {
				shared_ptr<MlexNfaReOperator> top_op;
				while (!opStack.empty()) {
					top_op = opStack.top();
					if (top_op->_opType == MlexReOpTypes::lbracket)
						break;

					opStack.pop();
					workStack.emplace_back(top_op);
				}
				if (opStack.size() == 0 || opStack.top()->_opType != MlexReOpTypes::lbracket) {
					throw(0);
				}
				opStack.pop();

				return;
			}
		}

	public:
		//NFA����ĸ��
		vector<char> _char_tab;
		//NFA������ת̬ת��ͼ
		shared_ptr<MlexNfaStateDiagram> _main_diagram;

		MlexNfa(MlexRegexp& re) 
			:_regExps(re) {
		}

		/**
		 * ��һ��������ʽת��Ϊһ��NFA
		 * @param	re		������ʽ
		 *
		 * @retval	����������ʽ�����γɵ�NFA״̬ͼ
		 * 
		 * ���Ƚ�������ʽ����Ϊ��׺���ʽ
		 * Ȼ�󽫺�׺���ʽ��װΪNFA״̬ͼ
		 */
		shared_ptr<MlexNfaStateDiagram> convertReToNfa(string& re) {
			stack<shared_ptr<MlexNfaReOperator>> opStack;
			vector<shared_ptr<MlexNfaWorkElement>> workStack;

			//ѭ������������ʽ��������ʽ�ĺ�׺��ʽ
			for (size_t i = 0;i < re.length();i++) {

				if (isLetter(re[i]) || isNumberic(re[i])) {

					//����ĸ�����Ԫ��
					_char_tab.emplace_back(re[i]);

					//����Ԫ�ص�״̬ת��ͼ����state1��->c->��state2��
					workStack.emplace_back(createBasicStateDiagram(re[i]));

					//ѹ�����������
					goto CONTACT;
				}

				switch (re[i])
				{
				//�ظ�0-n�εıհ�
				case '*':
				{
					pushOperator(shared_ptr<MlexNfaReOperator>(new MlexNfaReOperator(0, 0xFFFF)), opStack, workStack);
					break;
				}
				//�ظ�1-n�εıհ�
				case '+':
				{
					pushOperator(shared_ptr<MlexNfaReOperator>(new MlexNfaReOperator(1, 0xFFFF)), opStack, workStack);
					break;
				}
				//�ظ�0-1�εıհ�
				case '?':
				{
					pushOperator(shared_ptr<MlexNfaReOperator>(new MlexNfaReOperator(0, 1)), opStack, workStack);
					break;
				}
				//ѡ������
				case '|':
				{
					opStack.pop();
					pushOperator(shared_ptr<MlexNfaReOperator>(new MlexNfaReOperator(MlexReOpTypes::select)), opStack, workStack);
					continue;
				}
				//ѡ�����㣬���ַ�����ѡ�񵥸��ַ�
				case '[':
				{
					i++;
					auto c = readSelect(re, i);
					vector<shared_ptr<MlexNfaStateDiagram>> selected;
					for (auto iter : c) {

						//����ĸ�����Ԫ��
						_char_tab.emplace_back(iter);
						auto start = shared_ptr<MlexNfaState>(new MlexNfaState());
						auto end = shared_ptr<MlexNfaState>(new MlexNfaState());
						start->addMove(iter, end);
						shared_ptr<MlexNfaStateDiagram> sd = shared_ptr<MlexNfaStateDiagram>(new MlexNfaStateDiagram(start, end));
						selected.emplace_back(sd);
					}
					auto newDiagram = move(MlexNfaStateDiagram::select(selected));
					workStack.emplace_back(newDiagram);
					break;
				}
				//�����ţ�ֱ����ջ
				case '(':
				{
					pushOperator(shared_ptr<MlexNfaReOperator>(new MlexNfaReOperator(MlexReOpTypes::lbracket)), opStack, workStack);
					continue;
				}
				//�����ţ���Ҫ���ȵ������ڵ����������
				case ')':
				{
					if (opStack.top()->_opType != MlexReOpTypes::contact) {
						throw(string(re + ":)ǰ���д����������"));
					}
					opStack.pop();
					pushOperator(shared_ptr<MlexNfaReOperator>(new MlexNfaReOperator(MlexReOpTypes::rbracket)), opStack, workStack);
					break;
				}
				//ת�������
				case '\\':
				{
					if (!isValidChar(re[i + 1])) {
						throw(string(re + ":�����Ƿ��ַ���"));
					}
					i++;
					_char_tab.emplace_back(re[i]);
					workStack.emplace_back(createBasicStateDiagram(re[i]));
					break;
				}
				//�����ַ�ƥ�������
				case '.':
				{
					//������������ַ�������-1��ʶ
					//��NFA��-1��DFA��-1����ͬһ������
					//NFA��-1��ʶ����Ϸ��ַ�����(32,127)
					//DNF�б�ʾ������ĸ���е���������Ϸ��ַ�
					//����DFA����ֶ���ӳ��
					_char_tab.emplace_back(1);
					workStack.emplace_back(createBasicStateDiagram(1));
					break;
				}
				default:
					break;
				}

				//���˱հ���������ŵĺ��涼��Ҫ׷��һ�����Ե����������
			CONTACT:
				if (!isClosure(re[i + 1])) {
					pushOperator(shared_ptr<MlexNfaReOperator>(new MlexNfaReOperator(MlexReOpTypes::contact)), opStack, workStack);
				}
			}

			if (opStack.top()->_opType == MlexReOpTypes::contact)
				opStack.pop();

			//������ջ�еĶ���Ԫ�ص���
			while (!opStack.empty()) {
				workStack.emplace_back(opStack.top());
				opStack.pop();
			}

			stack<shared_ptr<MlexNfaStateDiagram>> valStack;
			vector<shared_ptr<MlexNfaStateDiagram>> effSelect;
			shared_ptr<MlexNfaWorkElement> ele;

			//������׺ʽ������NFA
			for (auto iter : workStack)
			{
				ele = iter;

				if (ele->_eleType == MlexNfaWorkEleTypes::StateDiagram) {
					valStack.emplace(static_pointer_cast<MlexNfaStateDiagram>(ele));
				}
				else if (ele->_eleType == MlexNfaWorkEleTypes::Operator) {

					shared_ptr<MlexNfaReOperator> opt;
					opt = move(static_pointer_cast<MlexNfaReOperator>(ele));

					switch (opt->_opType)
					{
					case MlexReOpTypes::closure:
						handlerClosure(valStack, opt);
						break;
					case MlexReOpTypes::contact:
						handlerContact(valStack);
						break;
					case MlexReOpTypes::select:
						effSelect.emplace_back(valStack.top());
						valStack.pop();
						effSelect.emplace_back(valStack.top());
						valStack.pop();
						handlerSelect(valStack, effSelect);
						break;
					default:
						break;
					}
				}
			}

			//�������յ�״̬ͼ
			return valStack.top();
		}

		/**
		 * ���ν�����������ʽת��ΪNFA��������ѡ�������������е�NFA
		 */
		shared_ptr<MlexNfaStateDiagram> convert() {
			
			vector<shared_ptr<MlexNfaStateDiagram>> reSds;

			for (size_t i = 0;i < _regExps.getReCount();i++) {

				MlexRegexpContext& re = _regExps.getRegExp(i);
				auto diag = convertReToNfa(re._regExp);
				diag->getEndState()->_oldre = re;
				diag->getEndState()->_final = true;
				reSds.emplace_back(diag);
			}

			//ȥ����ĸ�����ظ�Ԫ��
			sort(_char_tab.begin(), _char_tab.end());
			_char_tab.erase(unique(_char_tab.begin(), _char_tab.end()), _char_tab.end());

			//����re���ɵ�NFA����ѡ������
			_main_diagram = move(MlexNfaStateDiagram::select(reSds));
			//_main_diagram->getEndState()->_final = true;

			return _main_diagram;
		}

	};
}