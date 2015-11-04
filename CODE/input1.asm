data segment
         list db 04h,39h,12h,02h
         count equ 04
data ends

code segment
assume cs:code,ds:data
start:
       mov ax,data
       mov ds,ax
       mov dl,count-1

again0:
        mov cl,dl
        mov si,00

again1:
       mov ax,si
       cmp al,[si+1]
       jl pr1
       xchg al,[si+1]
       xchg al,[si]

pr1: add si,01
    loop again1
    dec dx
    jnz again0

    int 21h
    mov ax,4c00h
    int 21h
    code ends   

