/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   file.h
 * Author: goodwyna21
 *
 * Created on April 18, 2019, 8:58 AM
 */

#ifndef FILE_H
#define FILE_H

#include "imports.h";

struct file{
    string name;
    vector<byte> data;
    vector<byte>::iterator pos;

    void operator~ (){
        pos = data.begin();
    }
    
    //traversal ops
    void operator++(){
        pos=data.begin();
    }
    void operator--(){
        pos=data.end();
    }
    file operator-(unsigned int n){
        pos = prev(pos,n);
        return *this;
    }
    file operator+(unsigned int n){
        for(int i = 0; i < n; i++){
            pos++;
        }
        return *this;
    }
    file operator*= (int n){
        if(n < 0){
            (*this) - abs(n);
        }else{
            (*this) + n;
        }
        return *this;
    }
    
    //write ops
    file operator<< (byte b){ //append byte
        data.push_back(b);
        return *this;
    }
    file operator<< (char c){
        *this << byte(c);
        return *this;
    }
    file operator<< (string s){ 
        for(int i = 0; i < s.size(); i++){
            (*this) << s[i];
        }
        return *this;
    }
    file operator!= (string s){ //clear data then write
        data.clear();
        (*this) << s;
        pos = data.begin();
        return *this;
    }
    file operator<< (ifstream &f){ //copy file
        string temp;
        getline(f,temp);
        data.clear();
        while(true){
            (*this) << temp;
            if(getline(f, temp)){
                (*this) << '\n';
            }else{
                break;
            }
        }
        f.close();
        pos = data.begin();
        return *this;
    }
    
    //read ops
    file operator>> (byte * b){
        (*b) = (*pos);
        ++pos;
        return *this;
    }
    file operator>> (char * c){
        byte b;
        (*this) >> (&b);
        *c = static_cast<unsigned char>( b.to_ulong());
        return *this;
    }
    file operator>> (int * n){
        byte b;
        (*this) >> (&b);
        *n = (int)(b.to_ulong());
        return *this;
    }
    file operator>> (string &s){
        s = "";
        while(byteToChar(*pos) != ' ' && byteToChar(*pos)!='\n' && pos!=data.end()){
            s += byteToChar(*pos);
            (*this) + 1;
        }
        return *this;
    }
    bool operator%= (string &s){ //readline
        s="";
        if(pos==data.end()){
            return false;
        }
        
        char c;
        (*this) >> (&c);
        while(c != '\n' && (pos!=data.end())){
            s += c;
            (*this) >> (&c);
        }
        if(pos==data.end()){
            s += byteToChar(data.at(data.size()-1));
        }
        return (pos!=data.end());
    }
    file operator*= (string * s){ //read all
        (*s) = "";
        for(vector<byte>::iterator it = data.begin(); it!=data.end(); ++it){
            (*s)+=byteToChar(*it);
        }
        return *this;
    }
    file operator>> (ofstream * f){
        string temp;
        (*this) *= &temp;
        (*f) << temp;
        f->close();
        return *this;
    }
};

#endif /* FILE_H */

