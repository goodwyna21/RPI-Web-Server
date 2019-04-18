/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   shell.h
 * Author: goodwyna21
 *
 * Created on April 18, 2019, 8:54 AM
 */

#ifndef SHELL_H
#define SHELL_H

#include "printer.h";

struct var{
    string id;
    string text;
    bool operator==(const char* s){
        return (id==s);
    }
    bool operator==(string s){
        return (id==s);
    }
    bool operator==(var v){
        return (id==v.id);
    }
};

struct shell{
    string user;
    dir * pos;
    printer p;
    printer log;
    vector<var *> vars;
    
    void operator~(){
        pos = new dir;
        pos->name = "root";
        pos->up = nullptr;
        ~(*pos);
        user = "root";
        
    }
    void operator<=(string u){
        user = u;
    }
    void operator% (printer print){
        p = print;
    }
    
    shell operator+(){
        pos = pos->up;
        return *this;
    }
    bool operator>=(string targ){ //move one dir down
        if(targ == ".."){
            +(*this);
            return true;
        }
        if(((*pos) || targ) == nullptr){
            return false;
        }
        pos = (*pos)||targ;
        return true;
    }
    bool operator-(vector<string> path){
        for(vector<string>::iterator it=path.begin(); it!=path.end(); ++it){
            if((*it)==""){
                continue;
            }
            if(!((*this) >= (*it))){
                return false;
            }
        }
        return true;
    }
    bool operator++(){
        while(pos->up != nullptr){
            +(*this);
        }
        return true;
    }
    bool operator-(string path){
        if(path[0]=='/'){
            ++(*this);
        }
        return (*this) - split(path,'/');
    }
    dir * operator||(string path){
        dir * spot = pos;
        if(!((*this)-path)){
            return nullptr;
        }
        dir * temp = pos;
        pos = spot;
        return temp;
    }
    file * operator|(string p){
        dir * spot = pos;
        //remove file name from path
        if(p[0] == '/'){++(*this);}
        vector<string> vec = split(p,'/');
        string fname = vec.back();
        vec.pop_back();
        (*this) - vec;
        file * temp = ((*pos)|fname);
        pos = spot;
        return temp;
    }
    var * operator%=(string s){
        for(int i = 0; i < vars.size(); i++){
            if(vars.at(i)->id == s){
                return vars.at(i);
            }
        }
        return nullptr;
    }
    
