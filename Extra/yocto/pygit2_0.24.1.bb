inherit pypi setuptools
SUMMARY = "Pygit2 is a set of Python bindings to the libgit2 shared library"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=6f00d4a50713fa859858dd9abaa35b21"

SRC_URI[md5sum] = "dd98b6a9fded731e36ca5a40484c8545"
SRC_URI[sha256sum] = "4d1d0196b38d6012faf0a7c45e235c208315672b6035da504566c605ba494064"

do_compile_append() {
    ${PYTHON} setup.py -q bdist_egg --dist-dir ./
}
do_install_append() {
    install -m 0644 ${S}/*.egg ${D}/${PYTHON_SITEPACKAGES_DIR}/
}

DEPENDS_${PN} += "libgit2-native_0.24.1 "

BBCLASSEXTEND = "native nativesdk"


