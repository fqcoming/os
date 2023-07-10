
# 设置项目的名称为 "shell"，并指定项目使用的语言为C。
project(shell LANGUAGES C)  

# 使用自定义的链接器
# 加入相应的库
set(LIBS_FLAGS "-L ${CMAKE_BINARY_DIR}/source/applib/ -lapp -L ${CMAKE_BINARY_DIR}/../../newlib/i686-elf/lib -lm -lc")
# 设置一个变量 LIBS_FLAGS，该变量包含了链接时所需的库路径和库名。
# ${CMAKE_BINARY_DIR} 是生成文件的目录，${PROJECT_SOURCE_DIR} 是源代码目录。
# 这里通过设置 -L 选项指定了库的路径，-l 选项指定了库的名称。



# -m elf_i386 表示生成elf_i386格式的elf文件，生成的可执行文件适用于32位x86架构。
# -T ${PROJECT_SOURCE_DIR}/link.lds 指定链接文件，该脚本定义了链接器如何组织和布局可执行文件的各个部分。
# ${LIBS_FLAGS} 包含其他的库标志，可能是其他变量或具体的标志选项。${LIBS_FLAGS}在上面定义过
set(CMAKE_EXE_LINKER_FLAGS "-m elf_i386 -T ${PROJECT_SOURCE_DIR}/link.lds ${LIBS_FLAGS}")
set(CMAKE_C_LINK_EXECUTABLE "${LINKER_TOOL} <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} -o ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf")

include_directories(
    ${PROJECT_SOURCE_DIR}/../applib/
)

# 将所有的汇编、C文件加入工程
# 注意保证start.asm在最前头
file(GLOB C_LIST "*.c" "*.h" "*.S")
add_executable(${PROJECT_NAME} ${C_LIST})

# 不带调试信息的elf生成，何种更小，写入到image目录下
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${OBJCOPY_TOOL} -S ${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/../../image/${PROJECT_NAME}.elf
                   COMMAND ${OBJDUMP_TOOL} -x -d -S -m i386 ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_dis.txt
                   COMMAND ${READELF_TOOL} -a ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_elf.txt
)