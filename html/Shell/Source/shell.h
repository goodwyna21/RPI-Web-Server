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

struct ret_val{
    string data;
    bool success;
    vector<char> spcls;
    ret_val(){
        data = "";
        success = true;
    }
    ret_val(bool b){ //failure
        data = "";
        success = b;
    }
    
    ret_val operator+ (ret_val r){
        ret_val tmp = (*this);
        tmp.data += r.data;
        join<char>(tmp.spcls,r.spcls);
        tmp.success = (tmp.success && r.success);
        return tmp;
    }
    
    ret_val& operator<< (const char* s){
        data += s;
        return *this;
    }
    ret_val& operator<< (std::string s){
        (*this) << s.c_str();
        return (*this);
    }
    ret_val& operator<< (char s){
        (*this) << ("" + s);
        return *this;
    }
    ret_val& operator<< (int a){
        (*this) << to_string(a);
        return *this;
    }
    ret_val& operator<< (bool b){
        (*this) << (b?"true":"false");
        return *this;
    }
    ret_val& operator<< (byte b){
        (*this) << b.to_string();
        return *this;
    }
    ret_val& operator<< (vector<string> v){
        for(vector<string>::iterator it = v.begin(); it!=v.end(); ++it){
            (*this) << (*it) << ", ";
        }
        return *this;
    }
};

ret_val FAIL(false);

vector<string> sep_comm(string s, vector<char> &opts); //forward declaration for neatness

struct shell{
    string user;
    dir * pos;
    printer print;
    printer log;
    vector<var *> vars;
    
