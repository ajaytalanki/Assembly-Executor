start:
       not r1 r0     ;  r1 = not garbage
       and r0 r1 r0  ;  r0 = 0
       not r1 r0     ;  r1 = FF
       add r2 r1 r1  ;  r2 = FE
       not r1 r2     ;  r1 = 1
       and r3 r0 r0  ;  display 0
       and r3 r1 r1  ;  display 1
loop:
       add r3 r1 r0  ;  r3 = r1 + r0
       and r0 r1 r1  ;  r0 = r1
       and r1 r3 r3  ;  r1 = r3
       bnz loop
