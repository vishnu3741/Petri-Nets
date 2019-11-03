#include <iostream>
#include <stack>
#include <queue>
#include <list>
#include <map>
#include <vector>
using namespace std;

int operatorPrecedence(char c){
  if(c=='+') return 0;
  else if(c=='.') return 1;
  else if(c=='*') return 2;
}

string toPostfix(string regEx){
  string postFix;
  stack<char> operatorStack;

  for(int i=0;i<regEx.size();i++){
    if(regEx[i]=='.' || regEx[i]=='+' || regEx[i]=='*'){
      while(!operatorStack.empty() && operatorStack.top()!='(' && (operatorPrecedence(operatorStack.top()) >= operatorPrecedence(regEx[i]))){
	postFix=postFix+operatorStack.top();
	operatorStack.pop();
      }
      operatorStack.push(regEx[i]);
    }
    else if(regEx[i]=='(' || regEx[i]==')'){
      if(regEx[i]=='(') operatorStack.push(regEx[i]);
      else{
	while(operatorStack.top() != '('){
	  postFix=postFix+operatorStack.top();
	  operatorStack.pop();
	}
	operatorStack.pop();
      }
    }
    else postFix=postFix+regEx[i];
  }

  while(!operatorStack.empty()){
    postFix=postFix+operatorStack.top();
    operatorStack.pop();
  }

  return postFix;
  
}

class state{
public:
  int ID;
  bool ifFinal;
  map<char,vector<state*> > transitions;

  void setValues(int ID,bool ifFinal){
    this->ID=ID;
    this->ifFinal=ifFinal;
    return;
  }
  int getID(){
    return this->ID;
  }
  void setID(int id){
    this->ID=id;
    return;
  }
  void setIfFinal(bool ifFinal){
    this->ifFinal=ifFinal;
    return;
  }
  void addTransition(char letter,state* s){
    this->transitions[letter].push_back(s);
    return;
  }
  void addEpsilonTransition(state* s){
    this->transitions['#'].push_back(s);
    return;
  }
};

bool checkElement(vector<state*> visited,state* s){
  for(int i=0;i<visited.size();i++) if(visited[i]==s) return true;
  return false;
}

class NFA{
public:
  state* startState;
  state* endState;

  void setStartState(state* s){
    this->startState=s;
    return;
  }
  void setEndState(state* s){
    this->endState=s;
    return;
  }
  void printNFA(){
    cout<<"Printing NFA : "<<endl;
    cout<<"Start State : "<<startState->ID<<endl;
    cout<<"End State : "<<endState->ID<<endl;
    cout<<'\n';
    queue<state*> list;
    vector<state*> visited;
    list.push(this->startState);
    
    while(!list.empty()){
      state* n=list.front();
      visited.push_back(n);
      list.pop();
      if(n->transitions.size()>0)cout<<"State : "<<n->ID<<endl;
      map<char,vector<state*> >::iterator itr;
      for(itr=n->transitions.begin(); itr!=n->transitions.end();itr++){
	for(int i=0;i<itr->second.size();i++){
	  cout<<n->ID<<"-"<<itr->first<<"->"<<itr->second[i]->ID<<endl;
	  if(!checkElement(visited,itr->second[i])) list.push(itr->second[i]);
	}
      }
    }
    cout<<"print done"<<endl;
  }
};

NFA fromEpsilon(int &ID){
  state *start=new state;
  state *final=new state;
  start->setValues(ID,false);
  final->setValues(ID+1,true);
  ID+=2;
  NFA *retNFA=new NFA;
  start->addEpsilonTransition(final);
  retNFA->setStartState(start);
  retNFA->setEndState(final);
  return *retNFA;
}

NFA fromSymbol(char letter,int &ID){
  state *start=new state;
  state *final=new state;
  start->setValues(ID,false);
  final->setValues(ID+1,true);
  ID+=2;
  NFA *retNFA=new NFA;
  start->addTransition(letter,final);
  retNFA->setStartState(start);
  retNFA->setEndState(final);
  return *retNFA;
}

NFA concatOP(NFA first,NFA second){
  first.endState->addEpsilonTransition(second.startState);
  first.endState->setIfFinal(false);
  NFA *retNFA=new NFA;
  retNFA->setStartState(first.startState);
  retNFA->setEndState(second.endState);
  return *(retNFA);
}

NFA unionOP(NFA first,NFA second,int &ID){
  state *start=new state;
  start->setValues(ID,false);
  start->addEpsilonTransition(first.startState);
  start->addEpsilonTransition(second.startState);

  state* end=new state;
  end->setValues(ID+1,true);
  ID+=2;
  first.endState->addEpsilonTransition(end);
  first.endState->ifFinal=false;
  second.endState->addEpsilonTransition(end);
  second.endState->ifFinal=false;

  NFA* retNFA=new NFA;
  retNFA->setStartState(start);
  retNFA->setEndState(end);
  return *(retNFA);
}

NFA closureOP(NFA &curNFA,int &ID){
  state* start=new state;
  start->setValues(ID,false);
  state* end=new state;
  end->setValues(ID+1,true);
  ID+=2;

  curNFA.endState->addEpsilonTransition(curNFA.startState);
  curNFA.endState->ifFinal=false;
  curNFA.endState->addEpsilonTransition(end);
  start->addEpsilonTransition(end);
  start->addEpsilonTransition(curNFA.startState);
  
  NFA* retNFA=new NFA;
  retNFA->setStartState(start);
  retNFA->setEndState(end);
  return *(retNFA);
}

NFA toNFA(string postfixExp){
  int ID=0;
  if(postfixExp=="") return fromEpsilon(ID);
  stack<NFA> nfaStack;
  for(int i=0;i<postfixExp.size();i++){
    if(postfixExp[i]=='*'){
      NFA curNFA=nfaStack.top();
      nfaStack.pop();
      nfaStack.push(closureOP(curNFA,ID));
    }
    else if(postfixExp[i]=='+'){
      NFA right=nfaStack.top();
      nfaStack.pop();
      NFA left=nfaStack.top();
      nfaStack.pop();
      nfaStack.push(unionOP(left,right,ID));
    }
    else if(postfixExp[i]=='.'){
      NFA right=nfaStack.top();
      nfaStack.pop();
      NFA left=nfaStack.top();
      nfaStack.pop();
      nfaStack.push(concatOP(left,right));
    }
    else{
      nfaStack.push(fromSymbol(postfixExp[i],ID));
    }
  }

  return nfaStack.top();
}

int main(){
  string regEX;
  cin>>regEX;
  string postfixExp=toPostfix(regEX);
  cout<<'\n';
  cout<<"The Postfix Expression : "<<postfixExp<<endl;
  cout<<'\n';
  NFA retNFA=toNFA(postfixExp);
  retNFA.printNFA();
}

