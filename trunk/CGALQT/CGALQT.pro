
HEADERS=./config.h \
	./color.h \
	./fallson.h \
	./QT/mainwindow.h \
    ./QT/glmdichild.h \
	./QT/filepropertydialog.h \
	./CGAL/builder.h \
	./CGAL/enriched_polyhedron.h \
	./CGAL/parser_obj.h \
	./CGAL/sqrt3.h \
	./CGAL/quad-triangle.h \
	./CGAL/enriched_polygon.h \
	./CGAL/quad-simp.h \
	./Util/uglyfont.h \
	./Util/stringutils.h \
				
SOURCES =./QT/main.cpp \
         ./QT/mainwindow.cpp \
         ./QT/glmdichild.cpp \
         ./QT/filepropertydialog.cpp \
         ./Util/uglyfont.cpp \
				
RESOURCES     = ./QT/mdi.qrc
RC_FILE       = CGALQT.rc

QT           += opengl

INCLUDEPATH += . ./QT ./CGAL ./Util $(BOOSTROOT) $(CGALROOT)/include $(CGALROOT)/auxiliary/taucs/include

LIBS += -L$(CGALROOT)/lib -L$(CGALROOT)/auxiliary/taucs/lib -L$(BOOSTROOT)/stage/lib

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX _SECURE_SCL=0 _CRT_SECURE_NO_DEPRECATE _SCL_SECURE_NO_DEPRECATE

CONFIG += stl
