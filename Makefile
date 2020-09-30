EXECUTABLE=AWSimulation

#CPPFLAGS=-g $(shell root-config --cflags)
#LDFLAGS=-g $(shell root-config --ldflags)
#LDLIBS=$(shell root-config --libs)

SRCS=ConsoleApplication1.cpp MasterBoard.cpp Minion.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>> ./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(EXECUTABLE) .depend

include .depend

