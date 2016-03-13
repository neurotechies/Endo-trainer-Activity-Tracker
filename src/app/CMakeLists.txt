
link_directories(${OpenCV_LIB_DIR})

include_directories(.
    ../libopentld/imacq
    ../libopentld/mftracker
    ../libopentld/tld
    ../3rdparty/cvblobs
    ${OpenCV_INCLUDE_DIRS})


include_directories(../3rdparty/libconfig)


#-------------------------------------------------------------------------------
# main
add_library(app
    Config.cpp
    Gui.cpp
    Main.cpp
    Settings.cpp
	Trajectory.cpp
    Config.h
    Gui.h
    Main.h
    Settings.h
	Trajectory.h
	ContourFeature.hpp
	ContourFeature.cpp
	pegBox.h
	pegBox.cpp
	RingBox.h
	RingBox.cpp
	Scorer.h
	Scorer.cpp
	util.h
	util.cpp
	)

target_link_libraries(app libopentld cvblobs config++ ${OpenCV_LIBS})

#-------------------------------------------------------------------------------
# opentld
add_executable(Neuro-Endo-Trainer-Activity-Tracker
    Neuro-Endo-Trainer-Activity-Tracker.cpp)

target_link_libraries(Neuro-Endo-Trainer-Activity-Tracker app libopentld cvblobs config++ ${OpenCV_LIBS})

install(TARGETS Neuro-Endo-Trainer-Activity-Tracker DESTINATION bin)



