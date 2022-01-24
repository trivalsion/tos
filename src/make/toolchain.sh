#! /bin/bash


## Variables ##
script_start_time=$(date +%s)

packages="nasm binutils diffutils valgrind clang lld llvm gcc qemu-system-x86 gnu-efi mtools"
gcc_build_version="9.3.0"
binutils_build_version="2.30"
toolchain_prefix=$(realpath ../toolchain/)
make_folder=$(realpath ./make)

mkdir -p ../toolchain/src   # toolchain_src - Can't be decalred without making folder
toolchain_src=$(realpath ../toolchain/src)

target_x86_32=i686-elf
target_x86_64=x86_64-elf

build_ovmf=n
build_gnu_tools=no
build_gnuefi=n


## Installation Config ##
read -r -p "Package Manager(dnf, apt, macos, other): " package_manager
read -r -p "Do you want to compile the EDK2 tools(y/n): " build_option_edk2
read -r -p "Do you want to configure other options(y/n): " option_extra

# Extra configuration of the build
if [ "$option_extra" == y ]
  then
    # GCC Toolchain Build option
    read -r -p "Should the x86_32/x86_64 cross-compiler be compiled(no/all/64/32): " build_gnu_tools

    # OVMF Build option
    read -r -p "Should the UEFI be compiled(y/n): " build_ovmf

    # GNU-EFI Build option
    read -r -p "Should the GNU-EFI be compiled(y/n): " build_gnuefi

    # Configure the build directory
    read -r -p "Do you want to customize the build path(y/n): " option_prefix
    if [ "$option_prefix" == y ]
      then
        read -r -p "Enter a new build directory path: " toolchain_prefix
    fi
fi


## Package Installation ##
# DNF
func_pm_dnf() {
    # Install developer tools and headers
    sudo dnf -y install $packages @development-tools kernel-headers kernel-devel edk2-ovmf

    # Install x86/x86_64 Cross-compiler build dependencies
    if [ "$build_gnu_tools" != no ]
      then
        sudo dnf -y install gcc gcc-c++ make bison flex gmp-devel libmpc-devel mpfr-devel texinfo automake autoconf xorriso @development-tools
    fi

    # Install OVMF UEFI Dependencies
    if [ "$build_ovmf" == y ]
      then
        sudo dnf -y install @development-tools gcc-c++ iasl libuuid-devel nasm edk2-tools-python
    fi
}

# APT
func_pm_apt() {
    # Install developer tools and headers
    sudo apt -y install $packages build-essential linux-headers-$(uname -r) ovmf

    # Install x86/x86_64 Cross-compiler build dependencies
    if [ "$build_gnu_tools" != no ]
      then
        sudo apt -y install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
    fi

    # Install OVMF UEFI build dependencies
    if [ "$build_ovmf" == y ]
      then
        sudo apt -y install build-essential uuid-dev iasl nasm python3-distutils
    fi
}

# MacOS
func_pm_macos() {
    # Packages
    brew install gdb nasm binutils diffutils coreutils

    # Dependencies
    brew install gmp mpfr libmpc libiconv
    brew install libiconv
    echo "There maybe problems during the compilation proccess due to wrong attributes, then remove -with-sysroot and add --enable-interwork to both gcc and binutils"
}

# Other
func_pm_other() {
    echo "If you are here, your package manager is probably not in the list, so you need to make sure all of the libs are installed before preceding, here is the list: "
    echo "$packages"

    # GCC cross-compiler dependencies
    if [ "$build_gnu_tools" != no ]
      then
        echo "build-essentail(platform specific package), linux-headers, ovmf, bison, flex, gmp, libmpc, libmpfr, texinfo, xorriso, autoconf, automake"
    fi

    # OVMF dependencies
    if [ "$build_ovmf" == y ]
      then
        echo "build-essential uuid-dev iasl nasm python3-distutils"
    fi

    read -r -p "Press any button to continue, if all libs are installed"
}


## Compilation Setup ##
# Compilation Setup for GCC
func_setup_gcc() {
    local arch=$1

    # Create the nesecerry direcotries
    mkdir -p "$toolchain_src"/binutils"$binutils_build_version"_"$arch"/build/
    mkdir -p "$toolchain_src"/gcc"$gcc_build_version"_"$arch"/build/
    mkdir -p "$toolchain_prefix"

    # Download and unpack source code
    cd "$toolchain_src"/binutils"$binutils_build_version"_"$arch"/ || exit
    wget https://ftp.gnu.org/gnu/binutils/binutils-"$binutils_build_version".tar.gz
    tar -xzf binutils-"$binutils_build_version".tar.gz
    rm binutils-"$binutils_build_version".tar.gz

    cd "$toolchain_src"/gcc"$gcc_build_version"_"$arch"/ || exit
    wget https://ftp.gnu.org/gnu/gcc/gcc-"$gcc_build_version"/gcc-"$gcc_build_version".tar.gz
    tar -xzf gcc-"$gcc_build_version".tar.gz
    rm gcc-"$gcc_build_version".tar.gz
}

