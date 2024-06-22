.SILENT:
.PHONY: run clean

run:
	if [ ! -d "build" ]; then mkdir build; fi
	cd build && cmake ..
	cp imgui.ini build/
	run

clean:
	rm -rf build

