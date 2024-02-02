all : generate_scenarios

ifeq ($(DEBUG_MODE), true)
 $(info DEBUG MODE)
 CCFLAGS= -g
else
 $(info PRODUCTION MODE)
 CCFLAGS = -O3 
endif

CCC = g++
LIBS= -lm
TARGETDIR=.
CPPFLAGS += \
         -I.

OBJS2 = \
	    $(TARGETDIR)/Block.o \
		$(TARGETDIR)/Params.o \
		$(TARGETDIR)/Point.o \
		$(TARGETDIR)/Polygon.o \
		$(TARGETDIR)/main.o 


$(TARGETDIR)/generate_scenarios: $(OBJS2)
	$(CCC)  $(CCFLAGS) -o $(TARGETDIR)/generate_scenarios $(OBJS2)
	
$(TARGETDIR)/Block.o: Block.h Block.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Block.cpp -o $(TARGETDIR)/Block.o

$(TARGETDIR)/Polygon.o: Polygon.h Polygon.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Polygon.cpp -o $(TARGETDIR)/Polygon.o

$(TARGETDIR)/Point.o: Point.h Point.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Point.cpp -o $(TARGETDIR)/Point.o
	
$(TARGETDIR)/main.o: main.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c main.cpp -o $(TARGETDIR)/main.o

$(TARGETDIR)/Params.o: Params.h Params.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Params.cpp -o $(TARGETDIR)/Params.o

clean:
	 $(RM) generate_scenarios \
     $(TARGETDIR)/main.o \
	 $(TARGETDIR)/Block.o \
     $(TARGETDIR)/Point.o \
	 $(TARGETDIR)/Polygon.o \
     $(TARGETDIR)/Params.o 