# Compilation Setup for EDK2
func_setup_edk2() {
    # Create the necessery directories
    mkdir -p "$toolchain_prefix"/edk2/

    # Download the source code
    git clone https://github.com/tianocore/edk2 "$toolchain_prefix"/edk2/
    cd "$toolchain_prefix"/edk2/ || exit
    git submodule update --init
}


## Build proccess ##
# Compile the OVMF UEFI
func_build_ovmf() {
    cd "$toolchain_prefix"/edk2/ || exit
    make -C BaseTools
    . edksetup.sh
    echo "
    ACTIVE_PLATFORM=OvmfPkg/OvmfPkgX64.dsc
    TARGET = DEBUG
    TARGET_ARCH = X64
    TOOL_CHAIN_CONF = Conf/tools_def.txt
    TOOL_CHAIN_TAG = GCC5
    BUILD_RULE_CONF = Conf/build_rule.txt" > Conf/target.txt
    build
}

# Compile EDK2 Tools
func_build_edk2_tools() {
    cd "$toolchain_prefix"/edk2/ || exit
    make -C BaseTools
    . edksetup.sh
}

# If kernel headers installation on debian not work check "ls -l /usr/src/linux-headers-$(uname -r)"(if does not exist then there are no headers), insetad try to find the latest version if not installed
# Setup for GNU-EFI toolkit compilation
func_build_gnuefi() {
    mkdir -p "$toolchain_prefix"/gnu-efi/
    git clone https://git.code.sf.net/p/gnu-efi/code "$toolchain_prefix"/gnu-efi/
    cd "$toolchain_prefix"/gnu-efi/ || exit
    make
}

# Setup for compiling the x86_32 compiler
func_build_gcc_linux() {
  if [[ $build_gnu_tools == 32 || $build_gnu_tools == all ]]
    then
      func_setup_gcc "i686"

      # Build binutils
      cd "$toolchain_src"/binutils"$binutils_build_version"_i686/build/ || exit
      ../binutils-"$binutils_build_version"/configure --target=$target_x86_32 --prefix="$toolchain_prefix" --with-sysroot --disable-nls --disable-werror
      make
      make install

      # Build GCC
      cd "$toolchain_src"/gcc"$gcc_build_version"_i686/build/ || exit
      which -- $target_x86_32-as || echo $target_x86_32-as is not in the PATH
      ../gcc-"$gcc_build_version"/configure --target=$target_x86_32 --prefix="$toolchain_prefix" --disable-nls --enable-language=c,c++ --without-headers
      make all-gcc
      make all-target-libgcc
      make install-gcc
      make install-target-libgcc
  fi

  # Setup for compiling the x86_64 compiler
  if [[ $build_gnu_tools == 64 || $build_gnu_tools == all ]]
    then
      func_setup_gcc "amd64"

      echo "MULTILIB_OPTIONS += mno-red-zone
  MULTILIB_DIRNAMES += no-red-zone" > gcc-"$gcc_build_version"/gcc/config/i386/t-x86_64-elf
      cat "$make_folder"/config.gcc > gcc-"$gcc_build_version"/gcc/config.gcc

      # Build binutils
      cd "$toolchain_src"/binutils"$binutils_build_version"_amd64/build/ || exit
      ../binutils-"$binutils_build_version"/configure --target=$target_x86_64 --prefix="$toolchain_prefix" --with-sysroot --disable-nls --disable-werror
      make
      make install

      # Build GCC
      cd "$toolchain_src"/gcc"$gcc_build_version"_amd64/build/ || exit
      which -- $target_x86_64-as || echo $target_x86_64-as is not in the PATH
      ../gcc-"$gcc_build_version"/configure --target=$target_x86_64 --prefix="$toolchain_prefix" --disable-nls --enable-language=c,c++ --without-headers
      make all-gcc
      make all-target-libgcc
      make install-gcc
      make install-target-libgcc
  fi
}

