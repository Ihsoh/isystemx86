#System directory
MAKE_DIR 	VA:/ isystem

#Kernel loader directory
MAKE_DIR	VA:/isystem/ kernelldrs
MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrtm.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrtm.bin kernelldr/bin/kernelldrtm.bin
MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_640_480.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_640_480.bin kernelldr/bin/kernelldrgm_640_480.bin
MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_800_600.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_800_600.bin kernelldr/bin/kernelldrgm_800_600.bin
MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1024_768.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1024_768.bin kernelldr/bin/kernelldrgm_1024_768.bin
MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1280_1024.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1280_1024.bin kernelldr/bin/kernelldrgm_1280_1024.bin

#System command directory
MAKE_DIR	VA:/isystem/ bins

#Basic
MAKE_FILE	VA:/isystem/bins/ basic
WRITE_FILE	VA:/isystem/bins/basic apps/basic/bin/basic.bin

#Edit
MAKE_FILE	VA:/isystem/bins/ edit
WRITE_FILE	VA:/isystem/bins/edit apps/edit/bin/edit.bin

#Print text with format
MAKE_FILE	VA:/isystem/bins/ printf
WRITE_FILE	VA:/isystem/bins/printf apps/printf/bin/printf.bin

#IASM
MAKE_FILE	VA:/isystem/bins/ iasm
WRITE_FILE	VA:/isystem/bins/iasm apps/iasm/bin/iasm.bin

#Paint
MAKE_FILE	VA:/isystem/bins/ paint
WRITE_FILE	VA:/isystem/bins/paint apps/paint/bin/paint.bin

#WDVM
MAKE_FILE	VA:/isystem/bins/ wdvm
WRITE_FILE	VA:/isystem/bins/wdvm apps/WDVM-ISystemx86/bin/wdvm.bin

#Test1
MAKE_FILE	VA:/isystem/bins/ test1
WRITE_FILE	VA:/isystem/bins/test1 apps/test1/bin/test1.bin

#Test2
MAKE_FILE	VA:/isystem/bins/ test2
WRITE_FILE	VA:/isystem/bins/test2 apps/test2/bin/test2.bin

#CppTest
MAKE_FILE	VA:/isystem/bins/ cpptest
#WRITE_FILE	VA:/isystem/bins/cpptest apps/cpptest/bin/cpptest.bin

#WDVM application directory
MAKE_DIR	VA:/isystem/ wd
MAKE_FILE	VA:/isystem/wd/ fib.wd
WRITE_FILE	VA:/isystem/wd/fib.wd isystem/wd/fib.wd
MAKE_FILE	VA:/isystem/wd/ helloworld.wd
WRITE_FILE	VA:/isystem/wd/helloworld.wd isystem/wd/helloworld.wd
MAKE_FILE	VA:/isystem/wd/ a.wd
WRITE_FILE	VA:/isystem/wd/a.wd isystem/wd/a.wd

#System dump directory
MAKE_DIR	VA:/isystem/ dump

#System data directory
MAKE_DIR	VA:/isystem/ data

#Log directory
MAKE_DIR	VA:/isystem/data/ log

#System log file
MAKE_FILE	VA:/isystem/data/log/ system.log

#System resource directory
MAKE_DIR	VA:/isystem/ resources
MAKE_DIR	VA:/isystem/resources/ images

#Images
MAKE_FILE	VA:/isystem/resources/images/ pointer.img0
WRITE_FILE	VA:/isystem/resources/images/pointer.img0 isystem/resources/images/pointer.img0
MAKE_FILE	VA:/isystem/resources/images/ bg_640_480.img0
WRITE_FILE	VA:/isystem/resources/images/bg_640_480.img0 isystem/resources/images/bg_640_480.img0
MAKE_FILE	VA:/isystem/resources/images/ bg_800_600.img0
WRITE_FILE	VA:/isystem/resources/images/bg_800_600.img0 isystem/resources/images/bg_800_600.img0
MAKE_FILE	VA:/isystem/resources/images/ bg_1024_768.img0
WRITE_FILE	VA:/isystem/resources/images/bg_1024_768.img0 isystem/resources/images/bg_1024_768.img0
MAKE_FILE	VA:/isystem/resources/images/ bg_1280_1024.img0
WRITE_FILE	VA:/isystem/resources/images/bg_1280_1024.img0 isystem/resources/images/bg_1280_1024.img0
MAKE_FILE	VA:/isystem/resources/images/ close_button_ns.img0
WRITE_FILE	VA:/isystem/resources/images/close_button_ns.img0 isystem/resources/images/close_button_ns.img0
MAKE_FILE	VA:/isystem/resources/images/ close_button_s.img0
WRITE_FILE	VA:/isystem/resources/images/close_button_s.img0 isystem/resources/images/close_button_s.img0
MAKE_FILE	VA:/isystem/resources/images/ hidden_button_ns.img0
WRITE_FILE	VA:/isystem/resources/images/hidden_button_ns.img0 isystem/resources/images/hidden_button_ns.img0
MAKE_FILE	VA:/isystem/resources/images/ hidden_button_s.img0
WRITE_FILE	VA:/isystem/resources/images/hidden_button_s.img0 isystem/resources/images/hidden_button_s.img0

#System font directory
MAKE_DIR	VA:/isystem/ fonts
MAKE_FILE	VA:/isystem/fonts/ default.ef
WRITE_FILE	VA:/isystem/fonts/default.ef isystem/fonts/default.ef

#System flags directory
MAKE_DIR	VA:/isystem/ flags
MAKE_FILE	VA:/isystem/flags/ cfs.flg
WRITE_FILE	VA:/isystem/flags/cfs.flg isystem/flags/cfs.flg

#System init file
MAKE_FILE	VA:/isystem/ init.bat
WRITE_FILE	VA:/isystem/init.bat isystem/init.bat

#System test file
MAKE_FILE	VA:/isystem/ test.bat
WRITE_FILE	VA:/isystem/test.bat isystem/test.bat

#System help file
MAKE_FILE	VA:/isystem/ help.txt
WRITE_FILE	VA:/isystem/help.txt isystem/help.txt
