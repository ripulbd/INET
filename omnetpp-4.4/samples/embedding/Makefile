#
# OMNeT++/OMNEST Makefile for embedding
#
# This file was generated with the command:
#  opp_makemake -f --deep --nolink
#

# C++ include paths (with -I)
INCLUDE_PATH = -I. -Imodel

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = $O/main.o $O/model/AbstractFifo.o $O/model/Fifo.o $O/model/Sink.o $O/model/Source.o

# Message files
MSGFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# inserted from file 'makefrag':
#
# Customize Makefile to link an executable with the simulation kernel,
# but without the user interface libs (oppenvir, oppcmdenv, opptkenv)
#

TARGET = embedding$(EXE_SUFFIX)

OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" $(KERNEL_LIBS) $(SYS_LIBS)

all: $(TARGET)

$(TARGET) : $O/$(TARGET)
	$(Q)ln -s -f $O/$(TARGET) .

$O/$(TARGET): $(OBJS) Makefile
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET) $(OBJS) $(OMNETPP_LIBS)

# <<<
#------------------------------------------------------------------------------

# Main target

all: $(OBJS)  Makefile
	@# Do nothing

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f embedding embedding.exe libembedding.so libembedding.a libembedding.dll libembedding.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h
	$(Q)-rm -f model/*_m.cc model/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc model/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/main.o: main.cc
$O/model/AbstractFifo.o: model/AbstractFifo.cc \
  model/AbstractFifo.h
$O/model/Fifo.o: model/Fifo.cc \
  model/Fifo.h \
  model/AbstractFifo.h
$O/model/Sink.o: model/Sink.cc
$O/model/Source.o: model/Source.cc
