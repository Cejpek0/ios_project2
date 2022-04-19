parameters=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lpthread
cfiles = cfiles/main.c
output = proj2
app: ${cfiles}
	gcc ${parameters} -o ${output} ${cfiles}