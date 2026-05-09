1. change the printf from standard to https://github.com/mpaland/printf/blob/master/printf.c this is made for embedded and does not use the heap, there are some things that I need to figure out about this, like the buffer and things like that but that is not for now. (done);
2. make objects opaque
- gpio(done)
- itm (done)

3. deal with side effects for it. 
- gpio (done)

4. wrap the third-party code i.e. printf.c/h for itm

5. delay do that in general

6. set up button service

7. convert all enums that are used in bit operations to uint32_t. 

8. create masks for gpio bitshifts (current)

9. create critical sections for __IO variables when doing read/modify/write
