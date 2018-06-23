#pragma once

#include <list>
#include "nfa.h"

namespace mlex {

	class MlexDfaState {
	private:
		map<char, shared_ptr<MlexDfaState>> _moveMap;
		vector<shared_ptr<MlexNfaState>> _nfaStates;
		uint32_t _nfaStatesSum;
		uint32_t _stateId;
		

	public:
		bool _final;
		
		/**
		 * ����һ��DFA״̬
		 */
		MlexDfaState() {
			_final = false;
			_stateId = _stateIdCounter++;
		}

		/**
		 * ����һ��DFA״̬
		 * @param	nfaStates		ԭʼNFA��״̬��
		 */
		MlexDfaState(vector<shared_ptr<MlexNfaState>>nfaStates) :_nfaStates(move(nfaStates)) {
			_final = false;
			_stateId = _stateIdCounter++;
			_nfaStatesSum = 0;
			for (auto iter1 : _nfaStates) {
				_nfaStatesSum += iter1->getStateId();
			}
		}

		/**
		 * ��ȡ״̬ID
		 */
		uint32_t getStateId() {
			return _stateId;
		}

		uint32_t getNfaStatesSum() {
			return _nfaStatesSum;
		}

		/**
		 * ��ȡDFA��NFA״̬��
		 */
		vector<shared_ptr<MlexNfaState>>& getNfaStates() {
			return _nfaStates;
		}

		/**
		 * ���һ�������ַ�λInput�Ļ�
		 * @param	Input		Input�����ַ�
		 * @param	State		ת�����DFA״̬
		 */
		void addMove(char Input, shared_ptr<MlexDfaState> State) {
			pair<char, shared_ptr<MlexDfaState>> newMove(Input, shared_ptr<MlexDfaState>(State));
			_moveMap.emplace(newMove);
		}

		/**
		 * ��ȡ�����ַ���Ӧ��DFA״̬
		 * @param	Input		Input�����ַ�
		 */
		shared_ptr<MlexDfaState> getMove(char Input) {
			//�ַ����е�����
			if (_moveMap[Input]) {
				return _moveMap[Input];
			}
			//������Խ��������ַ��Ļ�
			else {
				return _moveMap[-1];
			}
		}

		//�ж�����dfa״̬�Ƿ�ȼ�
		bool operator==(const MlexDfaState& Another) {

			if (_nfaStates.size() != Another._nfaStates.size()) {
				return false;
			}

			auto iter2 = begin(Another._nfaStates);
			for (auto iter1:_nfaStates) {
				if (iter1->getStateId() != (*iter2)->getStateId())
					return false;
				iter2++;
			}

			return true;
		}
	};

	class MlexDfa {

	private:
		MlexNfa _nfa;
		map<uint32_t,vector<shared_ptr<MlexDfaState>>> _dfaStates;
		shared_ptr<MlexDfaState> _startState;

	public:
		MlexDfa(MlexNfa Nfa) :_nfa(Nfa) {

		}

		vector<shared_ptr<MlexNfaState>>::iterator findEmplacePosition(vector<shared_ptr<MlexNfaState>>& v,shared_ptr<MlexNfaState>& dfa) {

			if (v.size() == 0) {
				return end(v);
			}

			if (v[0]->getStateId() > dfa->getStateId()) {
				return begin(v);
			}

			auto startState = begin(v);
			auto endState = end(v) - 1;
			decltype(startState) midState;// = startState + (endState - startState) / 2;

			while (startState <= endState) {

				midState = startState + (endState - startState) / 2;
				
				if (dfa->getStateId() > (*midState)->getStateId()) {
					startState = midState + 1;
				}
				else if (dfa->getStateId() < (*midState)->getStateId()) {
					endState = midState - 1;
				}
				else {
					return midState;
				}
			}

			return startState;
		}

		/**
		 * ���һ��dfa״̬������nfa״̬����ĳ������Input�Ħ�-closure(move(Input))
		 * @param	Input		�����ַ�
		 * @param	OldVector	ԭʼNFA״̬��
		 * @param	Final		��״̬�����Ƿ������̬
		 */
		vector<shared_ptr<MlexNfaState>> getInputDfaState(char Input, vector<shared_ptr<MlexNfaState>>& OldVector,bool& Final) {

			vector<shared_ptr<MlexNfaState>> newVector;
			newVector.reserve(OldVector.size() * 4);

			for (auto iter :OldVector) {
				//��ÿ��Ԫ�أ����move(Input)�ļ���
				auto moveState = iter->getMove(Input);
				for (auto iter2 :moveState) {
					//�ٶ�ÿ��move(Input)���-closure(move(Input))
					//���ﷵ�ص������ã����Բ�Ҫʹ��move
					auto emptyState = iter2->emptyClosure();
					for (auto iter3 :emptyState) {
						//�����nfa����̬�Ļ�����ǰdfaҲ����̬
						if (iter3->_final)
							Final = true;
						newVector.emplace_back(iter3);
					}
					//move(Input)����Ҳ��Ҫ��ջ
					if (iter2->_final)
						Final = true;
					newVector.emplace_back(iter2);
				}
				
			}

			//����
			sort(begin(newVector), end(newVector),
				[](const shared_ptr<MlexNfaState>& a, const shared_ptr<MlexNfaState>& b) { 
				return a->getStateId() < b->getStateId();
			});
			//ȥ��
			newVector.erase(unique(begin(newVector), end(newVector), 
				[](const shared_ptr<MlexNfaState>& a, const shared_ptr<MlexNfaState>& b) {
				return a->getStateId() == b->getStateId();
			}), end(newVector));

			return newVector;
		}

