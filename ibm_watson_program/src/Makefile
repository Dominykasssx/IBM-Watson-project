BIN:=ibm_watson_program
OBJS := program.o invoke.o
FLAGS:=-liotp-as-device -lpaho-mqtt3as -lblobmsg_json -largp -lubox -lubus

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC)  -o $@ $^ $(FLAGS)

clean:
	$(RM) -f $(BIN) $(OBJS)
