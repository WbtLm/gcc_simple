//int ret;
//int func(){
//    int i;
//    int ans;
//    i=1;
//    ans=0;
//    while (i<101) {
//        ans=ans+i;
//        i=i+1;
//    }
//    return ans;
//}
//int main(){
//    ret = func();
//}

//int ret;
//int *p;
//int func(){
//    p=&ret;
//    int i;
//    int *iptr;
//    iptr=&i;
//    *iptr=0;
//    ret=0;
//    while (*iptr<101) {
//        *p=*p+i;
//        i=i+1;
//    }
//}
//int main(){
//
//    func();
//}
//
//int mid;
//int mid1;
//int ret;
//int *p;
//
//int func(){
//    int i;
//    int *iptr;
//    iptr=&i;
//    *iptr=0;
//    ret=0;
//    while (*iptr<102) {
//        *p=*p+i;
//        i=i+1;
//    }
//}
//int f1(){
//    p=&mid;
//    p=p+1;
//    func();
//}
//int main(){
//    f1();
//}
//int tt;
//int ttt;
//int main(){
//    int ret[10];
//    int i;
//    ret[0]=1;
//    ret[1]=1;
//    i=0;
//    while (i<4) {
//        ret[i+2]=ret[i]+ret[i+1];
//        i=i+1;
//    }
//    tt=ret[2];
//    ttt=ret[3];
//}
//int ret[10];
//int main(){
//    int i;
//    ret[0]=1;
//    ret[1]=1;
//    i=0;
//    while (i<4) {
//        ret[i+2]=ret[i]+ret[i+1];
//        i=i+1;
//    }
//}
//struct node{
//    int a;
//    int b;
//};
//int ret;
//
//int main(){
//    struct node snode;
//    snode.a=1;
//    snode.b=2;
//    ret=snode.a+snode.b;
//}
//struct nodec{
//    int node;
//};
//struct node{
//    int a;
//    int b;
//    struct nodec npt;
//    int c;
//};
//int ret[3];
//int main(){
//    struct node f;
//    f.npt.node=111;
//    f.a=222;
//    f.b=999;
//    f.c=211;
//    ret[0]=f.npt.node;
//}


//int ret;
//int main(){
//    int a;
//    int b;
//    a=1;
//    b=2;
//    a=(b=100);
//    ret=a+b; //ret==200
//}
//int ret;
//int main(){
//    int a;
//    int b;
//    a=1;
//    b=2;
//    a=b=100;
//    ret=a+b; //first:a=b, ans is a; then a=100;so:a=100,b=2;
//}
//struct nodec{
//    int node;
//};
//struct node{
//    int a;
//    int b;
//    struct nodec npt;
//};
//int ret[6];
//struct node d;
//int main(){
//    struct node father;
//    father.a=(father.b=101);
//    father.npt.node=202;
//    d.npt=father.npt;
//    ret[0]=father.a+father.b+father.npt.node;   //8 == 404
//    ret[1]=d.npt.node;  //12 == 202
//    ret[2]=father.npt.node; //16 == 202
//    ret[3]=father.a+father.b; //20 == 202
//    ret[4]=father.a;//24 == 101
//    ret[5]=father.b;//28 == 101
//}
//
//struct nodec{
//    int nodea;
//    int nodeb;
//};
//struct node{
//    int a;
//    int b;
//    struct nodec npt;
//};
//int ret[6];
//int main(){
//    struct nodec nc;
//    struct node nco[3];
//    nc.nodea=1;
//    nc.nodeb=2;
//    //init
//    nco[0].a=0;
//    nco[0].b=0;
//    nco[0].npt.nodea=0;
//    nco[0].npt.nodeb=0;
//    //
//    nco[0].npt=nc;//test nodec copy
//    nco[0].a=104;
//    nco[0].b=108;
////    //init ncopy
//    nco[1].a=999;
//    nco[1].b=999;
//    nco[1].npt.nodea=999;
//    nco[1].npt.nodeb=999;
//    //copy
//    nco[1]=nco[0];
//    //return ans;
//    ret[0]=nco[1].a;    //104            8
//    ret[1]=nco[1].b;    //108            12
//    ret[2]=nco[1].npt.nodea;    //1      16
//    ret[3]=nco[1].npt.nodeb;    //2      20
//    //dirty
//    nco[2].a=888;
//    nco[2].b=887;
//    nco[2].npt.nodea=886;
//    nco[2].npt.nodeb=885;
//    nco[1]=nco[2];
//    nc.nodea=7;
//    nc.nodeb=6;
//}
/*
pm 8 ret0
pm 12 ret1
pm 16 ret2
pm 20 ret3
pm 24 ret4
sh
*/
//
//struct nodec{
//    int nodea;
//    int nodeb;
//};
//struct node{
//    int a;
//    int b;
//    struct nodec npt;
//};
//int ret[6];
//struct nodec nc;
//struct node nco[2];
//int main(){
//    nc.nodea=1;
//    nc.nodeb=2;
//    //init
//    nco[0].a=0;
//    nco[0].b=0;
//    nco[0].npt.nodea=0;
//    nco[0].npt.nodeb=0;
//    //
//    nco[0].npt=nc;//test nodec copy
//    nco[0].a=104;
//    nco[0].b=108;
//    //    //init ncopy
//    nco[1].a=999;
//    nco[1].b=999;
//    nco[1].npt.nodea=999;
//    nco[1].npt.nodeb=999;
//    //copy
//    nco[1]=nco[0];
//    //return ans;
//    ret[0]=nco[1].a;    //104            8
//    ret[1]=nco[1].b;    //108            12
//    ret[2]=nco[1].npt.nodea;    //1      16
//    ret[3]=nco[1].npt.nodeb;    //2      20
//}
/*
 pm 8 ret0
 pm 12 ret1
 pm 16 ret2
 pm 20 ret3
 pm 24 ret4
 sh
 */
