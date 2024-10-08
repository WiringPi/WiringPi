# CMakeLists.txt for pins
project(pins LANGUAGES NONE)

find_program (PDFLATEX pdflatex)

if(NOT PDFLATEX)
    message(WARNING "Disabling pins target as pdflatex not found!")
else()
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pins.pdf
        COMMAND ${PDFLATEX} -interaction=nonstopmode -halt-on-error ${CMAKE_CURRENT_SOURCE_DIR}/pins.tex
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating pins.pdf"
    )

    # Add custom target to generate PDF
    add_custom_target(pins
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pins.pdf
    )

    # Install PDF
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pins.pdf DESTINATION share/pins)
endif()
