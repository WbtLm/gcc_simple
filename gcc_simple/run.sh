asslan=./asslan
output=run.c
code=/c/Users/John/Desktop/毕设/毕设lan1.1.1/lan1.1/code.c
mips_simulator=./mips
#gcc_simple输出汇编到code.txt 手动执行
#asslan读入code.txt，输出二进制到run.txt

$asslan $code> $output
if (($?==0))
then
	echo asslan success
else
	echo asslan failed
	exit
fi
echo =======end======
$mips_simulator $output
