#include"head.h"
//by ����ͮ (���) 
int order_mult(int arg,char*args[])
{
	if(arg != 3)
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
	st(rt,numofstr(args[2]));
	check();
	st(func,numofb("011000")); //func 
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

	printf("mult {hi,lo} = r%s * r%s, signed",args[1],args[2]);
//    cout<<endl;
	return 1;
}