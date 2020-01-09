PROG = proj
SRC_DIR = ./
INC_DIR = include/

# source code files to be compiled
SRCS != find $(SRC_DIR) -maxdepth 1 -type f -name '*.c'
#SRCS = bmp.c event_dispatcher.c kbc_utils.c kbd.c mouse.c skane_royale.c timer_utils.c utils.c vg.c vg_utils.c
CFLAGS += -I $(INC_DIR)
# CFLAGS += -Ofast
CFLAGS += -O3

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CPPFLAGS += -pedantic -D __LCOM_OPTIMIZED__
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
