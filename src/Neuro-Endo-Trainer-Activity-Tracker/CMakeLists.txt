
link_directories(${OpenCV_LIB_DIR})

include_directories(main
	../libopentld/imacq
	../libopentld/mftracker
    ../libopentld/tld
    ../3rdparty/cvblobs
    ${OpenCV_INCLUDE_DIRS})


#-------------------------------------------------------------------------------
# main
add_library(main
    main/ringBox.cpp
	main/pegBox.cpp
	main/ContourFeature.cpp
    main/Gui.cpp
    main/Main.cpp
	main/util.cpp
	main/Trajectory.cpp
	main/Scorer.cpp
	
	main/Scorer.h
	main/ringBox.h
	main/params.h
    main/pegBox.h
	main/ContourFeature.hpp
    main/Gui.h
    main/Main.h
	main/util.h
	main/Trajectory.h)

target_link_libraries(main libopentld cvblobs ${OpenCV_LIBS})


add_executable(Neuro-Endo-Trainer-Activity-Tracker
    Neuro-Endo-Trainer-Activity-Tracker.cpp)

target_link_libraries(Neuro-Endo-Trainer-Activity-Tracker main libopentld cvblobs ${OpenCV_LIBS})

install(TARGETS Neuro-Endo-Trainer-Activity-Tracker DESTINATION bin)