		/**
		 * NFAװDFA
		 */
		void convert() {

			bool final = false;
			stack<shared_ptr<MlexDfaState>> new_states;

			//�����ʼ�ڵ�Ħ�-closure
			auto st_state = _nfa._main_diagram->_startState->emptyClosure();
			//����
			sort(begin(st_state), end(st_state),
				[](const shared_ptr<MlexNfaState>& a, const shared_ptr<MlexNfaState>& b) {
				return a->getStateId() < b->getStateId();
			});
			//ȥ��
			st_state.erase(unique(begin(st_state), end(st_state),
				[](const shared_ptr<MlexNfaState>& a, const shared_ptr<MlexNfaState>& b) {
				return a->getStateId() == b->getStateId();
			}), end(st_state));

			//����ʼ�ڵ�Ħ�-closure���ɵ�dfa״̬�ڵ�ѹ��ջ��
			auto newDfaState = shared_ptr<MlexDfaState>(new MlexDfaState(st_state));

			//������ʼDF״̬������ƥ������￪ʼ
			_startState = shared_ptr<MlexDfaState>(newDfaState);

			new_states.push(move(newDfaState));

			while (!new_states.empty()) {

				//����ջ��dfa״̬�ڵ�
				auto instack_state = new_states.top();
				new_states.pop();

				//���ÿ�������ַ���������-closure(move(Input))����
				for (auto iter : _nfa._char_tab) {

					//ȡ�æ�-closure(move(Input))����
					st_state = move(getInputDfaState(iter, instack_state->getNfaStates(), final));
					if (st_state.size() == 0) {
						continue;
					}

					//���ü��Ϲ����µ�dfa�ڵ�
					newDfaState = shared_ptr<MlexDfaState>(new MlexDfaState(st_state));
					//�����nfa״̬���а�����̬�����dfaΪ��̬
					if (final) {
						newDfaState->_final = true;
					}
					final = false;

					//����ȷ���µ�DFA״̬�Ƿ��Ѿ�����
					if (_dfaStates.count(newDfaState->getNfaStatesSum()) != 0) {
						vector<shared_ptr<MlexDfaState>>& v = _dfaStates[newDfaState->getNfaStatesSum()];
						auto find = find_if(begin(v), end(v), [newDfaState](const shared_ptr<MlexDfaState> One) {
							return (*One) == (*newDfaState);
						});
						//�����dfaջ�в����ڣ���ѹ��dfaջ
						if (find == end(v)) {
							instack_state->addMove(iter, newDfaState);
							new_states.emplace(newDfaState);
						}
						else {
							instack_state->addMove(iter, *find);
						}
					}
					else {
						instack_state->addMove(iter, newDfaState);
						new_states.emplace(newDfaState);
					}

					//�����������dfa�ڵ�
					
				}

				//���DFA״̬���а�������DFA��NFA״̬����֮�ͺ͵�ǰDFA״̬��NFA״̬����֮����ͬ��Ԫ��
				//��ô����ǽ���map�ڲ���Ч��ӳ���㷨��������DFA״̬�Ƚϵ�ʱ��
				if( _dfaStates.count(instack_state->getNfaStatesSum())){
					vector<shared_ptr<MlexDfaState>>& v = _dfaStates[instack_state->getNfaStatesSum()];
					v.emplace_back(instack_state);
				}
				//������������½�һ��
				else {
					vector<shared_ptr<MlexDfaState>> v;
					v.emplace_back(instack_state);
					_dfaStates.emplace(pair<uint32_t, vector<shared_ptr<MlexDfaState>>>(instack_state->getNfaStatesSum(), v));
				}
			}
		}

		/**
		 * DFAת��DFA
		 */
		void simplify() {

		}

		/**
		 * ����һ���ַ����Ƿ�ƥ�䲢����ԭʼ������ʽ
		 */
		bool validateString(string& s,string& re) {
			size_t i = 0;
			shared_ptr<MlexDfaState> dfa_state = _startState->getMove(s[i]);
			if (!dfa_state)
				return false;
			while (i != s.length() - 1) {
				i++;
				dfa_state = dfa_state->getMove(s[i]);
				if (!dfa_state)
					return false;
			}
			if (!dfa_state->_final)
				return false;

			//����֮ǰ���Ź���ģ�����϶���������ƥ��ԭ��
			for (auto iter : dfa_state->getNfaStates()) {
				if (iter->_final) {
					re = iter->_oldre;
					break;
				}
			}
			return true;
		}
	};


}