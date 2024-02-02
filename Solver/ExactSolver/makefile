all : exact-solver

CCC = g++

ifeq ($(DEBUG_MODE), true)
 $(info DEBUG MODE)
 CCFLAGS = -g 
else
 $(info PRODUCTION MODE)
 CCFLAGS = -O3 
endif

LIBS= -L${GUROBI_HOME}/lib \
      -lgurobi_c++ -lgurobi91 \
	  -lm

TARGETDIR=./
CPPFLAGS += \
         -I${GUROBI_HOME}/include

OBJS2 = \
	    $(TARGETDIR)/CostEvaluator.o \
		$(TARGETDIR)/GurobiSolver.o \
        $(TARGETDIR)/Solver.o \
		$(TARGETDIR)/../ILS/src/CommandLineHelper.o \
        $(TARGETDIR)/../ILS/src/Instance.o \
        $(TARGETDIR)/../ILS/src/Solution.o \
		$(TARGETDIR)/../ILS/src/utils.o \
        $(TARGETDIR)/main.o 


exact-solver: $(OBJS2)
	$(CCC)  $(CCFLAGS) -o exact-solver $(OBJS2) $(LIBS)

$(TARGETDIR)/CostEvaluator.o:  CostEvaluator.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c CostEvaluator.cpp -o $(TARGETDIR)/CostEvaluator.o 

$(TARGETDIR)/GurobiSolver.o:  GurobiSolver.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c GurobiSolver.cpp -o $(TARGETDIR)/GurobiSolver.o 

$(TARGETDIR)/Solver.o: Solver.h Solver.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Solver.cpp -o $(TARGETDIR)/Solver.o 

$(TARGETDIR)/main.o: main.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c main.cpp -o $(TARGETDIR)/main.o 


clean:
	 $(RM) \
     $(TARGETDIR)/CostEvaluator.o \
	 $(TARGETDIR)/GurobiSolver.o \
     $(TARGETDIR)/Solver.o \
     $(TARGETDIR)/main.o 