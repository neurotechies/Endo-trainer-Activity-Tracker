
link_directories(${OpenCV_LIB_DIR})

include_directories(main
	../libopentld/imacq
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
	
	main/ringBox.h
    main/pegBox.h
	main/ContourFeature.hpp
    main/Gui.h
    main/Main.h
	main/util.h)

target_link_libraries(main libopentld ${OpenCV_LIBS})

#-------------------------------------------------------------------------------
# opentld
add_executable(Neuro-Endo-Trainer-Activity-Tracker
    Neuro-Endo-Trainer-Activity-Tracker.cpp)

target_link_libraries(Neuro-Endo-Trainer-Activity-Tracker main libopentld ${OpenCV_LIBS})

install(TARGETS Neuro-Endo-Trainer-Activity-Tracker DESTINATION bin)

