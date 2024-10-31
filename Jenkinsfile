pipeline {
    agent { label 'jenkinsfile' }

    stages {
        stage('Preamble') {
            parallel {
                stage('x86_64 linux clang-18') {
            	     agent { label 'x86_64 && ubuntu24 && cuda' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "x86_64 clang-18 on" `hostname`
			 export CC=clang-18
			 export CXX=clang++-18
			 export CUDACXX=/opt/cuda-12.6/bin/nvcc
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_CUDA_TEST=True -DENABLE_LTO=True ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('x86_64 linux gcc-13') {
            	     agent { label 'x86_64 && ubuntu24' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "x86_64 gcc-13 on" `hostname`
			 export CC=gcc-13
			 export CXX=g++-13
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_ASAN=True ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('cross-s390x linux gcc') {
            	     agent { label 'x86_64 && ubuntu24 && cuda' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "cross-x390x on" `hostname`
			 mkdir build
			 cd build
			 cmake -GNinja .. -DCMAKE_TOOLCHAIN_FILE=../toolchains/s390x-gcc.cmake
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('cross-ppc64el linux clang') {
            	     agent { label 'x86_64 && ubuntu24 && cuda' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "cross-ppc64el on" `hostname`
			 mkdir build
			 cd build
			 cmake -GNinja .. -DCMAKE_TOOLCHAIN_FILE=../toolchains/ppc64el-llvm.cmake
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('x86_64 linux emscripten') {
            	     agent { label 'x86_64 && ubuntu24 && emscripten' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''#!/bin/bash
			 set -e
                	 echo "emscripten on" `hostname`
			 ~/opt/emsdk/emsdk install latest
			 ~/opt/emsdk/emsdk activate latest
			 source ~/opt/emsdk/emsdk_env.sh
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DSET_COMPILER_SUPPORTS_INT128=False -DSET_COMPILER_SUPPORTS_FLOAT128=False -DSET_LONGDOUBLE_IS_FLOAT128=False ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('x86_64 freebsd clang') {
            	     agent { label 'x86_64 && freebsd' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "x86_64 freebsd on" `hostname`
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_ASAN=False ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('aarch64 linux gcc-14') {
            	     agent { label 'aarch64 && ubuntu24' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "aarch64 gcc-14 on" `hostname`
			 export CC=gcc-14
			 export CXX=g++-14
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install ..
			 cmake -E time oomstaller ninja -j `nproc`
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('aarch64 linux clang-18') {
            	     agent { label 'aarch64 && ubuntu24' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "aarch64 clang-18 on" `hostname`
			 export CC=clang-18
			 export CXX=clang++-18
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_ASAN=True ..
			 cmake -E time oomstaller ninja -j `nproc`
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('arm32 linux gcc-12') {
            	     agent { label 'armv7 && debian12' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "arm32 gcc-12 on" `hostname`
			 export CC=gcc-12
			 export CXX=g++-12
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install ..
			 cmake -E time oomstaller ninja -j `nproc`
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('x86_64 windows vs2022') {
            	     agent { label 'windows11 && vs2022' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
		     	 bat """
			 call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 call "winbuild-msvc.bat" -DCMAKE_BUILD_TYPE=Release
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 ctest -j 4 --output-on-failure
			 exit /b %ERRORLEVEL%
			 """
		     }
		}

                stage('x86_64 windows clang') {
            	     agent { label 'windows11 && vs2022' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
		     	 bat """
			 call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 call "winbuild-clang.bat" -DCMAKE_BUILD_TYPE=Release
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 ctest -j 4 --output-on-failure
			 exit /b %ERRORLEVEL%
			 """
		     }
		}

                stage('x86_64 windows clang-cl') {
            	     agent { label 'windows11 && vs2022' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
		     	 bat """
			 call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 call "winbuild-clang-cl.bat" -DCMAKE_BUILD_TYPE=Release
			 exit /b %ERRORLEVEL%
			 """
		     }
		}

		/*
                stage('aarch64 macos gcc-13') {
            	     agent { label 'macos' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
			 eval "$(/opt/homebrew/bin/brew shellenv)"
			 export CC=gcc-13
			 export CXX=g++-13
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DBUILD_BENCH=True -DENABLE_INLINING=True ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j 8
			 '''
            	     }
                }
		*/

                stage('aarch64 macos clang-17') {
            	     agent { label 'macos' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
			 eval "$(/opt/homebrew/bin/brew shellenv)"
			 export CC=/opt/homebrew/opt/llvm@17/bin/clang-17
			 export CXX=/opt/homebrew/opt/llvm@17/bin/clang++
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DBUILD_BENCH=True -DENABLE_INLINING=True ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j 8
			 cd ..
 			 mkdir build2
			 cd build2
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install2 -DBUILD_LIBS=False -DBUILD_UTILS=False ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j 8
			 '''
            	     }
                }

                stage('i386 linux gcc-12') {
            	     agent { label 'i386 && debian12' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "i386 gcc-12 on" `hostname`
			 export CC=gcc-12
			 export CXX=g++-12
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('riscv linux gcc-13') {
            	     agent { label 'riscv && ubuntu23' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "riscv gcc-13 on" `hostname`
			 export CC=gcc-13
			 export CXX=g++-13
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install ..
			 cmake -E time oomstaller ninja -j `nproc`
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }
            }
        }
    }
}
