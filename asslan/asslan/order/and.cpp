#include"head.h"
//by ����ͮ (���) 
int order_and(int arg,char*args[])
{
	if(arg != 4)
	{
		cout<<"�����ȷ"<<endl;
		return 0;
	}
	//c 1 2 3 
	code = 0;
	st(op,numofb("000000"));
	check();
	st(rs,numofstr(args[2]));
	check();
	st(rt,numofstr(args[3]));
	check();
	st(rd,numofstr(args[1]));
	check();
	st(func,numofb("100100")); //func 
	check();
	st(shamt,numofb("000000"));
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

	printf("and r%s=r%s & r%s",args[1],args[2],args[3]);
	return 1;
}
