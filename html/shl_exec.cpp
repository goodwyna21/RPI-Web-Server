#include "Shell/Source/shell.h"

int main(int argc, char *argv[]){
	shell shl;
	ret_val ret = shl("ldd @root.dir");
	if(!ret.success){
		cout << ret.data;
		return 1;
	}
	
	cout << !shl;
	ret = shl(argv[1]);
	cout << ret.data << "\n";
	shl("svd / root");

	return 0;
}
