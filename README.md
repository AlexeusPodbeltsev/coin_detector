# Coin Detection

Coin Detection Software for 2D Image Processing course

## Use

For *compiling* do the following:

	mkdir build
	cd build
	cmake ..
	make

And then *run* with

	./coin_detection [detect <filepath> | test <path_to_test_dir> <path_to_test_txt>]

You can use two modes. The first one is to detect coins. You simply write the command *detect* and provide path to the image. 
The second one is to test quality of detector. Write command *test* and paths to directory with images and test.txt file.

## Requirement for test.txt file
Your txt file must follow this structure: <br>
coin1.png <br>
x=*num* y=*num* radius=*num* <br>
x=*num* y=*num* radius=*num* <br>
x=*num* y=*num* radius=*num* <br>
...<br>

coin2.png <br>
x=*num* y=*num* radius=*num* <br>
x=*num* y=*num* radius=*num* <br>
x=*num* y=*num* radius=*num* <br>
... <br>
end