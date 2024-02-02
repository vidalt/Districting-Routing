ifeq ($(DEBUG_MODE), true)
 $(info DEBUG MODE)
 CCFLAGS = -g 
else
 $(info PRODUCTION MODE)
 CCFLAGS = -O3 
endif

all : cost-estimation

HELLO_DIR=LKH
CCC = g++


LIBS= -lm
TARGETDIR=.
CPPFLAGS += \
         -I.
OBJS2 = \
	    $(TARGETDIR)/Block.o \
		$(TARGETDIR)/District.o \
        $(TARGETDIR)/Point.o \
        $(TARGETDIR)/main.o \
        $(TARGETDIR)/Params.o \
		$(TARGETDIR)/LKH/Activate.o \
		$(TARGETDIR)/LKH/AddCandidate.o \
		$(TARGETDIR)/LKH/AddExtraCandidates.o \
		$(TARGETDIR)/LKH/AddTourCandidates.o \
		$(TARGETDIR)/LKH/AdjustCandidateSet.o \
		$(TARGETDIR)/LKH/AdjustClusters.o \
		$(TARGETDIR)/LKH/AllocateStructures.o \
		$(TARGETDIR)/LKH/Ascent.o \
		$(TARGETDIR)/LKH/Best2OptMove.o \
		$(TARGETDIR)/LKH/Best3OptMove.o \
		$(TARGETDIR)/LKH/Best4OptMove.o \
		$(TARGETDIR)/LKH/Best5OptMove.o \
		$(TARGETDIR)/LKH/BestKOptMove.o \
		$(TARGETDIR)/LKH/Between.o \
		$(TARGETDIR)/LKH/Between_SL.o \
		$(TARGETDIR)/LKH/Between_SSL.o \
		$(TARGETDIR)/LKH/BridgeGain.o \
		$(TARGETDIR)/LKH/BuildKDTree.o \
		$(TARGETDIR)/LKH/C.o \
		$(TARGETDIR)/LKH/CandidateReport.o \
		$(TARGETDIR)/LKH/ChooseInitialTour.o \
		$(TARGETDIR)/LKH/Connect.o \
		$(TARGETDIR)/LKH/Create_POPMUSIC_CandidateSet.o \
		$(TARGETDIR)/LKH/CreateCandidateSet.o \
		$(TARGETDIR)/LKH/CreateDelaunayCandidateSet.o \
		$(TARGETDIR)/LKH/CreateNNCandidateSet.o \
		$(TARGETDIR)/LKH/CreateQuadrantCandidateSet.o \
		$(TARGETDIR)/LKH/Delaunay.o \
		$(TARGETDIR)/LKH/Distance.o \
		$(TARGETDIR)/LKH/Distance_SPECIAL.o \
		$(TARGETDIR)/LKH/eprintf.o \
		$(TARGETDIR)/LKH/ERXT.o \
		$(TARGETDIR)/LKH/Excludable.o \
		$(TARGETDIR)/LKH/Exclude.o \
		$(TARGETDIR)/LKH/FindTour.o \
		$(TARGETDIR)/LKH/FixedOrCommonCandidates.o \
		$(TARGETDIR)/LKH/Flip.o \
		$(TARGETDIR)/LKH/Flip_SL.o \
		$(TARGETDIR)/LKH/Flip_SSL.o \
		$(TARGETDIR)/LKH/Forbidden.o \
		$(TARGETDIR)/LKH/FreeStructures.o \
		$(TARGETDIR)/LKH/fscanint.o \
		$(TARGETDIR)/LKH/Gain23.o \
		$(TARGETDIR)/LKH/GenerateCandidates.o \
		$(TARGETDIR)/LKH/Genetic.o \
		$(TARGETDIR)/LKH/GeoConversion.o \
		$(TARGETDIR)/LKH/GetTime.o \
		$(TARGETDIR)/LKH/gpx.o \
		$(TARGETDIR)/LKH/GreedyTour.o \
		$(TARGETDIR)/LKH/Hashing.o \
		$(TARGETDIR)/LKH/Heap.o \
		$(TARGETDIR)/LKH/IsBackboneCandidate.o \
		$(TARGETDIR)/LKH/IsCandidate.o \
		$(TARGETDIR)/LKH/IsCommonEdge.o \
		$(TARGETDIR)/LKH/IsPossibleCandidate.o \
		$(TARGETDIR)/LKH/KSwapKick.o \
		$(TARGETDIR)/LKH/LinKernighan.o \
		$(TARGETDIR)/LKH/LKHmain.o \
		$(TARGETDIR)/LKH/Make2OptMove.o \
		$(TARGETDIR)/LKH/Make3OptMove.o \
		$(TARGETDIR)/LKH/Make4OptMove.o \
		$(TARGETDIR)/LKH/Make5OptMove.o \
		$(TARGETDIR)/LKH/MakeKOptMove.o \
		$(TARGETDIR)/LKH/MergeTourWithBestTour.o \
		$(TARGETDIR)/LKH/MergeWithTourGPX2.o \
		$(TARGETDIR)/LKH/MergeWithTourIPT.o \
		$(TARGETDIR)/LKH/Minimum1TreeCost.o \
		$(TARGETDIR)/LKH/MinimumSpanningTree.o \
		$(TARGETDIR)/LKH/NormalizeNodeList.o \
		$(TARGETDIR)/LKH/NormalizeSegmentList.o \
		$(TARGETDIR)/LKH/OrderCandidateSet.o \
		$(TARGETDIR)/LKH/PatchCycles.o \
		$(TARGETDIR)/LKH/printff.o \
		$(TARGETDIR)/LKH/PrintParameters.o \
		$(TARGETDIR)/LKH/Random.o \
		$(TARGETDIR)/LKH/ReadCandidates.o \
		$(TARGETDIR)/LKH/ReadEdges.o \
		$(TARGETDIR)/LKH/ReadLine.o \
		$(TARGETDIR)/LKH/ReadParameters.o \
		$(TARGETDIR)/LKH/ReadPenalties.o \
		$(TARGETDIR)/LKH/ReadProblem.o \
		$(TARGETDIR)/LKH/RecordBestTour.o \
		$(TARGETDIR)/LKH/RecordBetterTour.o \
		$(TARGETDIR)/LKH/RemoveFirstActive.o \
		$(TARGETDIR)/LKH/ResetCandidateSet.o \
		$(TARGETDIR)/LKH/RestoreTour.o \
		$(TARGETDIR)/LKH/SegmentSize.o \
		$(TARGETDIR)/LKH/Sequence.o \
		$(TARGETDIR)/LKH/SFCTour.o \
		$(TARGETDIR)/LKH/SolveCompressedSubproblem.o \
		$(TARGETDIR)/LKH/SolveDelaunaySubproblems.o \
		$(TARGETDIR)/LKH/SolveKarpSubproblems.o \
		$(TARGETDIR)/LKH/SolveKCenterSubproblems.o \
		$(TARGETDIR)/LKH/SolveKMeansSubproblems.o \
		$(TARGETDIR)/LKH/SolveRoheSubproblems.o \
		$(TARGETDIR)/LKH/SolveSFCSubproblems.o \
		$(TARGETDIR)/LKH/SolveSubproblem.o \
		$(TARGETDIR)/LKH/SolveSubproblemBorderProblems.o \
		$(TARGETDIR)/LKH/SolveTourSegmentSubproblems.o \
		$(TARGETDIR)/LKH/Statistics.o \
		$(TARGETDIR)/LKH/StoreTour.o \
		$(TARGETDIR)/LKH/SymmetrizeCandidateSet.o \
		$(TARGETDIR)/LKH/TrimCandidateSet.o \
		$(TARGETDIR)/LKH/WriteCandidates.o \
		$(TARGETDIR)/LKH/WritePenalties.o \
		$(TARGETDIR)/LKH/WriteTour.o

$(TARGETDIR)/cost-estimation: $(OBJS2)
	$(CCC)  $(CCFLAGS) -o $(TARGETDIR)/cost-estimation $(OBJS2)
	
$(TARGETDIR)/Block.o: Block.h Block.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Block.cpp -o $(TARGETDIR)/Block.o

$(TARGETDIR)/District.o: District.h District.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c District.cpp -o $(TARGETDIR)/District.o

$(TARGETDIR)/Point.o: Point.h Point.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Point.cpp -o $(TARGETDIR)/Point.o
	
$(TARGETDIR)/main.o: main.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c main.cpp -o $(TARGETDIR)/main.o

$(TARGETDIR)/Params.o: Params.h Params.cpp
	$(CCC) $(CCFLAGS) $(CPPFLAGS) -c Params.cpp -o $(TARGETDIR)/Params.o

clean:
	 $(RM) \
     $(TARGETDIR)/main.o \
	 $(TARGETDIR)/Block.o \
	 $(TARGETDIR)/District.o \
     $(TARGETDIR)/Point.o \
	 $(TARGETDIR)/LKH/*.o \
     $(TARGETDIR)/Params.o 

