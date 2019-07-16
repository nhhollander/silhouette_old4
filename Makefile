all: do_build docs

do_build:
	@printf "\033[33mCreating Directories...\033[0m\n"
	@mkdir -p build bin lib
	@printf "\033[33mConfiguring Project...\033[0m\n"
	@cd build; cmake ../
	@printf "\033[33mBuilding Project...\033[0m\n"
	@cd build; make -j 8 --no-print-directory

docs:
	@printf "\033[33mRunning Doxygen...\033[0m\n"
	@doxygen

clean:
	@printf "\033[33mDeleting build files...\033[0m\n"
	@rm -rdf build/*
	@printf "\033[33mDeleting binaries...\033[0m\n"
	@rm -rdf bin/*
	@printf "\033[33mDeleting libraries...\033[0m\n"
	@rm -rdf lib/*
	@printf "\033[33mDeleting generated documentation...\033[0m\n"
	@rm -rdf doc/generated/*

run: all
	@printf "\033[33mRunning Project...\033[0m\n"
	@./bin/se_test

debug: all
	@printf "\033[33mDebugging Project...\033[0m\n"
	@gdb ./bin/se_test

rebuild: clean all
	@printf "\033[33mRebuilt Project\033[0m\n"