    shell(){
        pos = new dir;
        pos->name = "root";
        pos->up = nullptr;
        ~(*pos);
        user = "root";
    }
    void operator<=(string u){
        user = u;
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
    var * operator%=(string s){ //return var with id s
        if(s[0]=='$'){
            s=s.substr(1,s.size()-1); //string leading $
        }
        for(int i = 0; i < vars.size(); i++){
            if(vars.at(i)->id == s){
                return vars.at(i);
            }
        }
        return nullptr;
    }
    /*
     * Define execute with external functions
     */
    
    
    ret_val operator() (vector<string> comm, vector<char> opts){
        ret_val RET;
        FAIL.data="";
        if(comm.at(0)=="ls"){
            dir * d;
            if(comm.size()<2){
                d = pos;
            }else{
                d = ((*this)||comm.at(1));
            }
            if(d == nullptr){
                FAIL << "Error: Invalid path '" << comm.at(1) << "'\n";
                return FAIL;
            }
            if(has<char>(opts,'p')){
                RET << d->path << "\n";
            }
            
            
            string tmp;
            if(!has<char>(opts,'r')){
                tmp = "";
                int cnt = 0;
                for(vector<dir *>::iterator it = d->dirs.begin(); it != d->dirs.end(); ++it){
                    cnt++;
                    if((cnt > 4 || has<char>(opts,'c')) && it!=d->dirs.begin()){
                        tmp += "\n";
                        cnt = 0;
                    }
                    tmp += "@" + (*it)->name + "  ";
                }
                for(vector<file *>::iterator it = d->files.begin(); it != d->files.end(); ++it){
                    cnt++;
                    if(cnt > 4 || has<char>(opts,'c')){
                        tmp += "\n";
                        cnt = 0;
                    }
                    tmp += "!" + (*it)->name + "  ";
                }
            } else {
                tmp = ((*d)^0);
                if(has<char>(opts,'n')){
                    remove(tmp,"\n\t");
                }
            }
            if(has<char>(opts,'t')){
                remove(tmp,"@!");
            }
            RET << tmp << ((tmp!="")?"\n":"");
            return RET;
        }
        if(comm.at(0)=="pwd"){
            RET << pos->path << "\n ";
            return RET;
        }
        if(comm.at(0)=="cd"){
            if(comm.size()<2){
                FAIL << "Error: Improper arguments\nExpected like 'cd PATH'\n";
                return FAIL;
            }
            if(!((*this)-comm.at(1))){
                FAIL << "Error: Invalid path '" << comm.at(1) << "'\n";
                return FAIL;
            }
            return RET;
        }
        if(comm.at(0)=="echo"){
            if(comm.size()>=2){
                RET << comm.at(1);
            }
            RET << (has<char>(opts,'n')?"":"\n");
            return RET;
        }
        if(comm.at(0)=="svd"){ //save directory: svd [path] [fname]
            dir * d;
            if(comm.size()<2){
                d = pos;
            } else {
                d = ((*this)||comm.at(1));
            }
            string path = "@";
            if(comm.size()<3){
                path += d->name;
            } else {
                path += comm.at(2);
            }
            path += ".dir/";
            system(("mkdir -p " + path + " > /dev/null").c_str());

            ofstream wf;
            wf.open(path + d->name + ".tree");
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
            return RET;
        }
        if(comm.at(0)=="ldd"){ //ldd .dir
            if(comm.size()<2){
                FAIL << "Error: Improper arguments\nExpected like 'ldd FNAME.dir'\n";
                return FAIL;
            }
            string fname = comm.at(1).substr(0,(comm.at(1).size()-4));
            ifstream tree(fname + ".dir/" + fname + ".tree");
            if(tree.fail()){
                FAIL << "Error: Invalid file '" << comm.at(1) << "'\n";
                return FAIL;
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
            return RET;
        }
        if(comm.at(0)=="mkdir"){
            if(comm.size()<2){
                FAIL << "Error: Improper arguments\nExpected like 'mkdir DNAME'\n";
                return FAIL;
            }
            if((*this)||comm.at(1)){
                FAIL << ("Error: '" + comm.at(1) + "' already exists\n");
                return FAIL;
            }

            (*pos) << comm.at(1);

            return RET;
        }
        if(comm.at(0)=="touch"){
            if(comm.size()<2){
                FAIL << "Error: Improper arguments\nExpected like 'touch FNAME'\n";
                return FAIL;
            }
            if(((*pos)|comm.at(1)) != nullptr){
                FAIL << "Error: '" << comm.at(1) << "' already exists.";
                return FAIL;
            }
            (*pos) += comm.at(1);
            return RET;
        }
        if(comm.at(0)=="write"){ //write FNAME [DATA]
            if(comm.size() < 2){
                FAIL << "Error: Improper arguments\nExpected like 'write FNAME [DATA]'\n";
                return FAIL;
            }
            file * f = (*pos)|comm.at(1);
            if(f == nullptr){
                if(has<char>(opts,'i')){
                    (*pos) += comm.at(1);
                    f = (*pos)|comm.at(1);
                }else{
                    FAIL << "Error: Invalid filename\n";
                    return FAIL;
                }
            }
            if(comm.size() >= 3){
                (*f) << comm.at(2);
            }else{
                string line, inp = "";
                cout << "Editing " << comm.at(1) << "    Enter `exit` to finish\n";
                while(true){
                    cout << ">";
                    getline(cin, line);
                    if(line=="exit"){break;}
                    inp += line + "\n";
                }
                (*f) << inp;
            }
            
            return RET;
        }
        if(comm.at(0)=="cat"){ //cat FNAME
            if(comm.size() < 2){
                FAIL << "Error: Improper arguments\nExpected like 'cat FNAME'\n";
                return FAIL;
            }
            string text;
            file * f = (*pos)|comm.at(1);
            if(f==nullptr){
                FAIL << "Error: Invalid filename\n";
                return FAIL;
            }
            (*f) *= &text;
            RET << text << "\n";
            return RET;
        }
        if(comm.at(0)=="rmdir"){//rmdir DNAME
            if(comm.size() < 2){
                FAIL << "Error: Improper arguments\nExpected like 'rmdir DNAME'\n";
                return FAIL;
            }
            if(((*pos)||comm.at(1)) == nullptr){
                FAIL << "Error: No directory '" << comm.at(1) << "'\n";
                return FAIL;
            }
            (*pos)-((*pos)||comm.at(1));
            return RET;
        }
        if(comm.at(0)=="mvd"){
            if(comm.size() < 3){
                FAIL << "Error: Improper arguments\nExpected like 'mvd DNAME path'\n";
                return FAIL;
            }
            if(((*pos)||comm.at(1))==nullptr){
                FAIL << "Error: No directory '" << comm.at(1) << "'\n";
                return FAIL;
            }

        }
        if(comm.at(0)=="rmf"){
            if(comm.size() < 2){
                FAIL << "Error: Improper arguments\nExpected like 'rmf FNAME'\n";
                return FAIL;
            }
            if(((*pos)|comm.at(1))==nullptr){
                FAIL << "Error: Invalid file '" << comm.at(1) << "'\n";
                return FAIL;
            }
            (*pos)-((*pos)|comm.at(1));
            return RET;
        }
        if(comm.at(0)=="setusr"){ //setusr NAME
            if(comm.size() < 2){
                FAIL << "Error: Improper arguments\nExpected like 'setusr NAME'\n";
                return FAIL;
            }
            (*this)<=comm.at(1);
            return RET;
        }
        if(comm.at(0)=="exec"){ //execute script
            if(comm.size() < 2){
                FAIL << "Error: Improper arguments\nExpected like 'exec FNAME'\n";
                return FAIL;
            }
            if(((*pos)|comm.at(1))==nullptr){
                FAIL << "Error: Invalid file '" << comm.at(1) << "'\n";
                return FAIL;
            }
            file * f = ((*pos)|comm.at(1));
            string line;
            ++(*f);
            (*f)%=line;
            do{
                ret_val out = (*this)(line);
                if(!out.success){
                    FAIL << out.data << "in '" << line << "'\n";
                    return FAIL;
                }
                RET << out.data;
                (*f)%=line;
            }while(line!="");
            return RET;
        }
        
        if(comm.at(0)=="grep"){
            if(comm.size() < 3){
                FAIL << "Error: Improper arguments\nExpected like 'grep PATTERN string'\n";
                return FAIL;
            }
            string s = comm.at(2);
            if(has<char>(opts,'f')){
                file * f = ((*this)|s);
                if(f == nullptr){
                    FAIL << "Error: Invalid file '" << s << "'\n";
                    return FAIL;
                }
                (*f) >> s;
            }
            vector<string> lines = split(s,(has<char>(opts,'w')?"\n ":"\n"));
            for(vector<string>::iterator it = lines.begin(); it!=lines.end(); ++it){
                if(it->find(comm.at(1)) != string::npos){
                    RET << (*it) << "\n";
                }
            }
            return RET;
        }
        
        if(comm.at(0)=="ldf"){ //load file
            if(comm.size() < 3){
                FAIL << "Error: Improper arguments\nExpected like 'ldf FNAME dataFile'\n";
                return FAIL;
            }
            string sys_com = "ls | grep " + comm.at(2) + " > /dev/null";
            ifstream rf(comm.at(2));
            if(rf.fail()){
                FAIL << "Error: No file '" << comm.at(2) << "'\n";
                return FAIL;
            }
            if(((*pos)|comm.at(1)) == nullptr){
                (*this)("touch " + comm.at(1));
            }
            file * f = ((*pos)|comm.at(1));
            (*f) << rf;
            return RET;
        }
        if(comm.at(0)=="sf"){ //save file
            if(comm.size() < 3){
                FAIL << "Error: Improper arguments\nExpected like 'sf FNAME saveLocation'\n";
                return FAIL;
            }
            if(((*pos)|comm.at(1)) == nullptr){
                FAIL << "Error: Invalid file '" << comm.at(1) << "'\n";
                return FAIL;
            }
            ofstream wf;
            file * f = ((*pos)|comm.at(1));
            wf.open(comm.at(2));
            (*f) >> (&wf);
            return RET;
        }
        if(comm.at(0)=="set"){
            if(comm.size()<3){
                FAIL << "Error: Invalid arguments\nExpected like 'set VAR value'\n";
                return FAIL;
            }
            if(comm.at(1)[0]=='$'){
                comm.at(1)=comm.at(1).substr(1,comm.at(1).size()-1);
            }
            var * v = ((*this)%=comm.at(1));
            if(v==nullptr){
		if(has<char>(opts,'i'){
	       	    v = new var;
		    v->id = comm.at(1);
		    vars.push_back(v);
		}else{
		    FAIL << "Error: No var " << comm.at(1) << "\n";
		    return FAIL;
		}
            }

            (v->text) = comm.at(2);
            return RET;
        }
        if(comm.at(0)=="vars"){
            for(vector<var *>::iterator it=vars.begin(); it!=vars.end(); ++it){
                RET << ((*it)->id + " : " + escNewl((*it)->text) + "\n");
            }
            return RET;
        }
        if(comm.at(0)=="av"){//variable access
            if(comm.size()<2){
                FAIL << "Error: Improper arguments\nExpected like 'av VAR'\n";
                return FAIL;
            }
            var * v = (*this)%=comm.at(1);
            if(v==nullptr){
                if(has<char>(opts,'v')){
                    v = new var;
                    v->id = comm.at(1);
                    vars.push_back(v);
                }else{
                    FAIL << "Error: No variable '" << comm.at(1) << "'\n";
                    return FAIL;
                }
            }
            
            string tmp=v->text;
            if(has<char>(opts,'n')){
                tmp = escNewl(tmp);
            }
	    RET << tmp << (((tmp[tmp.size()-1])!='\n')?"\n":"");
            
            return RET;
        }
        
        FAIL << "Error: Unknown operation '" << comm.at(0) << "'\n";
        return FAIL;
    }
    
    ret_val operator()(string comm){ //parse command string
        if(comm == ""){return ret_val(true);}
        int redir = comm.find('>');
        int pipe = comm.find('|');
        int chain = comm.find(';');
        int multi = comm.find('%');
        if(redir != string::npos){
            ret_val ret = (*this)(comm.substr(0,redir));
            if(!ret.success){return ret;}
            ret = ret + (*this)(("set " + comm.substr(redir+1,comm.size()-redir) + " \"" + ret.data + "\""));
            return ret;
        }else if(pipe != string::npos){
            ret_val ret = ((*this)(comm.substr(0,pipe)));
            if(!ret.success){return ret;}
            ret = ((*this)((comm.substr(pipe+1,comm.size()-pipe) + " " + ret.data)));
            return ret;
        }else if(chain != string::npos){
            ret_val ret = (*this)(comm.substr(0,chain));
            if(!ret.success){return ret;}
            ret = ret + ((*this)(comm.substr(chain+1,comm.size()-chain)));
            return ret;
        }else if(multi != string::npos){
            ret_val ret;
            int r = stoi(comm.substr(multi+1,comm.find(' ',multi)-(multi+1)));
            for(int i = 0; i < r; i++){
                ret = ret + (*this)(comm.substr(0,multi));
                if(!ret.success){
                    return ret;
                }
            }
            return ret;
            
        }else{
            vector<char> opts;
            return (*this)(this->sep_comm(comm, opts),opts);
        }
    }
    
    vector<string> sep_comm(string inp, vector<char> &opts){
        vector<string> words;
        string temp = "";
        bool inquot = false;
        for(int i = 0; i < inp.size(); i++){
            if(inp[i] == '"' && valid(inp,i)){
                inquot = !inquot;
                continue;
            }
            if(inp[i] == ' ' && valid(inp,i) && !inquot){
                if(temp[0]=='$'){
                    words.push_back(((*this)%=temp)->text);
                    temp="";
                }
                if(temp[0]=='-'){
                    for(int i = 1; i < temp.size(); i++){
                        opts.push_back(temp[i]);
                    }
                    temp="";
                }
                if(temp!=""){
                    words.push_back(temp);
                    temp="";
                }
                continue;
            }
            temp+=inp[i];
        }
        if(temp[0]=='-'){
            for(int i = 1; i < temp.size(); i++){
                opts.push_back(temp[i]);
            }
        } else if(temp!=""){
            words.push_back(temp);
        }
        return words;
    }
    
    string operator!(){
        return (cyan + user + red + "@" + pos->name + cyan + "$ " + black);
    }
};

#endif /* SHELL_H */
