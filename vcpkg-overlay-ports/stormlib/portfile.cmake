message("using vcpkg overlay port for stormlib")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ladislav-zezula/StormLib
    REF 539a04e06578ce9b0cf005446eff66e18753076d
    SHA512 4d1a1c8eb559e7b685a3e7679bef2440af5bbed6c93157b6b6f2acce47d7249ef12643f4cd4c7ec341ea40b07e5109bdc5f99b34c9a536fa5d878430973af867
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