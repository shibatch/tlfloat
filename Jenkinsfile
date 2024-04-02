pipeline {
    agent { label 'jenkinsfile' }

    stages {
        stage('Preamble') {
            parallel {
                stage('x86_64 linux gcc-11') {
            	     agent { label 'x86_64 && ubuntu22' }
            	     steps {
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

                stage('x86_64 linux clang-18') {
            	     agent { label 'x86_64 && ubuntu22 && cuda' }
            	     steps {
	    	     	 sh '''
                	 echo "x86_64 clang-18 on" `hostname`
			 export CC=clang-18
			 export CXX=clang++-18
			 export CUDACXX=/opt/cuda-12.4/bin/nvcc
			 rm -rf build
 			 mkdir build
			 cd build
			 cmake -GNinja -DCMAKE_INSTALL_PREFIX=../../install -DENABLE_CUDA_TESTER=True ..
			 cmake -E time ninja
		         export CTEST_OUTPUT_ON_FAILURE=TRUE
		         ctest -j `nproc`
			 '''
            	     }
                }

                stage('aarch64 linux gcc-11') {
            	     agent { label 'aarch64 && ubuntu22' }
            	     steps {
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
            	     steps {
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

		/*
                stage('x86_64 windows vs2022') {
            	     agent { label 'windows11 && vs2022' }
            	     steps {
		     	 bat """
			 call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 call "winbuild-msvc.bat" -DENABLE_WIX=TRUE -DSUPPRESS_WIX_VALIDATION=TRUE
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 ctest -j 4 --output-on-failure
			 exit /b %ERRORLEVEL%
			 """
		     }
		}
		*/

                stage('x86_64 windows clang') {
            	     agent { label 'windows11 && vs2022' }
            	     steps {
		     	 bat """
			 call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
			 if not %ERRORLEVEL% == 0 exit /b %ERRORLEVEL%
			 call "winbuild-clang.bat"
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
