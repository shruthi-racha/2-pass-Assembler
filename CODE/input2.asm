prints macro msg
        mov ah,09h
        mov dx,offset msg
        int 21h
endm

data segment
        ms db "Hello"
        num db 65
data ends

code segment
assume cs:code,ds:data

        mov ax,data
        mov ds,ax
        
        prints ms
        mov ax, 4ch
        int 21h


code ends
