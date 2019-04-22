#ifndef STRUCTS_H
#define STRUCTS_H
#include <bitset>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "breakpoints.h"
using namespace std;

typedef bitset<8> byte;  
const string b_on = "\e[1m";
const string b_off= "\e[0m";
const string cyan = "\e[36m";
const string lime = "\e[32m";
const string black= "\e[30m";
const string red  = "\e[31m";

char byteToChar(bitset<8> b){
    return static_cast<unsigned char>(b.to_ulong());
}

string remove(string &s, string c){
    string ret = "";
    bool clear;
    for(int i = 0; i < s.size(); i++){
        clear = true;
        for(int k = 0; k < c.size(); k++){
            if(s[i]==c[k]){
                clear = false;
            }
        }
        if(clear){
            ret += s[i];
        }
    }
    s=ret;
    return ret;
}

string escNewl(string s){
    while(s.find('\n',0) != string::npos){
        s.replace(s.find('\n'),2,"\\n");
    }
    return s;
}

bool valid(string s, int pos){
    if(pos==0){return true;}
    return !(s[pos-1]=='\\' && valid(s,pos-1));
}

template<typename T>
void join(vector<T> &a, vector<T> b){
    for(int i = 0; i < b.size(); i++){
        a.push_back(b.at(i));
    }
}
template<typename T>
vector<T> sub(vector<T> v, unsigned int start){
    vector<T> ret;
    for(int i = start; i < v.size(); i++){
        ret.push_back(v.at(i));
    }
    return ret;
}

template<typename T>
bool has(vector<T> v, T foo){
    for(int i = 0; i < v.size(); i++){
        if(v.at(i)==foo){
            return true;
        }
    }
    return false;
}

vector<string> split(string inp, char sep){
    vector<string> words;
    string temp = "";
    for(int i = 0; i < inp.size(); i++){
        if(inp[i] == sep){
            if(temp!=""){
                words.push_back(temp);
                temp="";
            }
            continue;
        }
        temp+=inp[i];
    }
    if(temp!=""){
        words.push_back(temp);
    }
    return words;
}

vector<string> split(string inp, string sep){
    vector<string> words;
    string temp = "";
    for(int i = 0; i < inp.size(); i++){
        for(int k = 0; k < sep.size(); k++){
            if(inp[i]==sep[i]){
                if(temp!=""){
                    words.push_back(temp);
                    temp="";
                }
                continue;
            }
        }
        temp+=inp[i];
    }
    if(temp!=""){
        words.push_back(temp);
    }
    return words;
}

string combine(vector<string> v, const char* join){
    if(v.size()==0){
        return "";
    }
    string ret = "";
    for(vector<string>::iterator it = v.begin(); true; ){
        ret += (*it);
        ++it;
        if(it!=v.end()){
            ret += join;
        }else{
            break;
        }
    }
    return ret;
}

#endif /* STRUCTS_H */

