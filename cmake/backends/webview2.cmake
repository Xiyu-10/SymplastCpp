function(symplast_link_webview2 TARGET_NAME WEBVIEW2_VERSION)
    if(NOT WIN32)
        return()
    endif()
    if(NOT MSWEBVIEW2_ROOT)
        include(FetchContent)

        cmake_policy(SET CMP0135 NEW)

        FetchContent_Declare(
            microsoft_web_webview2
            URL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${WEBVIEW2_VERSION}"
        )
        FetchContent_MakeAvailable(microsoft_web_webview2)

        set(MSWEBVIEW2_ROOT "${microsoft_web_webview2_SOURCE_DIR}" PARENT_SCOPE)
        set(MSWEBVIEW2_ROOT "${microsoft_web_webview2_SOURCE_DIR}")
    endif()

    target_include_directories(${TARGET_NAME} PRIVATE
        "${MSWEBVIEW2_ROOT}/build/native/include"
    )

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WEBVIEW2_ARCH "x64")
    else()
        set(WEBVIEW2_ARCH "x86")
    endif()

    target_link_libraries(${TARGET_NAME} PRIVATE
        "${MSWEBVIEW2_ROOT}/build/native/${WEBVIEW2_ARCH}/WebView2LoaderStatic.lib"
        shlwapi
        version
    )
endfunction()