# Seperate build instructions if the MacOS is used
func_build_gcc_macos() {
    # Compile x86_32 gcc cross-compiler
    if [[ $build_gnu_tools == 32 || $build_gnu_tools == all ]]
      then
        func_setup_gcc "i686"

        # Building binutils
        cd "$toolchain_src"/binutils"$binutils_build_version"_i686/build || exit
        ../binutils-"$binutils_build_version"/configure --target=$target_x86_32 --prefix="$toolchain_prefix" --with-sysroot --disable-nls --disable-werror --enable-multilib --with-libiconv-prefix=/usr/local/opt/libiconv/
        make
        make install

        # Building GCC
        cd "$toolchain_src"/gcc"$gcc_build_version"_i686/build || exit
        which -- $target_x86_32-as || echo $target_x86_32-as is not in the PATH
        ../gcc-"$gcc_build_version"/configure --target=$target_x86_32 --prefix="$toolchain_prefix" --disable-nls --enable-language=c,c++ --without-headers --enable-multilib --with-libiconv-prefix=/usr/local/opt/libiconv/
        make all-gcc
        make all-target-libgcc
        make install-gcc
        make install-target-libgcc
    fi

    # Compile the x86_64 gcc cross-compiler
    if [[ $build_gnu_tools == 64 || $build_gnu_tools == all ]]
      then
        func_setup_gcc "amd64"

        # Building binutils
        cd "$toolchain_src"/binutils"$binutils_build_version"_amd64/build/ || exit
        ../binutils-"$binutils_build_version"/configure --target=$target_x86_64 --prefix="$toolchain_prefix" --with-sysroot --disable-nls --disable-werror --enable-multilib --with-libiconv-prefix=/usr/local/opt/libiconv/
        make
        make install

        # Building GCC
        cd "$toolchain_src"/gcc"$gcc_build_version"_amd64/build/ || exit
        which -- $target_x86_64-as || echo $target_x86_64-as is not in the PATH
        ../gcc-"$gcc_build_version"/configure --target=$target_x86_64 --prefix="$toolchain_prefix" --disable-nls --enable-language=c,c++ --without-headers --enable-multilib --with-libiconv-prefix=/usr/local/opt/libiconv/
        make all-gcc
        make all-target-libgcc
        make install-gcc
        make install-target-libgcc
    fi
}


## Installation check ##
func_installation_check() {
  if [ "$build_ovmf" == y ]
    then
      echo "
  ----- OVMF -----"
      ls "$toolchain_prefix"/edk2/Build
  fi

  if [ "$build_gnuefi" == y ]
    then
      echo "
  ----- GNU-EFI Toolkit -----"
    ls "$toolchain_prefix"/gnu-efi/x86_64
  fi

  if [ "$build_option_edk2" == y ]
    then
      echo "
  ----- EDK2 Build Tools -----"
      ls "$toolchain_prefix"/edk2/
  fi

  # Extra
  if [[ $build_gnu_tools == 64 || $build_gnu_tools == all ]]
    then
      echo "
  --- GCC Toolchain 64 bit ---"
      "$toolchain_prefix"/bin/x86_64-elf-gcc --version
      find "$toolchain_prefix"/lib -name 'libgcc.a'
  fi
  if [[ $build_gnu_tools == 32 || $build_gnu_tools == all ]]
    then
      echo "
  --- GCC Toolchain 32 bit ---"
      "$toolchain_prefix"/bin/i686-elf-gcc --version
  fi
}


## Start ##
# Package Management
if [ "$package_manager" == dnf ]
  then
    func_pm_dnf
  elif [ "$package_manager" == apt ]
  then
    func_pm_apt
  elif [ "$package_manager" == macos ]
  then
    func_pm_macos
  else
    func_pm_other
fi

# Build Setup
if [[ $build_ovmf == y || $build_option_edk2 == y ]]
  then
    func_setup_edk2
fi

# Build
if [ "$build_ovmf" == y ]
  then
    func_build_ovmf
fi
if [[ $build_option_edk2 == y && $build_ovmf != y ]]
  then
    func_build_edk2_tools
fi
if [ "$build_gnuefi" == y ]
  then
    func_build_gnuefi
fi
if [ "$build_gnu_tools" != no ]
  then
    if [ "$package_manager" != macos ]
      then
        func_build_gcc_linux
      elif [ "$package_manager" == macos ]
      then
        func_build_gcc_macos
    fi
fi

func_installation_check


# Time taken output
echo "
----- Execution Time -----"
script_end_time=$(date +%s)
script_execution_time="$((script_end_time-script_start_time))"
echo "Script took
Seconds: $script_execution_time
Minutes: $((script_execution_time / 60))"