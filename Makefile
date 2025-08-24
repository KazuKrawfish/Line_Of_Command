EXECUTABLE=Line_Of_Command


LDLIBS=

SOURCE += compie.cpp
SOURCE += compieMinionRecord.cpp
SOURCE += compiePropertyRecord.cpp
SOURCE += Cursor.cpp
SOURCE += inputLayer.cpp
SOURCE += main.cpp
SOURCE += mainmenu.cpp
SOURCE += MasterBoard.cpp 
SOURCE += Minion.cpp

       
OBJS=$(subst .cpp,.o,$(SOURCE))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SOURCE)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>> ./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(EXECUTABLE) .depend

include .depend

