LABEL1: .data 1,2,3
mcr m1
	inc r2
	clr r4
endmcr
LABEL2: mov r1,LABEL1
m1
.string "abc"
