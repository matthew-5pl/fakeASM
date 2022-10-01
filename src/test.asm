main:
    mov ra,255
    mov rb,0
    ; convert
    mov rb,ra
    mod rb,2
    arr rb
    div ra,2
    push ra
    ; end
    jnz 4
    oar
    och nl
    ret