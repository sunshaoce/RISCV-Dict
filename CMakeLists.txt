cmake_minimum_required(VERSION 3.16)

project(riscv-dict)

include_directories(include)

add_executable(RISCV-Dict
  src/RISCV-Dict.cpp
  src/Instruction.cpp
)
