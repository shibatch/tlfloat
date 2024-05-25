pipeline {
    agent { label 'jenkinsfile' }

    stages {
        stage('Preamble') {
            parallel {
                stage('x86_64 linux gcc-11') {
            	     agent { label 'x86_64 && ubuntu22' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "x86_64 gcc-11 on" `hostname`
			 export CC=gcc-11
			 export CXX=g++-11
			 rm -rf build
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_VALGRIND=False -DENABLE_ASAN=True ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('x86_64 linux emscripten') {
            	     agent { label 'emscripten' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "emscripten on" `hostname`
			 rm -rf build
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DENABLE_ARCH_OPTIMIZATION=False -DSET_COMPILER_SUPPORTS_INT128=False -DSET_COMPILER_SUPPORTS_FLOAT128=False -DSET_LONGDOUBLE_IS_FLOAT128=False -DCMAKE_BUILD_TYPE=Release ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('x86_64 linux clang-18') {
            	     agent { label 'x86_64 && ubuntu22 && cuda' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "x86_64 clang-18 on" `hostname`
			 export CC=clang-18
			 export CXX=clang++-18
			 export CUDACXX=/opt/cuda-12.4/bin/nvcc
			 rm -rf build
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_CUDA_TEST=True ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('aarch64 linux gcc-11') {
            	     agent { label 'aarch64 && ubuntu22' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "aarch64 gcc-11 on" `hostname`
			 export CC=gcc-11
			 export CXX=g++-11
			 rm -rf build
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_VALGRIND=False ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('aarch64 linux clang-17') {
            	     agent { label 'aarch64 && ubuntu23' }
                     options { skipDefaultCheckout() }
            	     steps {
                         cleanWs()
                         checkout scm
	    	     	 sh '''
                	 echo "aarch64 clang-17 on" `hostname`
			 export CC=clang-17
			 export CXX=clang++-17
			 rm -rf build
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_VALGRIND=False -DENABLE_ASAN=True ..
			 cmake -E time ninja
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
            }
        }
    }
}