    //EXECUTE
    bool operator<<(vector<string> comm){
        string op = comm.at(0);
        if(op=="ls"){
            if(comm.size()<2){
                p << (*pos) << "\n";
                return true;
            }
            p << (*((*this)||comm.at(1))) << "\n";
            return true;
        }
        if(op=="lsr"){
            if(comm.size()<2){
                p << ((*pos)^0) << "\n";
                return true;
            }
            p << ((*((*this)||comm.at(2)))^0) << "\n";
            return true;
        }
        if(op=="pwd"){
            p << b_on << pos->path << b_off << "\n";
            return true;
        }
        if(op=="cd"){
            if(comm.size()<2){
                log << "Error: Improper arguments\nExpected like 'cd PATH'\n";
                return false;
            }
            if(!((*this)-comm.at(1))){
                log << "Error: Invalid path '" << comm.at(1) << "'\n";
                return false;
            }
            return true;
        }
        if(op=="echo"){
            p << combine(sub<string>(comm,1)," ") << "\n";
            return true;
        }
        if(op=="svd"){ //save directory: svd [PATH]
            dir * d;
            if(comm.size()<2){
                d = pos;
            }else{
                d = ((*this)||comm.at(2));
            }
            string dname = "@" + d->name;
            string path = dname + ".dir/";
            system(("mkdir -p " + path + " > /dev/null").c_str());
            
            ofstream wf;
            wf.open((path + dname + ".tree"));
            string text = ((*d)^0);
            for(int i = 0; i < text.size(); i++){
                if(text[i]=='\t'){
                    text.erase(i,1);
                    i--;
                }
            }
            wf << text;
            wf.close();
            
            vector<file *> files;
            files = ((*d) %= "*");
            for(vector<file *>::iterator it = files.begin(); it!=files.end(); ++it){
                wf.open(path + ((*it)->name) + ".data");
                (**it) *= &text;
                wf << text;
                wf.close();
            }
            return true;
        }
        if(op=="ldd"){ //ldd .dir
            if(comm.size()<2){
                log << "Error: Improper arguments\nExpected like 'ldd FNAME.dir'\n";
                return false;
            }
            string fname = comm.at(1).substr(0,(comm.at(1).size()-4));
            ifstream tree(fname + ".dir/" + fname + ".tree");
            if(tree.fail()){
                log << "Error: Invalid file '" << comm.at(1) << "'\n";
                return false;
            }
            
            //clear root
            ++(*this);
            vector<dir * > dirs = ((*pos)||"*");
            for(vector<dir * >::iterator it = dirs.begin(); it!=dirs.end(); ++it){
                (*pos)-(*it);
            }
            vector<file * > files = ((*pos)|"*");
            for(vector<file * >::iterator it = files.begin(); it!=files.end(); ++it){
                (*pos)-(*it);
            }
            
            string line;
            file * f_tmp;
            ifstream rf;
            getline(tree,line);//ignore root
            while(getline(tree,line)){
                if(line[0]=='@'){
                    pos = ((*pos)<<(line.substr(1,line.size()-2)));
                }
                if(line[0]=='!'){
                    f_tmp = ((*pos)+=(line.substr(1,line.size()-1)));
                    rf.open(fname+".dir/"+(f_tmp->name)+".data");
                    (*f_tmp)<<rf;
                    rf.close();
                }
                if(line[0]=='}'){
                    if(pos->up != nullptr){
                        pos = pos->up;
                    }
                }
            }
            tree.close();
            return true;
        }
        if(op=="mkdir"){
            if(comm.size()<2){
                log << "Error: Improper arguments\nExpected like 'mkdir DNAME'\n";
                return false;
            }
            if((*this)||comm.at(1)){
                log << ("Error: '" + comm.at(1) + "' already exists\n");
                return false;
            }
            
            (*pos) << comm.at(1);
            
            return true;
        }
        if(op=="touch"){
            if(comm.size()<2){
                log << "Error: Improper arguments\nExpected like 'touch FNAME'\n";
                return false;
            }
            if(((*pos)|comm.at(1)) != nullptr){
                log << "Error: '" << comm.at(1) << "' already exists.";
                return false;
            }
            (*pos) += comm.at(1);
            return true;
        }
        if(op=="write"){ //write FNAME DATA
            if(comm.size() < 3){
                log << "Error: Improper arguments\nExpected like 'write FNAME TEXT'\n";
                return false;
            }
            file * f = (*pos)|comm.at(1);
            if(f == nullptr){
                log << "Error: Invalid filename\n";
                return false;
            }
            vector<string>::iterator d_start = comm.begin();
            ++d_start;
            ++d_start;
            string data = "";
            for(vector<string>::iterator it = d_start; it!=comm.end(); ){
                data += (*it);
                ++it;
                if(it!=comm.end()){
                    data += " ";
                }
            }
            (*f) << data;
            return true;
        }
        if(op=="cat"){ //cat FNAME
            if(comm.size() < 2){
                log << "Error: Improper arguments\nExpected like 'cat FNAME'\n";
                return false;
            }
            string text;
            file * f = (*pos)|comm.at(1);
            if(f==nullptr){
                log << "Error: Invalid filename\n";
                return false;
            }
            (*f) *= &text;
            p << text << "\n";
            return true;
        }
        if(op=="rmdir"){//rmdir DNAME
            if(comm.size() < 2){
                log << "Error: Improper arguments\nExpected like 'rmdir DNAME'\n";
                return false;
            }
            if(((*pos)||comm.at(1)) == nullptr){
                log << "Error: No directory '" << comm.at(1) << "'\n";
                return false;
            }
            (*pos)-((*pos)||comm.at(1));
            return true;
        }
        if(op=="mvd"){
            if(comm.size() < 3){
                log << "Error: Improper arguments\nExpected like 'mvd DNAME path'\n";
                return false;
            }
            if(((*pos)||comm.at(1))==nullptr){
                log << "Error: No directory '" << comm.at(1) << "'\n";
                return false;
            }
            
        }
        if(op=="rmf"){
            if(comm.size() < 2){
                log << "Error: Improper arguments\nExpected like 'rmf FNAME'\n";
                return false;
            }
            if(((*pos)|comm.at(1))==nullptr){
                log << "Error: Invalid file '" << comm.at(1) << "'\n";
                return false;
            }
            (*pos)-((*pos)|comm.at(1));
            return true;
        }
        if(op=="setusr"){ //setusr NAME
            if(comm.size() < 2){
                log << "Error: Improper arguments\nExpected like 'setusr NAME'\n";
                return false;
            }
            (*this)<=comm.at(1);
            return true;
        }
        if(op=="edit"){ //edit FNAME
            if(comm.size() < 2){
                log << "Error: Improper arguments\nExpected like 'edit FNAME'\n";
                return false;
            }
            if(((*pos)|comm.at(1))==nullptr){
                log << "Error: Invalid file '" << comm.at(1) << "'\n";
                return false;
            }
            file * f = ((*pos)|comm.at(1));
            ofstream wf;
            ifstream rf;
            wf.open("editor.txt");
            (*f) >> &wf;
            
            system("open -a TextEdit editor.txt");
            p << "Continue (Note: save file) >";
            cin.get();
            
            rf.open("editor.txt");
            (*f) << rf;
            return true;
        }
        if(op=="exec"){ //execute script
            if(comm.size() < 2){
                log << "Error: Improper arguments\nExpected like 'exec FNAME'\n";
                return false;
            }
            if(((*pos)|comm.at(1))==nullptr){
                log << "Error: Invalid file '" << comm.at(1) << "'\n";
                return false;
            }
            file * f = ((*pos)|comm.at(1));
            string line;
            ++(*f);
            (*f)%=line;
            do{
                if(!((*this)<<line)){
                    log << "in '" << line << "'\n";
                    return false;
                }
                (*f)%=line;
            }while(line!="");
            return true;
        }
        if(op=="ldf"){ //load file
            if(comm.size() < 3){
                log << "Error: Improper arguments\nExpected like 'ldf FNAME dataFile'\n";
                return false;
            }
            string sys_com = "ls | grep " + comm.at(2) + " > /dev/null";
            ifstream rf(comm.at(2));
            if(rf.fail()){
                log << "Error: No file '" << comm.at(2) << "'\n";
                return false;
            }
            if(((*pos)|comm.at(1)) == nullptr){
                (*this) << ("touch " + comm.at(1));
            }
            file * f = ((*pos)|comm.at(1));
            (*f) << rf;
            return true;
        }
        if(op=="sf"){ //save file
            if(comm.size() < 3){
                log << "Error: Improper arguments\nExpected like 'sf FNAME saveLocation'\n";
                return false;
            }
            if(((*pos)|comm.at(1)) == nullptr){
                log << "Error: Invalid file '" << comm.at(1) << "'\n";
                return false;
            }
            ofstream wf;
            file * f = ((*pos)|comm.at(1));
            wf.open(comm.at(2));
            (*f) >> (&wf);
            return false;
        }
        if(op=="set"){
            if(comm.size()<3){
                log << "Error: Invalid arguments\nExpected like 'set VAR value'\n";
                return false;
            }
            if(comm.at(1)[0]=='$'){
                comm.at(1)=comm.at(1).substr(1,comm.at(1).size()-1);
            }
            var * v = ((*this)%=comm.at(1));
            if(v==nullptr){
                v = new var;
                v->id = comm.at(1);
                vars.push_back(v);
            }
            
            (v->text) = comm.at(2);
            return true;
        }
        if(op=="vars"){
            for(vector<var *>::iterator it=vars.begin(); it!=vars.end(); ++it){
                p << ((*it)->id + " : " + escape((*it)->text) + "\n");
            }
            return true;
        }
        if(op[0]=='$'){
            var * v = (*this)%=(op.substr(1,op.size()-1));
            if(v==nullptr){
                log << "Error: No variable '" << op << "'\n";
                return false;
            }
            p << ((*this)%=(op.substr(1,op.size()-1)))->text << "\n";
            return true;
        }
        p || false;
        p << combine(comm," ") << "\n";
        p || true;
        log << "Error: Unknown operation '" << op << "'\n";
        return false;
    }
    
    bool operator<<(string comm){ //parse command string
        if(comm == ""){return false;}
        //parse redirect (>)
        int redir = comm.find('>');
        int pipe = comm.find('|');
        if(redir != string::npos){
            string c = comm.substr(0,redir);
            p || false;
            (*this) << c;
            string retval;
            p >> retval;
            c = comm.substr(redir+1,comm.size()-redir);
            (*this) << ("set " + c + " \"" + retval + "\"");
            p || true;
        }else if(pipe != string::npos){
            string c = comm.substr(0,pipe);
            p || false;
            (*this) << c;
            string retval;
            p >> retval;
            p || true;
            c = comm.substr(pipe+1,comm.size()-pipe);
            c += " " + retval;
            (*this) << c;
        }else{
            return (*this)<<sep_comm(comm);
        }
    }
    
    string operator!(){
        return (cyan + user + red + "@" + pos->name + cyan + "$ " + black);
    }
};

#endif /* SHELL_H */

