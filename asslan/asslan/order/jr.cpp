#include"head.h" 
//by ����ͮ (���)
int order_jr(int arg,char*args[])
{
	if(arg != 2)
	{
		cout<<"�����ȷ"<<endl;
		return 0;
	}
	//c 1 2 3 
	code = 0;
	st(op,numofb("000000"));
	check();
	st(rs,numofstr(args[1]));
	check();

	st(func,8); //func 
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

	printf("jr r%s",args[1]);
//    cout<<endl;
	return 1;
}
