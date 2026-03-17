# To compile

In the build folder

`g++ -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused -o HomeTrainer.exe ..\src\main.cpp ..\lib\Bike.cpp ..\lib\Edge.cpp ..\lib\Route.cpp ..\lib\User.cpp ..\lib\Vertex.cpp && .\HomeTrainer.exe`

# Sources

[gpx documentation](https://www.topografix.com/GPX/1/1/gpx.xsd)

[GPX Studio](https://gpx.studio/app#15.23/47.998664/-2.489531)

[Bike power to speed calculation](https://www.gribble.org/cycling/power_v_speed.html)