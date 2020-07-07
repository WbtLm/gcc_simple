#include"head.h" 
#include"orders.h"
map<string,int>mp,ofmp;
vector<string>cd;
vector<int>hint;
order_guidence &order_guid = *order_guidence::get_shared_impl();
int assemblylan(string filepath)
{//by Íõ±¦Í® (³õñ´)
	FILE *fp = fopen(filepath.c_str(),"rt");
    if(fp==NULL){
        cout<<"fp==NULL"<<endl;
        cout<<filepath<<endl;
        return 0;
    }
    order_regist();
	char tmp[1024],fst[64],scd[64],thrd[64];
	int i = 0 ;
	int ret; 
	int idx=0;
    int line_i=0;
	while(fgets(tmp,1024,fp))
	{
        line_i++;
		rmsp(tmp);
		for(int i=1;tmp[i]!=0;i++)
		{
			if(tmp[i]=='/' && tmp[i-1]=='/')
			{
				tmp[i-1]=0;
				break;
			}
		}
		if(tmp[0]==0)
			continue;
		ret=sscanf(tmp,"%s%s%s",fst,scd,thrd);
		if(ret == 0)
			continue;
		if(strcmp(fst,"sign:")==0)
		{
			if(ret != 2)
			{
                cout<<"sign is not right with arg num="<<ret<<endl;
				exit(0);
			}
			mp["sign_"+string(scd)] = idx;
			cout<<"//sign_"<<scd<<"=:"<<idx<<endl;
		}
		else if(strcmp(fst,"offset:")==0)
		{
			if(ret != 2)
			{
				cout<<"wrong §offset"<<ret<<endl;
				exit(0);
			}
			ofmp["offset_"+string(scd)] = idx;
			cout<<"//offset_"<<scd<<"=:"<<idx<<endl;
		} 
		else
		{
			idx++;
			cd.push_back(string(tmp));
            hint.push_back(line_i);
		}	
	}
	char ag[5][128];
	for(int i=0;i<cd.size();i++)
	{
		string s = cd[i];
		ret = sscanf(s.c_str(),"%s%s%s%s",ag[0],ag[1],ag[2],ag[3]);
		for(int j=0;j<ret;j++)
		{
			if(mp.count(string(ag[j])))
			{
				char idx[128];
				sprintf(idx,"%d",mp[string(ag[j])]); 
				s.replace(
						s.find(string(ag[j])),
						strlen(ag[j]),
						idx
					);
			}
			if(ofmp.count(string(ag[j])))
			{
				char idx[128];
				sprintf(idx,"%d",ofmp[string(ag[j])] - (i+1));
				s.replace(
						s.find(string(ag[j])),
						strlen(ag[j]),
						idx
					);
			}
		}
	//	cout<<s;
	//	printf("instmem[%d]=",i);
        stringstream in(s);
        string fst,tmp;
        char* args[128];
        in>>fst;
        int cnt=0;
        args[cnt]=new char[fst.length()+1];
        strcpy(args[cnt], fst.c_str());
        while(in>>tmp){
            cnt++;
            if(cnt>=128){
                cout<<"error:args too much"<<endl;
                return 0;
            }
            args[cnt]=new char[tmp.length()+1];
            strcpy(args[cnt], tmp.c_str());
        }
        order_func_t func_ptr = order_guid.get_order_func(fst);
        if(order_guid.is_error()){
            cout<<order_guid.get_error_info()<<endl;
            cout<<"Line:"<<endl;
            cout<<s<<endl;
            return 0;
        }
        int ret=func_ptr(cnt+1,args);
        cout<<"\t\t["+to_string(hint[i])+"]";
        cout<<endl;
        for (int i=0; i<=cnt; i++) {
            delete args[i];
        }
        if(ret==0){
            return 0;
        }
	}
	
	
	cout<<"//program end"<<endl;
    return 1;
}
