all : compactness-evaluation

CCC = g++
CCFLAGS = -O3 
LIBS= -lm
TARGETDIR=.
CPPFLAGS += \
         -I.
OBJS2 = \
	    $(TARGETDIR)/main.o \
        $(TARGETDIR)/Params.o \
		$(TARGETDIR)/Point.o


$(TARGETDIR)/compactness-evaluation: $(OBJS2)
	$(CCC)  $(CCFLAGS) -o $(TARGETDIR)/compactness-evaluation $(OBJS2)
	
$(TARGETDIR)/Point.o: Point.h Point.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Point.cpp -o $(TARGETDIR)/Point.o
	
$(TARGETDIR)/main.o: main.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c main.cpp -o $(TARGETDIR)/main.o

$(TARGETDIR)/Params.o: Params.h Params.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Params.cpp -o $(TARGETDIR)/Params.o

clean:
	 $(RM) compactness-evaluation \
     $(TARGETDIR)/main.o \
	 $(TARGETDIR)/Block.o \
	 $(TARGETDIR)/Compactness.o \
	 $(TARGETDIR)/District.o \
     $(TARGETDIR)/Point.o \
     $(TARGETDIR)/Params.o 
