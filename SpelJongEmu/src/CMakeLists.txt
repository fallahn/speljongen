#add source files used by your project here
set(PROJECT_SRC 
  ${PROJECT_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/AluFunctions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Argument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cpu.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/InterruptManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/OpCode.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/OpCodeBuilder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/OpCodesImpl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SpeedMode.cpp
  PARENT_SCOPE)