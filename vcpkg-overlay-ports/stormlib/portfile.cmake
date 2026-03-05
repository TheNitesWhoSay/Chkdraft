message("using vcpkg overlay port for stormlib")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO TheNitesWhoSay/StormLib
    REF 720941a94c66a4157bd368fdca361d143e9db63d
    SHA512 4efe16f8d09f01ce9bcc947d58bd27b85a23bd434c4db1f97502e16c52db862fabe2e677442495ba61589da01eef68be1f1f2ebf3a302d1c968f5145c7457dd4
    HEAD_REF master
)

set(STORM_UNICODE ON)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DSTORM_UNICODE=${STORM_UNICODE}
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME StormLib)
vcpkg_copy_pdbs()


file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")