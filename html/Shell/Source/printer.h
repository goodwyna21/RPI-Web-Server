/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   printer.h
 * Author: goodwyna21
 *
 * Created on April 18, 2019, 8:57 AM
 */

#ifndef PRINTER_H
#define PRINTER_H

#include "dir.h";

enum p_data {QUIET,LOUD};

class printer{
public:    
    string data;
    bool disp;
    bool shh;
    printer(){
        data = "";
        disp = true;
        shh = false;
    }
    void operator|| (bool b){
        disp = b;
    }
    void operator>> (string &s){
        s = data;
        data = "";
    }
    bool operator! (){//buffer not empty
        return data!="";
    }
    
    printer& operator<< (p_data d){
        switch (d){
            case QUIET:
                shh = true;
                break;
            case LOUD:
                shh = false;
                break;
        }
        return *this;
    }
    
    printer& operator<< (const char* s){
        if(disp && !shh){
            cout << s;
        }else{
            data += s;
        }
        return *this;
    }
    printer& operator<< (std::string s){
        (*this) << s.c_str();
        return (*this);
    }
    printer& operator<< (char s){
        (*this) << ("" + s);
        return *this;
    }
    printer& operator<< (int a){
        (*this) << to_string(a);
        return *this;
    }
    printer& operator<< (bool b){
        (*this) << (b?"true":"false");
        return *this;
    }
    printer& operator<< (byte b){
        (*this) << b.to_string();
        return *this;
    }
    printer& operator<< (vector<string> v){
        for(vector<string>::iterator it = v.begin(); it!=v.end(); ++it){
            (*this) << (*it) << ", ";
        }
        return *this;
    }
    
/*    printer operator<< (dir d){
        *this << b_on << d.path << b_off << "\n";
        int cnt = 0;
        for(vector<dir *>::iterator it = d.dirs.begin(); it != d.dirs.end(); ++it){
            cnt++;
            if(cnt > 4){
                (*this) << "\n";
                cnt = 0;
            }
            (*this) << "@" << (*it)->name << "  ";
        }
        for(vector<file *>::iterator it = d.files.begin(); it != d.files.end(); ++it){
            cnt++;
            if(cnt > 4){
                (*this) << "\n";
                cnt = 0;
            }
            (*this) << "!" << (*it)->name << "  ";
        }
        return (*this);
    }*/
private:
};

#endif /* PRINTER_H */

