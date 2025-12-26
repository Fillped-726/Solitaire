cmake_minimum_required(VERSION 3.6)

set(APP_NAME Solitaire)

project(${APP_NAME})

# ==============================================================================
# [关键修复 1] 强制降级 C++ 标准并解决 Windows SDK 冲突
# ==============================================================================
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================================
# [修复] 强制查找 Python 环境，解决资源同步脚本报错
# ========================================================= 
# =========================================================
# [修复] 强制指定 Python 绝对路径 (无条件执行)
# =========================================================
set(PYTHON_COMMAND "C:/Users/36064/.conda/envs/cocos_env/python.exe")
message(STATUS ">>> Force using Python: ${PYTHON_COMMAND}") 

if(MSVC)
    # 允许旧的 C 函数 (strcpy 等)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    
    # [新增] 解决 VS2022/2026 中 std::byte 与 Windows.h byte 的歧义
    add_definitions(-D_HAS_STD_BYTE=0)
    
    # [新增] 屏蔽部分 VS 新版过严的警告，防止把警告当错误中断编译
    add_compile_options(/wd4996)
endif()

# ==============================================================================
# Cocos 引擎加载
# ==============================================================================

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

if(NOT DEFINED BUILD_ENGINE_DONE)
    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# ==============================================================================
# [关键修复 2] 自动化源代码扫描 (告别手动添加文件的痛苦)
# ==============================================================================
# 原来的写法需要你每新建一个类都来改这里。
# 现在改为自动扫描 Classes 目录下所有的 .cpp 和 .h 文件。

# 扫描 Classes 目录
file(GLOB_RECURSE GAME_SOURCE "Classes/*.cpp")
file(GLOB_RECURSE GAME_HEADER "Classes/*.h")

# 设置资源目录
set(GAME_RES_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# ==============================================================================
# 平台特定配置
# ==============================================================================

if(ANDROID)
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE proj.android/app/jni/hellocpp/main.cpp)
elseif(LINUX)
    list(APPEND GAME_SOURCE proj.linux/main.cpp)
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# ==============================================================================
# 目标构建与链接
# ==============================================================================

set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )

if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files} "Classes/configs/LevelConfig.h" "Classes/configs/LevelConfigLoader.h" "Classes/configs/LevelConfigLoader.cpp" "Classes/models/GameConstants.h" "Classes/models/CardModel.h" "Classes/models/CardModel.cpp" "Classes/models/GameModel.h" "Classes/models/GameModel.cpp" "Classes/configs/CardResConfig.h" "Classes/views/CardView.h" "Classes/views/CardView.cpp" "Classes/views/GameView.h" "Classes/views/GameView.cpp" "Classes/services/GameRuleService.h" "Classes/managers/ICommand.h" "Classes/services/GameModelGenerator.h" "Classes/services/GameModelGenerator.cpp" "Classes/controllers/commands/DrawCardCommand.h" "Classes/controllers/commands/DrawCardCommand.cpp")
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

target_link_libraries(${APP_NAME} cocos2d)

# 包含目录设置
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

# 资源拷贝与 IDE 设置
setup_cocos_app_config(${APP_NAME})

if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    if(MACOSX)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()