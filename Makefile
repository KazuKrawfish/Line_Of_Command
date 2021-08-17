EXECUTABLE=AWSimulation

#CPPFLAGS=-g $(shell root-config --cflags)
#LDFLAGS=-g $(shell root-config --ldflags)
#LDLIBS=$(shell root-config --libs)

LDLIBS=-lncurses

SRCS=MasterBoard.cpp Minion.cpp compie.cpp  Cursor.cpp inputLayer.cpp main.cpp  mainmenu.cpp
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

