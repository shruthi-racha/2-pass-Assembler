data segment
	a db 10h
	b dw 20h
	c equ 30
data ends

last1:
	mov al,a
	add bx,b
	jmp last
	div bx
	shr ax,1
last:
	mov ah,4ch
	int 21h
	end
  