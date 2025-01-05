# CMakeLists.txt for pins
project(pins LANGUAGES NONE)

# Source file
set(SRC pins.tex)

# Add custom target to generate PDF
add_custom_target(pins_pdf
    COMMAND latex ${SRC}
    COMMAND dvipdf ${SRC:.tex=.dvi}
    COMMENT "Generating PDF"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add clean target
add_custom_target(clean_pins
    COMMAND rm -f *.dvi *.aux *.log *.ps *.toc *.bak *~
    COMMENT "Cleaning pins"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add dependencies
add_dependencies(pins_pdf clean_pins)

# Install PDF
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pins.pdf DESTINATION share/pins)
