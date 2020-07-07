#include"head.h" 
//by 王宝彤 (初翊)
int order_j(int arg,char*args[])
{
	if(arg != 2)
	{
		cout<<"命令不正确"<<endl;
		return 0;
	}
	code = 0;
	st(op,numofb("000010"));
	check();
	st(address,numofstr(args[1]));
	check();
	
	
	
	char tmp[128];
	sprintf(tmp,"%8X",code);
	for(int i=0;i<8;i++)
	{
		if(tmp[i]==' ')
		{
			tmp[i]='0';
		}
	}

	cout<<"32'h"<<tmp<<";//\t";
	printf("j %s",args[1]);
//    cout<<endl;
	return 1;
}
