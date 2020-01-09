# name of the program (Minix service)
PROG=lab5

# source code files to be compiled
SRCS = kbc_utils.c lab5.c lab5test.c timer_utils.c utils.c vg.c vg_utils.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies (for Lab 3, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
