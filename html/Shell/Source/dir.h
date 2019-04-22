#ifndef DIR_H
#define DIR_H

#include "file.h";

struct dir{
    string name;
    dir * up;
    string path;
    vector<dir *> dirs;
    vector<file *> files;
    
    void operator~(){ //initialize path
        if(up!=nullptr){
            path = up->path;
            path += ("/" + name);
        }else{
            path = "/root";
        }
    }
    
    string operator^(int t){ //recursive display
        string ret = "";
        string tabs = "";
        for(int i = 0; i < t; i++){
            tabs+="\t";
        }
        ret += (tabs + "@" + name + "{");
        
        for(vector<dir * >::iterator it = dirs.begin(); it!=dirs.end(); ++it){
            ret += "\n" + ((**it) ^ (t+1));
        }
        for(vector<file * >::iterator it = files.begin(); it!=files.end(); ++it){
            ret += ("\n" + tabs + "\t!" +  ((*it)->name));
        }
        
        ret += ("\n" + tabs + "}");
        return ret;
    }
    
    void operator-(file * f){
        file * ptr;
        for(vector<file * >::iterator it = files.begin(); it!=files.end(); ++it){
            if((f->name)==((*it)->name)){
                ptr = (*it);
                files.erase(it);
                delete ptr;
                return;
            }
        }
    }
    void operator-(dir * d){
        dir * ptr;
        for(vector<dir * >::iterator it = dirs.begin(); it!=dirs.end(); ++it){
            if(d==(*it)){
                ptr = (*it);
                !(**it);
                dirs.erase(it);
                delete ptr;
                return;
            }
        }
    }
    void operator!(){
        while(dirs.size()>0){
            (*this)-(*dirs.begin());
        }
        while(files.size()>0){
            (*this)-(*files.begin());
        }
    }
    
    dir * operator||(string targ){ //find directory
        for(vector<dir *>::iterator it = dirs.begin(); it!=dirs.end(); ++it){
            if((*it)->name == targ){
                return (*it);
            }
        }
        return nullptr;
    }
    file * operator|(string targ){
        for(vector<file *>::iterator it = files.begin(); it!=files.end(); ++it){
            if((*it)->name == targ){
                return (*it);
            }
        }
        return nullptr;
    }
    vector<dir *> operator||(const char* targ){ //all directories matching targ
        if(targ == "*"){
            return dirs;
        }
        vector<dir *> ret; 
        for(vector<dir *>::iterator it = dirs.begin(); it!=dirs.end(); ++it){
            if((*it)->name.find(targ) != string::npos){
                ret.push_back(*it);
            }
        }
        return ret;
    }
    vector<file *> operator|(const char* targ){
        if(targ == "*"){
            return files;
        }
        vector<file *> ret; 
        for(vector<file *>::iterator it = files.begin(); it!=files.end(); ++it){
            if((*it)->name.find(targ) != string::npos){
                ret.push_back(*it);
            }
        }
        return ret;
    }
    vector<file *> operator%=(const char* search){ //all files below dir
        search=((search=="")?"*":search);
        vector<file *> v;
        
        join<file *>(v, ((*this) | search));
        for(vector<dir *>::iterator it=dirs.begin(); it!=dirs.end(); ++it){
            join<file *>(v, (**it) %= search);
        }
        return(v);
    }
    
    dir * operator<<(dir d){
        dirs.push_back(&d);
        return &d;
    }
    file * operator<<(file f){
        files.push_back(&f);
        return &f;
    }
    dir * operator<<(string n){//add subdir
        dir * temp = new dir;
        temp->name = n;
        temp->up = this;
        ~(*temp);
        dirs.push_back(temp);
        return temp;
    }
    file * operator+=(string n){ //add file to dir
        file * temp = new file;
        temp->name = n;
        ~(*temp);
        files.push_back(temp);
        return temp;
    }
};

#endif /* DIR_H */

