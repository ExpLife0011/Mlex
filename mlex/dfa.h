#pragma once

#include "nfa.h"

namespace mlex {

	class MlexDfaState {
	private:
		map<char, shared_ptr<MlexDfaState>> _moveMap;
		vector<shared_ptr<MlexNfaState>> _nfaStates;
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
		MlexDfaState(vector<shared_ptr<MlexNfaState>>nfaStates) :_nfaStates(nfaStates) {
			_final = false;
			_stateId = _stateIdCounter++;
		}

		/**
		 * ��ȡ״̬ID
		 */
		uint32_t getStateId() {
			return _stateId;
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
			return _moveMap[Input];
		}

		//�ж�����dfa״̬�Ƿ�ȼ�
		bool operator==(const MlexDfaState& Another) {

			if (_nfaStates.size() != Another._nfaStates.size())
				return false;

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
		vector<shared_ptr<MlexDfaState>> _dfaStates;

	public:
		MlexDfa(MlexNfa Nfa) :_nfa(Nfa) {

		}

		/**
		 * ���һ��dfa״̬������nfa״̬����ĳ������Input�Ħ�-closure(move(Input))
		 * @param	Input		�����ַ�
		 * @param	OldVector	ԭʼNFA״̬��
		 * @param	Final		��״̬�����Ƿ������̬
		 */
		vector<shared_ptr<MlexNfaState>> getInputDfaState(char Input, vector<shared_ptr<MlexNfaState>>& OldVector,bool& Final) {

			vector<shared_ptr<MlexNfaState>> newVector;

			for (auto iter :OldVector) {
				//��ÿ��Ԫ�أ����move(Input)�ļ���
				auto moveState = move(iter->getMove(Input));
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
					//newVector.insert(end(newVector), make_move_iterator(begin(emptyState)), make_move_iterator(end(emptyState)));
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
			auto newDfaState = move(shared_ptr<MlexDfaState>(new MlexDfaState(st_state)));
			new_states.push(move(newDfaState));

			while (!new_states.empty()) {

				//����ջ��dfa״̬�ڵ�
				auto instack_state = move(new_states.top());
				new_states.pop();

				//���ÿ�������ַ���������-closure(move(Input))����
				for (auto iter : _nfa._char_tab) {

					//ȡ�æ�-closure(move(Input))����
					st_state = getInputDfaState(iter, instack_state->getNfaStates(), final);
					if (st_state.size() == 0) {
						continue;
					}

					//���ü��Ϲ����µ�dfa�ڵ�
					newDfaState = shared_ptr<MlexDfaState>(new MlexDfaState(st_state));
					//�����nfa״̬���а�����̬�����dfaΪ��̬
					if (final) {
						newDfaState->_final = true;
					}

					auto find = find_if(begin(_dfaStates), end(_dfaStates), [newDfaState](const shared_ptr<MlexDfaState> One) {
						return (*One) == (*newDfaState);
					});
					//�����dfaջ�в����ڣ���ѹ��dfaջ
					if (find == end(_dfaStates)) {
						new_states.push(newDfaState);
						instack_state->addMove(iter, newDfaState);
					}
					else {
						instack_state->addMove(iter, *find);
					}
					//�����������dfa�ڵ�

					_dfaStates.emplace_back(instack_state);
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
			shared_ptr<MlexDfaState> dfa_state = _dfaStates[0]->getMove(s[i]);
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