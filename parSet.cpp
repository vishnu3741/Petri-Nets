#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stack>
#include <queue>
#include <map>
using namespace std;

class derRegEx{
public:
  string Ex;
  string St;
  bool contE;

  vector<bool> eSet;
  vector<string> staSet;
  vector<string> derSet;

  void addDer(string der){
    this->derSet.push_back(der);
  }
  void setE(bool s){
    this->contE=s;
  }
  void setEx(string s){
    this->Ex=s;
  }
  void setSt(string s){
    this->St=s;
  }
  void printDer(){
    cout<<this->Ex<<" : "<<this->St<<" : "<<this->contE<<endl;
    if(this->derSet.size()>0){
      cout<<"{";
      for(int i=0;i<this->derSet.size();i++) cout<<"| "<<this->derSet[i]<<" : "<<this->staSet[i]<<" : "<<this->eSet[i]<<" |";
      cout<<"}"<<endl;
    }
  }   
};

class node{
public:
  derRegEx value;
  map<char,vector<node*> > children;
  
  derRegEx retValue(){
    return this->value;
  }
  void printTree(){
    queue<node*> list;
    list.push(this);
    cout<<"------------Printing Tree------------"<<endl;
    while(!list.empty()){
      node* n=list.front();
      list.pop();
      cout<<"-------------"<<endl;
      cout<<"Node : "<<endl;
      n->value.printDer();
      if(n->children.size()>0){
	cout<<'\n';
	cout<<"children :"<<endl;
	map<char,vector<node*> >::iterator it;
	for(it=n->children.begin();it!=n->children.end();it++){
	  cout<<"Transitions with "<<it->first<<" : "<<endl;
	  for(int i=0;i<n->children[it->first].size();i++){
	    list.push(n->children[it->first][i]);
	    cout<<n->children[it->first][i]->value.Ex<<" : "<<n->children[it->first][i]->value.contE<<endl;
	  }
	}
	cout<<'\n';
      }
      else cout<<"\nLeaf\n"<<endl;
    }
    cout<<"-------------Printing Done-------------"<<endl;
  }
};
 
vector<string> closureSet(int n,string s){
  vector<string> closure;
  closure.push_back("1");
  for(int i=0;i<s.size();i++){
    string q(1,s[i]);
    closure.push_back(q);
  }
  closure.push_back(s);
  for(int i=2;i<n;i++){
    for(int k=0;k<s.size();k++){
      string q;
      for(int l=0;l<k;l++) q=q+s[l];
      for(int l=0;l<i;l++) q=q+s[k];
      for(int l=k+1;l<s.size();l++) q=q+s[l];
      closure.push_back(q);
    }
  }
  return closure;
}

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

derRegEx fromSymbol(char s,char w,char wrt){
  derRegEx retObj;
  string l(1,s);
  string q(1,w);
  retObj.setEx(l);
  retObj.setSt(q);
  retObj.setE(false);
  if(l=="1") retObj.setE(true);
  if(s==wrt){
    retObj.derSet.push_back("1");
    retObj.eSet.push_back(true);
    retObj.staSet.push_back(q);
    return retObj;
  }
  return retObj;
}

derRegEx unionOP(derRegEx first,derRegEx second){
  derRegEx retObj;
  for(int i=0;i<first.derSet.size();i++){
    retObj.derSet.push_back(first.derSet[i]);
    retObj.eSet.push_back(first.eSet[i]);
    retObj.staSet.push_back(first.staSet[i]);
  }
  for(int i=0;i<second.derSet.size();i++){
    retObj.derSet.push_back(second.derSet[i]);
    retObj.eSet.push_back(second.eSet[i]);
    retObj.staSet.push_back(second.staSet[i]);
  }
  retObj.setE(first.contE || second.contE);
  retObj.setEx("("+first.Ex+"+"+second.Ex+")");
  retObj.setSt("("+first.St+"+"+second.St+")");
  return retObj;
}

derRegEx closureOP(derRegEx regEx){
  derRegEx retObj;
  for(int i=0;i<regEx.derSet.size();i++){
    if(regEx.derSet[i]!="1"){
      retObj.derSet.push_back(regEx.derSet[i]+".("+regEx.Ex+")*");
      retObj.eSet.push_back(regEx.eSet[i]);
      retObj.staSet.push_back(regEx.staSet[i]+".("+regEx.St+")*");
    }
    else{
      retObj.derSet.push_back("("+regEx.Ex+")*");
      retObj.eSet.push_back(true);
      retObj.staSet.push_back("("+regEx.St+")*");
    }
  }
  retObj.setE(true);
  retObj.setEx("("+regEx.Ex+")*");
  retObj.setSt("("+regEx.St+")*");
  return retObj;
}

