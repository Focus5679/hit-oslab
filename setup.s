.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

BOOTSEG  = 0x07c0			! original address of boot-sector
INITSEG  = 0x9000			! we move boot here - out of the way
SETUPSEG = 0x9020			! setup starts here

entry _start
_start:
	mov	ax,cs
	mov	ds,ax
	mov	es,ax

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10

	mov	cx,#28			! 要显示的字符串长度
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

! ok, the read went well so we get current cursor position and save it for
! posterity.
! 获取光标位置=>0x9000:0
	mov	ax,#INITSEG	! this is done in bootsect already, but...
	mov	ds,ax
	mov	ah,#0x03	! read cursor pos
	xor	bh,bh
	int	0x10		! save it in known place, con_init fetches
	mov	[0],dx		! it from 0x90000.

! Get memory size (extended mem, kB)
! 获取扩展内存大小 => 0x9000:2
	mov	ah,#0x88
	int	0x15
	mov	[2],ax

! Get hd0 data
! 获取硬盘参数 => 0x9000:80
	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x41]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0080
	mov	cx,#0x10
	rep
	movsb

! 设置ds为0x9000
	mov	ax,#INITSEG
	mov	ds,ax
	mov	ax,#SETUPSEG
	mov	es,ax

! 打印光标提示信息
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10

	mov	cx,#11			! 要显示的字符串长度
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg2
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

! 调用print_hex显示具体信息
	mov	ax,[0]
	call	print_hex
	call	print_nl

! 打印内存提示信息
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10

	mov	cx,#12			! 要显示的字符串长度
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg3
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

! 调用print_hex显示具体信息
	mov	ax,[2]
	call	print_hex
	call	print_nl

l:
	jmp l

!以16进制方式打印ax寄存器里的16位数字
print_hex:
	mov	cx,#4		!4个十六进制数字
	mov	dx,ax		!将ax所指的值放入dx中,ax作为参数传递寄存器
print_digit:
	rol	dx,#4		!循环左移4位	移动dx的高4位至低4位处
	mov	ax,#0xe0f	!ah=e请求的功能值,al=半字节掩码
	and	al,dl		!取dl低4位值
	add	al,#0x30	!给al数字加上十六进制数字0x30
	cmp	al,#0x3a
	jl	outp		!是一个不大于10的数字
	add	al,#0x07	!是a~f,要多加7
outp:
	int	0x10
	loop	print_digit
	ret

msg1:
	.byte 13,10
	.ascii "Now we are in setup..."
	.byte 13,10,13,10

msg2:
	.ascii "Cursor Pos:"

msg3:
	.ascii "Memory Size:"

!打印回车换行
print_nl:
	mov	ax,#0xe0d
	int	0x10
	mov	al,#0xa
	int	0x10
	ret

.text
endtext:
.data
enddata:
.bss
endbss:
