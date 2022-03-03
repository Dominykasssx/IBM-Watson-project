BIN:=ibm_watson_program
OBJS := arguments_parse.o main.o invoke.o ibm_watson.o 
FLAGS:=-liotp-as-device -lpaho-mqtt3as -lblobmsg_json -largp -lubox -lubus

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC)  -o $@ $^ $(FLAGS)

clean:
	$(RM) -f $(BIN) $(OBJS)