//struct node{
//    int a;
//    int arr[3];
//    int b;
//};
//int ret[10];
//int main(){
//    struct node st;
//    int i;
//    i=0;
//    while (i<3) {
//        st.arr[i]=i+500;
//        i=i+1;
//    }
//
//    st.a=999;
//    st.b=888;
//    i=0;
//    while (i<3) {
//        ret[i]=st.arr[i];
//        i=i+1;
//    }
//
//}
/*
 c
 pm 8 ret0
 pm 12 ret1
 pm 16 ret2
 pm 20 ret3
 sh
 exit
*/
//struct node{
//    int a;
//    int b;
//};
//int ret;
//int main(){
//    struct node st;
//    st.a=1;
//    st.b=2;
//    struct node *p;
//    p=&st;
//    (*p).a=5;
//    (*p).b=6;
//    ret=st.a+st.b;
//}
/*
 c
 pm 4 ret
 exit
 */
//struct node{
//    struct node *next;
//    int v;
//};
//int ret[5];
//
//int main(){
//    int co[5];
//    struct node lst[5];
//    struct node *head;
//    head = &lst[0];
//    co[0]=1;
//    co[1]=2;
//    co[2]=3;
//    co[3]=4;
//    co[4]=100;
//    int i;
//    i=0;
//    struct node *p;
//    p=head;
//    while(co[i]<100){
//        (*p).v=co[i];
//
//        (*p).next=&lst[i+1];
//        p=(*p).next;
//        i=i+1;
//    }
//    (*p).next=0;
//    i=0;
//    p=head;
//    while (p) {
//        ret[i]=(*p).v;
//        p=(*p).next;
//        i=i+1;
//    }
//}
/*
 c
 pm 8 ret0
 pm 12 ret1
 pm 16 ret2
 pm 20 ret3
 sh
 exit
 */
//int ret;
////calc:n+2+..+50
//int func(int n){
//    while (50<n) {
//        return 0;
//    }
//    return func(n+1)+n;
//}
//int main(){
//    ret=func(1);//ret=1275
//}
//
//int ret;
//int main(){
//    int i;
//    i=1;
//    ret=999;
//    if(i<02){
//        ret=4041;
//    }
//    else{
//        ret=222;
//    }
//
//}
//calc n+2+..+50
//int ret;
//int f(int n){
//    if(50<n){
//        return 0;
//    }
//    else
//        return f(n+1)+n;
//}
//int main(){
//    ret=f(1);//1275
//}
//int ret;
//int main(){
//    ret=0;
//    int i;
//    i=3;
//    if(i<2)
//    {
//        ret=222;
//    }
//    else
//        ret=344;
//
//}
//int ret;
//int main(){
//    int i;
//    i=0-1;
//    if(i<0){
//        ret=0-1;
//    }
//    else{
//        ret=0-2;
//    }
//
//}
//int ret;
//int main(){
//    int i;
//    i=2;
//    if(i==1 || i==2){
//        ret=1;
//    }
//    else{
//        ret=9;
//    }
//    return ret;
//}
//int map[4][5];
//int h;
//int w;
///*
//01000
//00010
//10100
//00100
//*/
//struct coor{
//	int x;
//	int y;
//};
//struct coor start;
//struct coor aim;//aim cannot equals to start
//int wall;
//int getMap(){
//	h=4;
//	w=5;
//	wall=1;
//	map[0][0]=0;
//	map[0][1]=1;
//	map[0][2]=0;
//	map[0][3]=0;
//	map[0][4]=0;
//	
//	map[1][0]=0;
//	map[1][1]=0;
//	map[1][2]=0;
//	map[1][3]=1;
//	map[1][4]=0;
//	
//	map[2][0]=1;
//	map[2][1]=0;
//	map[2][2]=1;
//	map[2][3]=0;
//	map[2][4]=0;
//
//	map[3][0]=0;
//	map[3][1]=0;
//	map[3][2]=1;
//	map[3][3]=0;
//	map[3][4]=0;
//	start.x=0;
//	start.y=0;
//	aim.x=4;
//	aim.y=3;
//}
//int book[5][5];
//int check(int ptr){
//	struct coor *p;
//	p=(struct coor*)ptr;
//	int x=(*p).x;
//	int y=(*p).y;
//	if(x<0 || y<0 || w-1<x || h-1<y){
//		return 0;
//	}
//	if(book[y][x]){
//		return 0;
//	}
//	if(map[y][x]==wall){
//		return 0;
//	}
//	return 1;
//}
//struct coor que[24];
//int xi[4];
//int yi[4];
//int init(){
//	xi[0]=0;
//	xi[1]=0;
//	xi[2]=1;
//	xi[3]=-1;
//	yi[0]=1;
//	yi[1]=-1;
//	yi[2]=0;
//	yi[3]=0;
//}
//int bfs(){
//	int i;
//	int head;
//	int end;
//	head=0;
//	end=1;
//	que[0]=start;
//	book[start.y][start.x]=1;
//	while(head<end){
//		struct coor cur;
//		i=0;
//		while(i<4){
//			cur=que[head];
//			cur.x=cur.x+xi[i];
//			cur.y=cur.y+yi[i];
//			i=i+1;
//			if(check((int)&cur)){
//				que[end]=cur;
//				book[cur.y][cur.x]=1;
//				if(cur.x==aim.x && cur.y==aim.y){
//					return 1;
//				}
//				end=end+1; 
//			}
//		}
//		head=head+1;
//	}
//	return 0;
//}
//int main(){
//	getMap();
//	init();
//	return bfs();
//}

int main(){
	return 1+3; 
} 




