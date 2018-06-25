#pragma once

#include <list>
#include "nfa.h"

namespace mlex {

	static uint32_t _dfaStateCounter = 0;
	static uint32_t _dfaSimpleStateCounter = 0;

	//�򻯵�DFA״̬
	class MlexDfaSimpleState {
	private:
		uint32_t _stateId;
		map<char, shared_ptr<MlexDfaSimpleState>> _moveMap;

	public:
		bool _final;
		string _oldre;

		MlexDfaSimpleState() {
			_final = false;
			_stateId = _dfaSimpleStateCounter++;
		}

		uint32_t getStateId() {
			return _stateId;
		}

		void addMove(char Input, shared_ptr<MlexDfaSimpleState> State) {
			pair<char, shared_ptr<MlexDfaSimpleState>> newPair(Input, State);
			_moveMap.emplace(newPair);
		}

		shared_ptr<MlexDfaSimpleState> getMove(char Input) {
			return _moveMap[Input];
		}
	};

	//��ͨ��DFA״̬
	class MlexDfaState {
	private:
		map<char, shared_ptr<MlexDfaState>> _moveMap;
		vector<shared_ptr<MlexNfaState>> _nfaStates;
		uint32_t _nfaStatesSum;
		uint32_t _stateId;
		
	public:
		bool _final;
		string _oldre;
		shared_ptr<list<shared_ptr<MlexDfaState>>> _equalityGroup;

		/**
		 * ����һ��DFA״̬
		 */
		MlexDfaState() {
			_final = false;
			_stateId = _dfaStateCounter++;
		}

