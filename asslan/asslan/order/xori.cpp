#include"head.h" 
int order_xori(int arg,char*args[])
{//by 王宝彤 (初翊)
	if(arg != 4)
	{
		cout<<"命令不正确"<<endl;
		return 0;
	}
	//c 1 2 3 
	code = 0;
	st(op,numofb("001110"));
	check();
	st(rs,numofstr(args[2]));
	check();
	st(rt,numofstr(args[1]));
	check();
	st(imm,numofstr(args[3]));
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

	printf("xori r%s=r%s xor %s",args[1],args[2],args[3]);
//    cout<<endl;
	return 1;
}
