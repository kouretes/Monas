

[![Kouretes](http://www.intelligence.tuc.gr/kouretes/web/templates/ja_purity/images/logo.png)](http://www.kouretes.gr)

[![ISL](http://www.intelligence.tuc.gr/images/intelligence.gif)](http://www.intelligence.tuc.gr/) [![TUC](http://www.tuc.gr/fileadmin/templates/bootstrap/imgs/logo-en.png)](http://www.tuc.gr/5397.html)

### __Requirements__

###### Linux:

  * Install the latest ***Ubuntu 32-bit LTS*** including all updates
  * Follow the instructions [here](https://help.github.com/articles/generating-an-ssh-key/) to add and verify your ssh key and link your Github account.

###### Packages:

  * Install the following packages using a package manager or ``sudo apt-get install``
    ```
    git cmake cmake-qt-gui libboost-all-dev libtinyxml-dev qt4-designer
    ```
    ***Note:*** [OpenCV](https://help.ubuntu.com/community/OpenCV) is required only for Tools.


###### Kouretes ***Monas***:

  * Create a folder ``Kouretes`` in ``$HOME`` (or use a folder of your choice)
  * Inside the folder ``Kouretes`` clone ***Monas*** by running:
    ```
    git clone https://github.com/kouretes/Monas.git
    ```
  * Folder ``Monas`` should appear inside folder ``Kouretes``.

  ***Note:*** Alternatively you can unpack the file ***Monas.zip*** (obtained from Github) inside folder ``Kouretes``.


###### Google Protobuf (You can skip this one, for reference only):

  * In folder ``/external/protobuf`` run:
    ```
    ./configure
    make
    ```
  * In folder ``/src/messages`` run:
    ```
    ./../../external/protobuf/src/protoc --cpp_out=. *.proto
    ```

### __Building and Linking (for local execution):__

  * Navigate to ``Kouretes/Monas/make`` and create a folder ``local``.

  * Inside the folder ``local`` run ``cmake-gui``
    - Source path: ``$HOME/Kouretes/Monas``
    - Build path: ``$HOME/Kouretes/Monas/make/local``
    - Press Configure
    - Choose generator "Unix Makefiles"
    - Choose "Use default native compilers"
    - Press Finish
    - Change TargetPlatform to linux
    - Change TargetRobot to host
    - Press Configure and repeat until everything is resolved
    - Install missing packages if necessary
    - Press Configure and repeat until everything is resolved
    - Press Generate
    - Exit


  * Inside the folder ``local`` run ``make -j4 install`` (change **4** to your number of CPU cores).

  * Go get a coffee and be patient...

  * Enter ``binaries/linux`` and run:
    ```
    export LD_LIBRARY_PATH=./lib
    ./bin/Monas
    ```
  * **WARNING**: Remove (comment out) from the ``agent.xml`` file all activities that
    won't compile when target is linux e.g., *Vision*, *MotionController*, etc.

### __Crossbuild (for on-board execution):__

  * Unpack the latest ``nao-cross-toolchain-XX.YY.ZZ.tar.gz``
    to folder ``Kouretes/nao-cross-toolchain-XX.YY.ZZ``

  * Navigate to ``Kouretes/Monas/make`` and create a folder ``cross``

  * Inside the folder ``cross`` run ``cmake-gui``
    - Source path: ``$HOME/Kouretes/Monas``
    - Build path: ``$HOME/Kouretes/Monas/make/cross``
    - Press Configure
    - Choose generator "Unix Makefiles"
    - Choose "Specify toolchain file for cross-compiling"
    - Press Next
    - Toolchain path: ``$HOME/Kouretes/nao-cross-toolchain-XX.YY.ZZ/toolchain-geode.cmake``
    - Press Finish
    - Change TargetPlatform to linux
    - Change TargetRobot to nao
    - Press Configure and repeat until everything is resolved
    - Install missing packages if necessary
    - Press Configure and repeat until everything is resolved
    - Press Generate
    - Exit


  * Inside the folder ``cross`` run ``make -j4 install`` (change **4** to your number of CPU cores)

  * Get another coffee and be even more patient...

Use the python scripts in the ``script`` directory to upload code to a robot. To do so, you need a valid ``config`` directory in ``/home/nao/naoqi/`` (on the robot)
and a valid ``autoload.ini`` script in ``/home/nao/naoqi/preferences/autoload.ini`` (on the robot). These are required to start the *krobot* module.

### __Flash Disk Format:__

  * Lookup ``/media/<user>/OpenNao-system/usr/share/opennao/core-version.txt``
    to find the current *NaoQi* ***version*** on your USB.
  * Unpack *NaoQi* SDK ``naoqi-sdk-XX.YY.ZZ-linux.tar.gz``
  * Flash *Nao* USB Keys using ``NaoQi XX.YY.ZZ``.
  * Instructions in ``$SDK/doc/site_en/reddoc/software_update/flash_process.html``.
  * Basically you run:
    ```
    sudo $SDK/bin/flash-usbstick nao-system-image-robocup-XX.YY.ZZ.gz
    ```
