#include "Shell/Source/shell.h"
#include <unistd.h>

int main(int argc, char *argv[]){
	shell shl;
	chdir("../saves");
	ret_val ret = shl("ldd @root.dir");
	if(!ret.success){
		cout << ret.data;
		return 1;
	}
	
	cout << !shl << argv[1] << "\n";
	ret = shl(argv[1]);
	cout << ret.data;
	shl("svd");

	return 0;
}
