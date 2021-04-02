#!/bin/bash

# Try to find a portable way of getting rid of
# any stray carriage returns
if which dos2unix ; then
    DOS2UNIX="dos2unix"
elif which fromdos ; then
    DOS2UNIX="fromdos"
else
    # This works on a GNU version of sed. I think this
    # will work in OSX as well, but don't have a machine
    # on which to test that. From reading the OSX docs,
    # it looks compatible.
    # The code \x0D is the ASCII code of carriage-return,
    # so it the regex should delete any CRs at the end of
    # a line (or anywhere in a line)
    DOS2UNIX="cat"
    # Tested for combinations of:
    # - Ubuntu
    # - Cygwin
    # and inputs formats:
    # - LF
    # - CRLF
fi

echo "========================================"
echo " Cleaning the temporaries and outputs"
make clean
rm -f executables/*
rm -f compiler_tests/*/*.o
rm -f compiler_tests/*/*.s
rm -f compiler_tests/*/*.cref

echo "========================================"
echo " Force building compiler"
make bin/c_compiler
if [[ "$?" -ne "0" ]]; then
    echo "Error while building compiler."
    exit 1;
fi

mkdir -p executables
PASSED=0
CHECKED=0

TESTFOLDERS="compiler_tests/*"

for i in ${TESTFOLDERS} ; do
    FOLDERNAME=$(basename ${i})
    echo "############################################"
    echo ${FOLDERNAME}
    echo "############################################"

    TESTCASES="compiler_tests/${FOLDERNAME}/*_driver.c"

    for j in ${TESTCASES} ; do
        TESTNAME=$(basename ${j} _driver.c)
        echo "========================================="
        echo "Input file : ${TESTNAME}.c"
        echo "Running compiler"
        bin/c_compiler -S compiler_tests/${FOLDERNAME}/${TESTNAME}.c -o compiler_tests/${FOLDERNAME}/${TESTNAME}.s
        if [[ "$?" -ne "0" ]]; then
            echo "Error while running compiler."
            echo "FAIL"
            echo "========================================="
            CHECKED=$(( ${CHECKED}+1 ));
            continue
        fi

        echo "Assembling generated MIPS"
        mips-linux-gnu-gcc -mfp32 -o compiler_tests/${FOLDERNAME}/${TESTNAME}.o -c compiler_tests/${FOLDERNAME}/${TESTNAME}.s
        if [[ "$?" -ne "0" ]]; then
            echo "Error while assembling the generated MIPS."
            echo "FAIL"
            echo "========================================="
            CHECKED=$(( ${CHECKED}+1 ));
            continue
        fi

        echo "Linking to the driver file and creating executable"
        mips-linux-gnu-gcc -mfp32 -static -o  executables/${TESTNAME} compiler_tests/${FOLDERNAME}/${TESTNAME}.o compiler_tests/${FOLDERNAME}/${TESTNAME}_driver.c
        if [[ "$?" -ne "0" ]]; then
            echo "Error while linking to driver file."
            echo "FAIL"
            echo "========================================="
            CHECKED=$(( ${CHECKED}+1 ));
            continue
        fi

        echo "Simulating the executable on MIPS"
        qemu-mips executables/${TESTNAME}
        ret=$?
        if [[ $ret -eq 0 ]] ; then
            echo "PASS"
            PASSED=$(( ${PASSED}+1 ));
        else
            echo "FAIL"
        fi
        echo "========================================="
        CHECKED=$(( ${CHECKED}+1 ));
    done
done

echo "########################################"
echo "Passed ${PASSED} out of ${CHECKED}".
echo "########################################"
