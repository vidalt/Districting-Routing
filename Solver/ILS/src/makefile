all : solver

CCC = g++

ifeq ($(DEBUG_MODE), true)
 $(info DEBUG MODE)
 CCFLAGS = -g 
else
 $(info PRODUCTION MODE)
 CCFLAGS = -O3 -std=c++17 
endif

LIBS= -L${GUROBI_HOME}/lib \
      -lgurobi_c++ -lgurobi91 \
	  -lm

TARGETDIR=./
CPPFLAGS += \
         -I${GUROBI_HOME}/include

OBJS2 = \
   	    $(TARGETDIR)/NeuralNetEvaluator.o \
        $(TARGETDIR)/Block.o \
        $(TARGETDIR)/ILS.o \
        $(TARGETDIR)/Instance.o \
		$(TARGETDIR)/GurobiSolver.o \
        $(TARGETDIR)/LocalSearch.o \
        $(TARGETDIR)/Solution.o \
        $(TARGETDIR)/CommandLineHelper.o \
        $(TARGETDIR)/utils.o \
        $(TARGETDIR)/main.o 


solver: $(OBJS2)
	$(CCC)  $(CCFLAGS) -o ../districting_solver $(OBJS2) $(LIBS)

$(TARGETDIR)/NeuralNetEvaluator.o:  NeuralNetEvaluator.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c NeuralNetEvaluator.cpp -o $(TARGETDIR)/NeuralNetEvaluator.o 

$(TARGETDIR)/Block.o: Block.h Block.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Block.cpp -o $(TARGETDIR)/Block.o 

$(TARGETDIR)/CommandLineHelper.o: CommandLineHelper.h CommandLineHelper.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c CommandLineHelper.cpp -o $(TARGETDIR)/CommandLineHelper.o 

$(TARGETDIR)/ILS.o: ILS.h ILS.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c ILS.cpp -o $(TARGETDIR)/ILS.o 
	
$(TARGETDIR)/GurobiSolver.o: GurobiSolver.h GurobiSolver.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c GurobiSolver.cpp -o $(TARGETDIR)/GurobiSolver.o 

$(TARGETDIR)/Instance.o: Instance.h Instance.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Instance.cpp -o $(TARGETDIR)/Instance.o 

$(TARGETDIR)/LocalSearch.o: LocalSearch.h LocalSearch.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c LocalSearch.cpp -o $(TARGETDIR)/LocalSearch.o 
	
$(TARGETDIR)/Solution.o: Solution.h Solution.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Solution.cpp -o $(TARGETDIR)/Solution.o 

$(TARGETDIR)/utils.o: utils.h utils.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c utils.cpp -o $(TARGETDIR)/utils.o 	

$(TARGETDIR)/main.o: main.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c main.cpp -o $(TARGETDIR)/main.o 


clean:
	 $(RM) \
     $(TARGETDIR)/main.o \
	 $(TARGETDIR)/BeardwoodEvaluator.o \
     $(TARGETDIR)/NeuralNetEvaluator.o \
     $(TARGETDIR)/CommandLineHelper.o \
     $(TARGETDIR)/Block.o \
     $(TARGETDIR)/ILS.o \
     $(TARGETDIR)/Instance.o \
     $(TARGETDIR)/GurobiSolver.o \
     $(TARGETDIR)/LocalSearch.o \
     $(TARGETDIR)/Solution.o \
     $(TARGETDIR)/main.o