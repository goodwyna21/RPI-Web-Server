#include "shell.h";

#include <unistd.h>
printer p;

int main(int argc, char** argv) {
    shell s;
    ~s;
    s % p;

    chdir("Data");
    string comm;
    while(true){
        p << !s;
        getline(cin,comm);
        s << comm;
    }
    return 0;
}