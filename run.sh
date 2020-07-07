debug=/Users/dawn/xcode/Build/Products/Debug
output=/Users/dawn/f/lan
#gcc_simple输出汇编到code.txt 手动执行
#asslan读入code.txt，输出二进制到run.txt

$debug/asslan > $output/run.c
if (($?==0))
then
	echo asslan success
else
	echo asslan failed
	exit
fi
echo =======end======
$debug/mips_simulator