derRegEx concatOP(derRegEx first,derRegEx second){
  derRegEx retObj;
  for(int i=0;i<first.derSet.size();i++){
    if(first.derSet[i]!="1"){
      retObj.derSet.push_back(first.derSet[i]+"."+second.Ex);
      retObj.eSet.push_back(first.eSet[i] && second.contE);
      retObj.staSet.push_back(first.staSet[i]+"."+second.St);
    }
    else{
      retObj.derSet.push_back(second.Ex);
      retObj.eSet.push_back(second.contE);
      retObj.staSet.push_back(second.St);
    }
  }
  if(first.contE){
    for(int i=0;i<second.derSet.size();i++){
      retObj.derSet.push_back(second.derSet[i]);
      retObj.eSet.push_back(second.eSet[i]);
      retObj.staSet.push_back(second.staSet[i]);
    }
  }
  retObj.setE(first.contE && second.contE);
  retObj.setEx(first.Ex+"."+second.Ex);
  retObj.setSt(first.St+"."+second.St);
  return retObj;
}

derRegEx derEx(string postfixExp,string postfixSta,char wrt){
  stack<derRegEx> derStack;
  for(int i=0;i<postfixExp.size();i++){
    cout<<"OP : "<<postfixExp[i]<<endl;
    cout<<"wrt : "<<wrt<<endl;
    if(postfixExp[i]=='*'){
      derRegEx curDer=derStack.top();
      derStack.pop();
      derStack.push(closureOP(curDer));
    }
    else if(postfixExp[i]=='+'){
      derRegEx first=derStack.top();
      derStack.pop();
      derRegEx second=derStack.top();
      derStack.pop();
      derStack.push(unionOP(first,second));
    }
    else if(postfixExp[i]=='.'){
      derRegEx second=derStack.top();
      derStack.pop();
      derRegEx first=derStack.top();
      derStack.pop();
      derStack.push(concatOP(first,second));
    }
    else{
      derStack.push(fromSymbol(postfixExp[i],postfixSta[i],wrt));
    }
    cout<<"------------"<<endl;
    cout<<derStack.top().Ex<<endl;
    derStack.top().printDer();
    cout<<"------------"<<endl;
  }
  return derStack.top();
}
		    
derRegEx derivative(string regEx,string states,string wrt){
  vector<string> setEx;
  setEx.push_back(regEx);
  vector<string> setState;
  setState.push_back(states);
  vector<bool> setE;
  
  for(int i=wrt.size()-1;i>=0;i--){
    vector<string> retList;
    vector<string> retState;
    vector<bool> retE;
    for(int j=0;j<setEx.size();j++){
      cout<<toPostfix(setEx[j])<<endl;
      cout<<toPostfix(setState[j])<<endl;
      derRegEx ret=derEx(toPostfix(setEx[j]),toPostfix(setState[j]),wrt[i]);
      for(int k=0;k<ret.derSet.size();k++) {
	retList.push_back(ret.derSet[k]);
	retState.push_back(ret.staSet[k]);
	retE.push_back(ret.eSet[k]);
      }
    }
    setE=retE;
    setEx=retList;
    setState=retState;
  }
  
  derRegEx retObj;
  retObj.setEx(regEx);
  retObj.setSt(states);
  retObj.derSet=setEx;
  retObj.staSet=setState;
  retObj.eSet=setE;

  return retObj;
}

node* parDerTree(string s,string state,string wrt){
  node *root=new node;
  root->value.Ex=s;
  root->value.St=state;
  vector<string> created;
  created.push_back(root->value.Ex);
  queue<node*> list;
  list.push(root);
  
  while(!list.empty()){
    node* n=list.front();
    list.pop();
    for(int i=0;i<wrt.size();i++){
      string wrtS(1,wrt[i]);
      derRegEx output=derivative(n->value.Ex,n->value.St,wrtS);
      for(int i=0;i<output.derSet.size();i++){
	bool visited=false;
	for(int j=0;j<created.size();j++){
	  if(created[j]==output.derSet[i]) visited=true;
	}
	if(visited) continue;
	created.push_back(output.derSet[i]);
	derRegEx *childEx=new derRegEx;
	childEx->setEx(output.derSet[i]);
	childEx->setSt(output.staSet[i]);
	childEx->setE(output.eSet[i]);
	node* child=new node;
	child->value=*childEx;
	n->children[wrtS[0]].push_back(child);
	list.push(child);
      }
    }
  }
  return root;
}

int main(){
  string regEx;
  cin>>regEx;
  string states;
  cin>>states;
  string wrt;
  cin>>wrt;

  node* root=parDerTree(regEx,states,wrt);
  cout<<"\n\n\n\n\n\n\n";
  root->printTree();

}
