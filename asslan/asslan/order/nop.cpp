#include"head.h" 
int order_nop(int arg,char*args[])
{//by 王宝彤 (初翊)
	if(arg != 1)
	{
		cout<<"命令不正确"<<endl;
		return 0;
	}
	//c 1 2 3 
	code = -1;
	
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

	printf("nop");
//    cout<<endl;
	return 1;
}
