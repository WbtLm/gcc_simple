#include"head.h"
//by 王宝彤 (初翊) 
int order_divu(int arg,char*args[])
{
	if(arg != 3)
	{
		cout<<"命令不正确"<<endl;
		return 0;
	}
	//c 1 2 3 
	code = 0;
	st(op,numofb("000000"));
	check();
	st(rs,numofstr(args[1]));
	check();
	st(rt,numofstr(args[2]));
	check();
	st(func,numofb("011011")); //func 
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

	printf("divu {hi,lo} = r%s / r%s, unsigned",args[1],args[2]);
//    cout<<endl;
	return 1;
}
