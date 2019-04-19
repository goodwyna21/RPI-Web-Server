#include "shell.h";

#include <unistd.h>

int main(int argc, char** argv) {
    shell shl;

    chdir("Data");
    string comm;
    while(true){
        cout << !shl;
        getline(cin,comm);
        cout << shl(comm).data;
    }
    return 0;
}