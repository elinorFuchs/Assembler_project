MAIN: mov @r1,@r2
LOOP: cmp -5,@r3
bne ENDLOOP
add @r4,R0
.entry START ,		 EXT5./
.entry SUBROUTINE
START: prn STR
SUBROUTINE: bne EXTERNVAR
jsr SUBROUTINE
lea ARR,@r5
stop
ENDLOOP: dec K
not @r2
inc R0
bne MAIN
MAIN1: mov @r1,@r2
LOOP2: cmp -5,@r2
EXTERNVAR: .data 100
.string "FRE"
RH5: .data 15,17
EXT: .data 100
EXT2: .data 100
EXT3: .data 100
EXT4: .data 100
EXT5: .data 100
.extern V5 , K7
