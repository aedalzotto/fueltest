BIN := fueltest

CXX := g++
CP  := cp
MK  := mkdir
RM  := rm

SRCDIR := src
SRCEXT := cpp
SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
INCDIR := $(SRCDIR)/include
DATADIR := data
GLADEFILES := $(wildcard $(DATADIR)/*.glade)

BUILDDIR := build
BINDIR := $(BUILDDIR)/bin
SHAREDIR := $(BUILDDIR)/share/$(BIN)

TMPDIR := tmp
OBJECTS := $(addprefix $(TMPDIR)/,$(notdir $(SOURCES:.$(SRCEXT)=.o)))

TARGET := $(BINDIR)/$(BIN)
CXXFLAGS := -g -Wall
LDFLAGS := -ldl -lboost_system -lboost_filesystem
PKGCFG := `pkg-config --cflags --libs gtkmm-3.0`
INCLUDES := -I./$(INCDIR)

CPGLADE := $(addprefix $(SHAREDIR)/,$(notdir $(GLADEFILES)))

all: $(TARGET) $(CPGLADE)

buildrun: all
	./$(TARGET)

run:
	./$(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(MK) -p $(BINDIR)
	$(CXX) $^ -o $(TARGET) $(LDFLAGS) $(PKGCFG) 

$(TMPDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling $<"
	@$(MK) -p $(TMPDIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES) $(PKGCFG)

$(CPGLADE): $(GLADEFILES)
	@echo "Copying glade files..."
	@$(MK) -p $(SHAREDIR)
	@$(CP) $< $@

clean:
	@echo "Cleaning..."
	@$(RM) -rf $(BUILDDIR) $(BINDIR) $(SHAREDIR)

.PHONY: clean