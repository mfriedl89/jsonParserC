CFLAGS+=-Wall  -std=gnu99 -O2
LDFLAGS+=`curl-config --libs`

TARGETS= test_frm_parse_json
OBJS=buf.o json.o jsmn.o

all: $(TARGETS) .gitignore

clean:
	$(RM) $(TARGETS)
	$(RM) $(OBJS)

test_frm_parse_json: $(OBJS) test_frm_parse_json.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
