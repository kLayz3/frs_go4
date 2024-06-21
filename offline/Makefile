ifndef GO4SYS
$(error GO4SYS should be defined)
endif
OURWD:= ..

INCLUDES:= $(addprefix -I, $(shell root-config --incdir)\
$(GO4SYS)/include ../src/)

CPPFLAGS := -Wall -MMD -MP $(INCLUDES)
CXXFLAGS := -O0 -g
LIBDIRS:=  $(addprefix -L,$(shell root-config --libdir) $(OURWD))


all: conv1


conv1: conv1.o
	$(CXX) -o $@ $^ -lCore -lImt -lRIO -lNet  -lROOTVecOps -lTree -lTreePlayer -lRint -lMathCore   -lGo4UserAnalysis $(LIBDIRS)

clean :
	$(RM) *.o

.PHONY: all clean
