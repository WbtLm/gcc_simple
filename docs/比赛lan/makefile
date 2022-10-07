input='/c/Users/John/Desktop/毕设/毕设lan1.1.1/lan1.1/input.c'
midFile=../mid.c
#---------------------------------


mainFile=main.cpp data_type_func.o data_type.o alloc.o utils.o symble.o syntax_analyze_matrix.o lexical_analyze.o tree_node.o guidance.o struct_type_dic.o
compileOrder=g++ -std=c++11 -c

all:run.c
	./mips run.c
run.c:code.c 
	./asslan code.c>run.c
	echo asslan success run.c已更新

code.c:mid.c background.out 
	./background.out mid.c
	echo code.c已更新
mid.c:main.out input.c
	./main.out 
	echo main.out已更新
	echo mid.c已更新
main.out:$(mainFile)
	g++ -std=c++11 -o main.out $(mainFile)
	echo main.out已更新
#后端
background.out:background/background.cpp background/background.h macro.h alloc.h instruction.o back_var_ctrl.o macro_inst.o alloc.o
	g++ -std=c++11 -o background.out background/background.cpp instruction.o back_var_ctrl.o macro_inst.o alloc.o
	echo background.out已更新

instruction.o:background/instruction.cpp background/instruction.h macro.h alloc.h
	$(compileOrder) background/instruction.cpp
	echo instruction.o已更新
macro_inst.o:background/macro_inst.cpp background/macro_inst.h back_var_ctrl.o
	$(compileOrder) background/macro_inst.cpp back_var_ctrl.o
	echo macro_inst.o已更新

back_var_ctrl.o:background/back_var_ctrl.cpp background/back_var_ctrl.h macro.h alloc.o
	$(compileOrder) background/back_var_ctrl.cpp alloc.o
	echo back_var_ctrl.o已更新
#前端：
struct_type_dic.o:struct_type_dic.cpp struct_type_dic.h
	$(compileOrder) struct_type_dic.cpp
	echo struct_type_dic.o已更新
guidance.o:guidance.cpp guidance.h symble.o tree_node.o 
	$(compileOrder) guidance.cpp symble.o tree_node.o
	echo guidance.o已更新
tree_node.o:tree_node.cpp tree_node.h symble.o
	$(compileOrder) tree_node.cpp symble.o
	echo tree_node.o已更新
syntax_analyze_matrix.o:syntax_analyze_matrix.cpp syntax_analyze_matrix.h lexical_analyze.o
	$(compileOrder) syntax_analyze_matrix.cpp lexical_analyze.o
	echo syntax_analyze_matrix.o已更新
lexical_analyze.o:lexical_analyze.cpp lexical_analyze.h
	$(compileOrder) lexical_analyze.cpp
	echo lexical_analyze.o已更新
symble.o:symble.cpp symble.h alloc.o data_type.o
	$(compileOrder) symble.cpp alloc.o data_type.o
	echo symble.o已更新
data_type_func.o:data_type_func.cpp data_type_func.h
	$(compileOrder) data_type_func.cpp
	echo data_type_func.o已更新
data_type.o:data_type.cpp data_type.h
	$(compileOrder) data_type.cpp 
	echo data_type.o已更新
alloc.o:alloc.cpp alloc.h
	$(compileOrder) alloc.cpp
	echo alloc.o已更新
utils.o:utils.cpp utils.h
	$(compileOrder) utils.cpp
	echo utils.o已更新
clean:
	rm *.o
	rm *.out
	rm code.c
	rm run.c
	rm mid.c
