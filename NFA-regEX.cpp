#include <iostream>
#include <string>
using namespace std;

void priEq(string eq[][3],int n){
  cout<<'\n';
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      cout<<eq[i][j]<<"     ";
    }
    cout<<'\n';
  }
  cout<<'\n';
  return;
}

void priTer(string term[],int n){
  cout<<'\n';
  for(int j=0;j<n;j++){
    cout<<term[j]<<endl;
  }
  cout<<'\n';
}

string starEx(string s){
  if(s=="e") return "e";
  else if(s=="#") return "#";
  else if(s.size()==1) return s+'*';
  return '('+s+')'+'*';
}

string proEx(string s1,string s2){
  if(s1=="#" || s2=="#") return "#";
  else if(s1.size()==1 || s2.size()==1) return s1+s2;
  else if(s1[0]=='(' && s2[0]=='(') return s1+s2;
  else if(s1[0]=='(' && s2[0]!='(') return s1+'('+s2+')';
  else if(s1[0]!='(' && s2[0]=='(') return '('+s1+')'+s2;
  return '('+s1+')'+'('+s2+')';
}

string sumEx(string s1,string s2){
  if(s1=="#") return s2;
  else if(s2=="#") return s1;
  return s1+"+"+s2;
}

int main(){
  const int n=3;
  string adj[n][n];
  string eq[n][n];
  string in[n][n];
  string term[n];
  int fStates[n];

  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      cin>>adj[i][j];
    }
  }

  for(int i=0;i<n;i++){
    term[i]="#";
    cin>>fStates[i];
  }
  
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      eq[j][i]=adj[i][j];
      in[j][i]=eq[j][i];
    }
  }

  //  priEq(eq,n);

  for(int i=n-1;i>=1;i--){
    if(eq[i][i]!="#"){
      eq[i][i]=starEx(eq[i][i]);
      for(int j=0;j<i;j++){
	if(i!=j){
	  eq[i][j]=proEx(eq[i][j],eq[i][i]);
	}
      }
    }

    priEq(eq,n);
    
    for(int j=i-1;j>=0;j--){
      for(int k=0;k<i;k++){
	//	cout<<" j = "<<j<<" i = "<<i<<" k = "<<k<<endl;
	//cout<<eq[j][i]<<"  :  "<<eq[j+1][i]<<"  :  "<<eq[j][k]+'+'+proEx(eq[i][k],eq[j][i])<<endl;
	eq[j][k]=sumEx(eq[j][k],proEx(eq[i][k],eq[j][i]));
	//cout<<" eq[j][k] = "<<eq[j][k]<<endl;
      }
    }
    priEq(eq,n);
  }   

  // cout<<eq[0][0]<<endl;
  term[0]=starEx(eq[0][0]);
  
  for(int i=1;i<n;i++){
    for(int j=0;j<i;j++){
      term[i]=sumEx(term[i],proEx(term[j],eq[i][j]));
    }
  }

  // priTer(term,3);

  string regEx="#";

  for(int i=0;i<n;i++){
    if(fStates[i]==1){
      regEx=sumEx(regEx,term[i]);
    }
  }

  cout<<"Regular Expression is : "<<regEx<<endl;
  
}
