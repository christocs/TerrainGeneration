target_sources(${PROJECT_NAME} PRIVATE
    Afk.cpp
)

add_subdirectory(component)
add_subdirectory(io)
add_subdirectory(physics)
add_subdirectory(renderer)
add_subdirectory(event)
add_subdirectory(script)
add_subdirectory(debug)
add_subdirectory(ui)
