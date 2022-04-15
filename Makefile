parameters=-std=gnu99 -Wall -Wextra -Werror -pedantic
cfiles = cfiles/main.c cfiles/functions.c cfiles/oxygen.c cfiles/hydrogen.c 
hfiles = hfiles/functions.h hfiles/oxygen.h hfiles/hydrogen.h 
output = proj2
app: ${hfiles} ${cfiles}
	gcc ${parameters} -o ${output} ${cfiles}