		/**
		 * ����һ��DFA״̬
		 * @param	nfaStates		ԭʼNFA��״̬��
		 */
		MlexDfaState(vector<shared_ptr<MlexNfaState>>nfaStates) :_nfaStates(move(nfaStates)) {
			_final = false;
			_stateId = _dfaStateCounter++;
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
		map<uint32_t,vector<shared_ptr<MlexDfaState>>> _dfaStatesOnIdSummary;
		map<uint32_t, shared_ptr<MlexDfaState>> _dfaStatesMap;
		map<uint32_t, shared_ptr<MlexDfaSimpleState>> _dfaSimpleStateMap;

	public:
		shared_ptr<MlexDfaState> _startState;
		shared_ptr<MlexDfaSimpleState> _sStartState;

		MlexDfa(MlexNfa Nfa) :_nfa(Nfa) {
		}

		vector<char>& getInputTable() {
			return _nfa._char_tab;
		}

		map<uint32_t, shared_ptr<MlexDfaState>>& getDfaStatesMap() {
			return _dfaStatesMap;
		}

		map<uint32_t, shared_ptr<MlexDfaSimpleState>>& getDfaSimpleStateMap() {
			return _dfaSimpleStateMap;
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
					if (_dfaStatesOnIdSummary.count(newDfaState->getNfaStatesSum()) != 0) {
						vector<shared_ptr<MlexDfaState>>& v = _dfaStatesOnIdSummary[newDfaState->getNfaStatesSum()];
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
							_dfaStateCounter--;
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
				if(_dfaStatesOnIdSummary.count(instack_state->getNfaStatesSum())){
					vector<shared_ptr<MlexDfaState>>& v = _dfaStatesOnIdSummary[instack_state->getNfaStatesSum()];
					v.emplace_back(instack_state);
				}
				//������������½�һ��
				else {
					vector<shared_ptr<MlexDfaState>> v;
					v.emplace_back(instack_state);
					_dfaStatesOnIdSummary.emplace(pair<uint32_t, vector<shared_ptr<MlexDfaState>>>(instack_state->getNfaStatesSum(), v));
				}

				//����DFA��Ӧ��������ʽ
				for (auto iter : instack_state->getNfaStates()) {
					if (iter->_final) {
						instack_state->_oldre = iter->_oldre;
						break;
					}
				}
				
				_dfaStatesMap.emplace(pair<uint32_t, shared_ptr<MlexDfaState>>(instack_state->getStateId(), instack_state));
			}

			//����NFA�����нڵ㶼�ᱻ����
			_dfaStatesOnIdSummary.clear();
			for (auto iter : _dfaStatesMap) {
				iter.second->getNfaStates().clear();
			}
		}

		/**
		 * DFAת��DFA
		 * @param	deep		��ȼ�
		 * 
		 * ������Ҫ����ԭʼ������ʽ����Ϣ�������ڼ���̬��ʱ��������Ҫ��������������
		 * ����Ҫ��״̬��Ӧ��������ʽ��ͬ��״̬���зָ�
		 * ���deepλtrue����״̬���٣���ֻ�ܽ����ַ���ƥ��
		 * ���deepΪfalse����״̬���һЩ�����������ڴ�������
		 */
		void simplify(bool deep) {

			stack<shared_ptr<list<shared_ptr<MlexDfaState>>>> _workGroup;
			vector<shared_ptr<list<shared_ptr<MlexDfaState>>>> _finalGroup;

			//���ȷָ�������飬һ������̬��һ���Ƿ���̬
			auto _group1 = shared_ptr<list<shared_ptr<MlexDfaState>>>(new list<shared_ptr<MlexDfaState>>);
			auto _group2 = shared_ptr<list<shared_ptr<MlexDfaState>>>(new list<shared_ptr<MlexDfaState>>);
			for (auto iter : _dfaStatesMap) {
				if (!iter.second->_final) {
					iter.second->_equalityGroup = shared_ptr<list<shared_ptr<MlexDfaState>>>(_group1);
					_group1->emplace_back(iter.second);
				}
				else {
					iter.second->_equalityGroup = shared_ptr<list<shared_ptr<MlexDfaState>>>(_group2);
					_group2->emplace_back(iter.second);
				}
			}
			_workGroup.emplace(_group1);
			_workGroup.emplace(_group2);

			while (!_workGroup.empty()) {
				bool not_final = false;
				auto top = _workGroup.top();
				_workGroup.pop();

				for (auto iter : _nfa._char_tab) {
					auto _tag_group = (top->front()->getMove(iter) ? top->front()->getMove(iter)->_equalityGroup.get() : nullptr);
					auto _tag_re = top->front()->_oldre;
					auto _group1 = shared_ptr<list<shared_ptr<MlexDfaState>>>(new list<shared_ptr<MlexDfaState>>);

					//�����ȼ۵�״̬�ŵ���һ���µ�����
					top->remove_if([_tag_group,_tag_re,_group1,iter,deep](shared_ptr<MlexDfaState>& state) {
						if (!(
							(_tag_group == nullptr && state->getMove(iter) == nullptr) ||
								(state->getMove(iter) && (state->getMove(iter)->_equalityGroup.get() == _tag_group))
								)
							|| (!deep && state->_final && (state->_oldre.compare(_tag_re) != 0))
							) {
							state->_equalityGroup = shared_ptr<list<shared_ptr<MlexDfaState>>>(_group1);
							_group1->emplace_back(state);
							return true;
						}
						else {
							return false;
						}
					});

					if (_group1->size() != 0) {
						_workGroup.emplace(_group1);
					}
				}

				_finalGroup.emplace_back(top);
			}

			map<list<shared_ptr<MlexDfaState>>*, shared_ptr<MlexDfaSimpleState>> stateMap;

			for (auto iter : _finalGroup) {
				shared_ptr<MlexDfaSimpleState> sstate = shared_ptr<MlexDfaSimpleState>(new MlexDfaSimpleState());
				stateMap.emplace(pair<list<shared_ptr<MlexDfaState>>*, shared_ptr<MlexDfaSimpleState>>(iter.get(), move(sstate)));
			}

			for (auto iter : _finalGroup) {
				auto sstate = stateMap[iter.get()];
				//����ÿ��������ţ������޳����ȼ۵�״̬
				for (auto iter2 : *iter) {
					for (auto iter3 : _nfa._char_tab) {
						if (iter2->getMove(iter3)) {
							auto moveTag = stateMap[iter2->getMove(iter3)->_equalityGroup.get()];
							sstate->addMove(iter3, moveTag);
						}
					}
					if (iter2->getStateId() == 0) {
						_sStartState = sstate;
					}
					if (iter2->_final) {
						sstate->_final = true;
						sstate->_oldre = iter2->_oldre;
					}
				}
			}
		}

		/**
		 * ����һ���ַ����Ƿ�ƥ�䲢����ԭʼ������ʽ
		 */
		bool validateString(string& s,string& re) {
			size_t i = 0;
			shared_ptr<MlexDfaSimpleState> dfa_state = _sStartState->getMove(s[i]);
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

			re = dfa_state->_oldre;
			return true;
		}
	};


}