#include"head.h"
//by ����ͮ (���) 
int order_mflo(int arg,char*args[])
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
	st(rd,numofstr(args[1]));
	check();
	st(func,numofb("010010")); //func 
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

	printf("mflo r%s = lo",args[1]);
//    cout<<endl;
	return 1;
}
