#include"head.h" 
int order_sra(int arg,char*args[])
{//by 王宝彤 (初翊)
	if(arg != 4)
	{
		cout<<"命令不正确"<<endl;
		return 0;
	}
	//c 1 2 3 
	code = 0;
	st(op,numofb("000000"));
	check();
	st(rd,numofstr(args[1]));
	check();
	st(rt,numofstr(args[2]));
	check();
	st(shamt,numofstr(args[3]));
	check();
	st(func,numofb("000011")); //func 
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

	printf("sra r%s=r%s >> %s",args[1],args[2],args[3]);
//    cout<<endl;
	return 1;
}
