# @Author: qlc
# @Date:   2018-10-30T10:46:21+08:00
# @Email:  qlcx@tom.com
# @Filename: Makefile
# @Last modified by:   qlc
# @Last modified time: 2018-10-30T11:54:06+08:00
# @License: GPL
TOOLCHAIN=
TAGET=test
SRC=$(shell find ./ -name "*.c")
OBJ_O=${patsubst %.c,%.o,$(SRC)}
OBJ_D=${patsubst %.c,%.d,$(SRC)}
INC_H=-I$(shell pwd)/json/
LIB_S=-lm
$(TAGET):$(OBJ_O)
	$(TOOLCHAIN)gcc -o $@ $^ $(LIB_S)
.c.o:
	$(TOOLCHAIN)gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" $(INC_H)
clean:
	rm -rf $(OBJ_O) $(OBJ_D) $(TAGET)
