gcc -c graph.c -fPIC
gcc -Wall -shared -o my_semaphore.so my_semaphore.c graph.o -ldl -fPIC -lm
gcc -Wall -o aplicacao aplicacao2.c -lpthread
LD_PRELOAD=./my_semaphore.so ./aplicacao
