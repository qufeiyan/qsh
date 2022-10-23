PREFIX = 
CC := $(PREFIX)gcc
AR := $(PREFIX)ar

$(warning "cc : $(CC)")
$(warning "ar : $(AR)")

DFLAG += -DUSE_DEFAULT
CFLAG += -g -O1
IFLAG += -I.

OBJ_DIR = ./objs

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
# is equal:
# OBJ = $(SRC:%.c=%.o)

EXCLUDE := test.o
LIB_OBJ := $(filter-out $(EXCLUDE), $(OBJ))

LIB_NAME = qsh
LIB = lib$(LIB_NAME).a

TARGET = test
 
all : desc $(LIB) $(TARGET)
.PHONE : clean 

# generate static library.
lib : $(LIB) 

desc : 
	@echo "src : $(SRC)"
	@echo "obj : $(OBJ)"
	@echo "lib_obj : $(LIB_OBJ)"

$(OBJ_DIR) :
	$(shell if [ ! -d $@ ]; then mkdir -p $@; fi)

$(OBJ) : %.o : %.c $(OBJ_DIR)
	$(CC) -c $(CFLAG) $(DFLAG) $(IFLAG) $< -o $(OBJ_DIR)/$@

$(LIB) : $(LIB_OBJ)
	$(AR) rscv $@ $(addprefix $(OBJ_DIR)/, $(LIB_OBJ))

$(TARGET) : $(OBJ) 
	$(CC) -Tqsh.lds $(CFLAG) $(DFLAG) $(IFLAG) $(addprefix $(OBJ_DIR)/, $(OBJ)) -o $@

clean :
	$(RM) -rf *.a *.o $(OBJ_DIR)
	$(RM) $(TARGET)