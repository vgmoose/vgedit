all:
	@echo "                       .___.__  __   \n___  ______   ____   __| _/|__|/  |_ \n\  \/ / ___\_/ __ \ / __ | |  \   __\\n \   / /_/  >  ___// /_/ | |  ||  |  \n  \_/\___  / \___  >____ | |__||__|  \n    /_____/      \/     \/       "
	@echo "\nHi! See the readme for instructions on how to make this program for different supported platforms."
	@echo "In short, you can add \"-f Makefile.switch\" or \"-f Makefile.wiiu\" to build apps for either of those targets."
	@echo "\nDefaulting to the PC build:"
	make -f Makefile.pc

clean:
	make -f Makefile.pc clean
