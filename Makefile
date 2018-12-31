all:
	@echo "                       .___.__  __   "
	@echo "___  ______   ____   __| _/|__|/  |_ "
	@echo "\  \/ / ___\_/ __ \ / __ | |  \   __\\"
	@echo " \   / /_/  >  ___// /_/ | |  ||  |  "
	@echo "  \_/\___  / \___  >____ | |__||__|  "
	@echo "    /_____/      \/     \/       "
	@echo
	@echo "Hi! See the readme for instructions on how to make this program for different supported platforms."
	@echo "In short, you can add \"-f Makefile.switch\" or \"-f Makefile.wiiu\" to build apps for either of those targets."
	@echo
	@echo "Defaulting to the PC build:"
	make -f Makefile.pc

clean:
	make -f Makefile.pc clean
