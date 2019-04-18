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
vector<string> sep_comm(string inp){
    vector<string> words;
    string temp = "";
    bool inquot = false;
    for(int i = 0; i < inp.size(); i++){
        if(inp[i] == '"'){
            inquot = !inquot;
            continue;
        }
        if(inp[i] == ' ' && !inquot){
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

string escape(string s){
    for(int i = 0; i < s.size(); i++){
        if(s[i]=='/'){
            s.insert(i,"//");
            i+=2;
        }
    }
    return s;
}

#endif /* STRUCTS_H */

