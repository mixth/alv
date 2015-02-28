Autonomous Land Vehicle v2
=====
ALV v2 is a bachelor's degree senior project at Faculty of Engineering, Mahidol University, Thailand. It was based on the original ALV, which is on the master branch. The v2 is being aimed to modify the late version and add a feature to follow the specific image. v2 project also changed the overall approach to be easier to add more modules and make them run concurrently. 



Installation
-----
0. Install necessary packages

   ```
   $ sudo apt-get install cmake libopencv-dev libboost-all-dev
   ```
1. Build Raspberry Pi "userland" project
   ```
   $ mkdir -p /home/pi/src/raspberrypi
   
   $ cd /home/pi/src/raspberrypi
   
   $ git clone --depth 1 https://github.com/raspberrypi/userland.git
   
   $ make -C /opt/vc/src/hello_pi/libs/vgfont
   ```
2. Get and build WiringPi project
   ```   
   $ mkdir -p /home/pi/src/wiringpi
   
   $ cd /home/pi/src/wiringpi
   
   $ git clone git://git.drogon.net/wiringPi
   
   $ cd wiringPi

   $ ./build
   ```
3. Get and build the ALV project
   ```
   $ mkdir -p /home/pi/src/alv
   
   $ cd /home/pi/src/alv
   
   $ git clone https://github.com/mixth/alv.git
   
   $ cd alv
   
   $ mkdir build
   
   $ cd build
   
   $ cmake ../
   
   $ make
   ```
4. To test the program: move test image (Space Invaders) and create follower_config
   ```
   $ cd /home/pi/src/alv
   
   $ mv invader-space-invert-resize.jpg build/
   
   $ cd build
   
   $ echo "1 0" > follower_config
   ```
