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
        $(TARGETDIR)/TSPCostEvaluator.o \
        $(TARGETDIR)/Block.o \
        $(TARGETDIR)/ILS.o \
        $(TARGETDIR)/Instance.o \
		$(TARGETDIR)/GurobiSolver.o \
        $(TARGETDIR)/LocalSearch.o \
        $(TARGETDIR)/Solution.o \
        $(TARGETDIR)/Point.o \
        $(TARGETDIR)/CommandLineHelper.o \
        $(TARGETDIR)/utils.o \
        $(TARGETDIR)/../lib/LKH/Activate.o \
		$(TARGETDIR)/../lib/LKH/AddCandidate.o \
		$(TARGETDIR)/../lib/LKH/AddExtraCandidates.o \
		$(TARGETDIR)/../lib/LKH/AddTourCandidates.o \
		$(TARGETDIR)/../lib/LKH/AdjustCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/AdjustClusters.o \
		$(TARGETDIR)/../lib/LKH/AllocateStructures.o \
		$(TARGETDIR)/../lib/LKH/Ascent.o \
		$(TARGETDIR)/../lib/LKH/Best2OptMove.o \
		$(TARGETDIR)/../lib/LKH/Best3OptMove.o \
		$(TARGETDIR)/../lib/LKH/Best4OptMove.o \
		$(TARGETDIR)/../lib/LKH/Best5OptMove.o \
		$(TARGETDIR)/../lib/LKH/BestKOptMove.o \
		$(TARGETDIR)/../lib/LKH/Between.o \
		$(TARGETDIR)/../lib/LKH/Between_SL.o \
		$(TARGETDIR)/../lib/LKH/Between_SSL.o \
		$(TARGETDIR)/../lib/LKH/BridgeGain.o \
		$(TARGETDIR)/../lib/LKH/BuildKDTree.o \
		$(TARGETDIR)/../lib/LKH/C.o \
		$(TARGETDIR)/../lib/LKH/CandidateReport.o \
		$(TARGETDIR)/../lib/LKH/ChooseInitialTour.o \
		$(TARGETDIR)/../lib/LKH/Connect.o \
		$(TARGETDIR)/../lib/LKH/Create_POPMUSIC_CandidateSet.o \
		$(TARGETDIR)/../lib/LKH/CreateCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/CreateDelaunayCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/CreateNNCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/CreateQuadrantCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/Delaunay.o \
		$(TARGETDIR)/../lib/LKH/Distance.o \
		$(TARGETDIR)/../lib/LKH/Distance_SPECIAL.o \
		$(TARGETDIR)/../lib/LKH/eprintf.o \
		$(TARGETDIR)/../lib/LKH/ERXT.o \
		$(TARGETDIR)/../lib/LKH/Excludable.o \
		$(TARGETDIR)/../lib/LKH/Exclude.o \
		$(TARGETDIR)/../lib/LKH/FindTour.o \
		$(TARGETDIR)/../lib/LKH/FixedOrCommonCandidates.o \
		$(TARGETDIR)/../lib/LKH/Flip.o \
		$(TARGETDIR)/../lib/LKH/Flip_SL.o \
		$(TARGETDIR)/../lib/LKH/Flip_SSL.o \
		$(TARGETDIR)/../lib/LKH/Forbidden.o \
		$(TARGETDIR)/../lib/LKH/FreeStructures.o \
		$(TARGETDIR)/../lib/LKH/fscanint.o \
		$(TARGETDIR)/../lib/LKH/Gain23.o \
		$(TARGETDIR)/../lib/LKH/GenerateCandidates.o \
		$(TARGETDIR)/../lib/LKH/Genetic.o \
		$(TARGETDIR)/../lib/LKH/GeoConversion.o \
		$(TARGETDIR)/../lib/LKH/GetTime.o \
		$(TARGETDIR)/../lib/LKH/gpx.o \
		$(TARGETDIR)/../lib/LKH/GreedyTour.o \
		$(TARGETDIR)/../lib/LKH/Hashing.o \
		$(TARGETDIR)/../lib/LKH/Heap.o \
		$(TARGETDIR)/../lib/LKH/IsBackboneCandidate.o \
		$(TARGETDIR)/../lib/LKH/IsCandidate.o \
		$(TARGETDIR)/../lib/LKH/IsCommonEdge.o \
		$(TARGETDIR)/../lib/LKH/IsPossibleCandidate.o \
		$(TARGETDIR)/../lib/LKH/KSwapKick.o \
		$(TARGETDIR)/../lib/LKH/LinKernighan.o \
		$(TARGETDIR)/../lib/LKH/LKHmain.o \
		$(TARGETDIR)/../lib/LKH/Make2OptMove.o \
		$(TARGETDIR)/../lib/LKH/Make3OptMove.o \
		$(TARGETDIR)/../lib/LKH/Make4OptMove.o \
		$(TARGETDIR)/../lib/LKH/Make5OptMove.o \
		$(TARGETDIR)/../lib/LKH/MakeKOptMove.o \
		$(TARGETDIR)/../lib/LKH/MergeTourWithBestTour.o \
		$(TARGETDIR)/../lib/LKH/MergeWithTourGPX2.o \
		$(TARGETDIR)/../lib/LKH/MergeWithTourIPT.o \
		$(TARGETDIR)/../lib/LKH/Minimum1TreeCost.o \
		$(TARGETDIR)/../lib/LKH/MinimumSpanningTree.o \
		$(TARGETDIR)/../lib/LKH/NormalizeNodeList.o \
		$(TARGETDIR)/../lib/LKH/NormalizeSegmentList.o \
		$(TARGETDIR)/../lib/LKH/OrderCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/PatchCycles.o \
		$(TARGETDIR)/../lib/LKH/printff.o \
		$(TARGETDIR)/../lib/LKH/PrintParameters.o \
		$(TARGETDIR)/../lib/LKH/Random.o \
		$(TARGETDIR)/../lib/LKH/ReadCandidates.o \
		$(TARGETDIR)/../lib/LKH/ReadEdges.o \
		$(TARGETDIR)/../lib/LKH/ReadLine.o \
		$(TARGETDIR)/../lib/LKH/ReadParameters.o \
		$(TARGETDIR)/../lib/LKH/ReadPenalties.o \
		$(TARGETDIR)/../lib/LKH/ReadProblem.o \
		$(TARGETDIR)/../lib/LKH/RecordBestTour.o \
		$(TARGETDIR)/../lib/LKH/RecordBetterTour.o \
		$(TARGETDIR)/../lib/LKH/RemoveFirstActive.o \
		$(TARGETDIR)/../lib/LKH/ResetCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/RestoreTour.o \
		$(TARGETDIR)/../lib/LKH/SegmentSize.o \
		$(TARGETDIR)/../lib/LKH/Sequence.o \
		$(TARGETDIR)/../lib/LKH/SFCTour.o \
		$(TARGETDIR)/../lib/LKH/SolveCompressedSubproblem.o \
		$(TARGETDIR)/../lib/LKH/SolveDelaunaySubproblems.o \
		$(TARGETDIR)/../lib/LKH/SolveKarpSubproblems.o \
		$(TARGETDIR)/../lib/LKH/SolveKCenterSubproblems.o \
		$(TARGETDIR)/../lib/LKH/SolveKMeansSubproblems.o \
		$(TARGETDIR)/../lib/LKH/SolveRoheSubproblems.o \
		$(TARGETDIR)/../lib/LKH/SolveSFCSubproblems.o \
		$(TARGETDIR)/../lib/LKH/SolveSubproblem.o \
		$(TARGETDIR)/../lib/LKH/SolveSubproblemBorderProblems.o \
		$(TARGETDIR)/../lib/LKH/SolveTourSegmentSubproblems.o \
		$(TARGETDIR)/../lib/LKH/Statistics.o \
		$(TARGETDIR)/../lib/LKH/StoreTour.o \
		$(TARGETDIR)/../lib/LKH/SymmetrizeCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/TrimCandidateSet.o \
		$(TARGETDIR)/../lib/LKH/WriteCandidates.o \
		$(TARGETDIR)/../lib/LKH/WritePenalties.o \
		$(TARGETDIR)/../lib/LKH/WriteTour.o \
        $(TARGETDIR)/main.o 


solver: $(OBJS2)
	$(CCC)  $(CCFLAGS) -o ../districting_solver $(OBJS2) $(LIBS)

$(TARGETDIR)/NeuralNetEvaluator.o:  NeuralNetEvaluator.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c NeuralNetEvaluator.cpp -o $(TARGETDIR)/NeuralNetEvaluator.o 

$(TARGETDIR)/TSPCostEvaluator.o:  TSPCostEvaluator.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c TSPCostEvaluator.cpp -o $(TARGETDIR)/TSPCostEvaluator.o 

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

$(TARGETDIR)/Point.o: Point.h Point.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Point.cpp -o $(TARGETDIR)/Point.o 
	
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
     $(TARGETDIR)/TSPCostEvaluator.o \
     $(TARGETDIR)/CommandLineHelper.o \
     $(TARGETDIR)/Block.o \
     $(TARGETDIR)/ILS.o \
     $(TARGETDIR)/Instance.o \
     $(TARGETDIR)/Point.o \
     $(TARGETDIR)/GurobiSolver.o \
     $(TARGETDIR)/LocalSearch.o \
     $(TARGETDIR)/Solution.o \
     $(TARGETDIR)/main.o