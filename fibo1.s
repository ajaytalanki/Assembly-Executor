start: not r1 r0
       and r0 r1 r0
       not r1 r0
       add r2 r1 r1
       not r1 r2
       and r3 r0 r0
       and r3 r1 r1
loop:  and r2 r1 r1
       add r3 r1 r0
       and r1 r3 r3
       and r0 r2 r2
       bnz loop
