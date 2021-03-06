function(toggle_static_analyzers target_name)
  option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
  option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
  option(ENABLE_IWYU "Enable static analysis with include-what-you-use" OFF)
  
  if(ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
      message(STATUS "Using cppcheck for ${target_name} target")
      set_target_properties(
        ${target_name} PROPERTIES
        CXX_CPPCHECK ${CPPCHECK} --suppress=missingInclude
                                 --enable=all
                                 --inconclusive)
    else()
      message(SEND_ERROR "cppcheck requested but executable not found")
    endif()
  endif()

  if(ENABLE_CLANG_TIDY)
    find_program(CLANGTIDY clang-tidy)
    if(CLANGTIDY)
       message(STATUS "Using clang-tidy for ${target_name} target")
       set_target_properties(${target_name} PROPERTIES CXX_CLANG_TIDY ${CLANGTIDY})
    else()
       message(SEND_ERROR "clang-tidy requested but executable not found")
    endif()
  endif()

  if(ENABLE_IWYU)
    find_program(IWYU NAMES include-what-you-use)
    if(IWYU)
      message(STATUS "Using include-what-you-use for ${target_name} target")
      set_target_properties(${target_name} PROPERTIES CXX_INCLUDE_WHAT_YOU_USE ${IWYU})
    else()
      message(SEND_ERROR "include-what-you-use requested but executable not found")
    endif()
  endif()
endfunction()