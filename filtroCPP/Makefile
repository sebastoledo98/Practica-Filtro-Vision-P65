filtro:
	#g++ Principal.cpp -I/usr/include/opencv4 -o vision.bin
	clang++ filtro.cpp -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio -o vision.bin

filtro-run:
	./vision.bin

ascii:
	clang++ asciiMono.cpp -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio -o asciiMono.bin

ascii-run:
	./asciiMono.bin

color:
	clang++ asciiColor.cpp -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio -o asciiColor.bin

color-run:
	./asciiColor.bin
