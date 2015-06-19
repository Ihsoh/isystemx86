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

#System window application directory
MAKE_DIR	VA:/isystem/ wbins

#Window application 'image'
MAKE_FILE	VA:/isystem/wbins/ image
WRITE_FILE	VA:/isystem/wbins/image wapps/image/bin/image.bin


#System commands directory
MAKE_DIR	VA:/isystem/ bins

#Command 'printf'
MAKE_FILE	VA:/isystem/bins/ printf
WRITE_FILE	VA:/isystem/bins/printf apps/printf/bin/printf.bin

#Command 'fwrite'
MAKE_FILE	VA:/isystem/bins/ fwrite
WRITE_FILE	VA:/isystem/bins/fwrite apps/fwrite/bin/fwrite.bin

#Command 'fappend'
MAKE_FILE	VA:/isystem/bins/ fappend
WRITE_FILE	VA:/isystem/bins/fappend apps/fappend/bin/fappend.bin

#Command 'test'
MAKE_FILE	VA:/isystem/bins/ test
WRITE_FILE	VA:/isystem/bins/test apps/test/bin/test.bin

#Basic
MAKE_FILE	VA:/isystem/bins/ basic
WRITE_FILE	VA:/isystem/bins/basic apps/basic/bin/basic.bin

#Edit
MAKE_FILE	VA:/isystem/bins/ edit
WRITE_FILE	VA:/isystem/bins/edit apps/edit/bin/edit.bin

#IASM
MAKE_FILE	VA:/isystem/bins/ iasm
WRITE_FILE	VA:/isystem/bins/iasm apps/iasm/bin/iasm.bin

#WDVM
MAKE_FILE	VA:/isystem/bins/ wdvm
WRITE_FILE	VA:/isystem/bins/wdvm apps/WDVM-ISystemx86/bin/wdvm.bin

#ubasic
MAKE_FILE	VA:/isystem/bins/ ubasic
WRITE_FILE	VA:/isystem/bins/ubasic apps/ubasic/bin/ubasic.bin

#lua1
MAKE_FILE	VA:/isystem/bins/ lua1
WRITE_FILE	VA:/isystem/bins/lua1 apps/lua1/bin/lua1.bin

#lua2
MAKE_FILE	VA:/isystem/bins/ lua2
WRITE_FILE	VA:/isystem/bins/lua2 apps/lua2/bin/lua2.bin
MAKE_FILE	VA:/isystem/bins/ lua2c
WRITE_FILE	VA:/isystem/bins/lua2c apps/lua2/bin/lua2c.bin

#WDVM application directory
MAKE_DIR	VA:/isystem/ wd
MAKE_FILE	VA:/isystem/wd/ fib.wd
WRITE_FILE	VA:/isystem/wd/fib.wd isystem/wd/fib.wd
MAKE_FILE	VA:/isystem/wd/ helloworld.wd
WRITE_FILE	VA:/isystem/wd/helloworld.wd isystem/wd/helloworld.wd
MAKE_FILE	VA:/isystem/wd/ a.wd
WRITE_FILE	VA:/isystem/wd/a.wd isystem/wd/a.wd

#System files directory
MAKE_DIR	VA:/isystem/ sys

MAKE_FILE	VA:/isystem/sys/ pci.sys
WRITE_FILE	VA:/isystem/sys/pci.sys sys/pci/bin/pci.bin

MAKE_FILE	VA:/isystem/sys/ screen.sys
WRITE_FILE	VA:/isystem/sys/screen.sys sys/screen/bin/screen.bin

#System dump directory
MAKE_DIR	VA:/isystem/ dump

#System data directory
MAKE_DIR	VA:/isystem/ data

#Log directory
MAKE_DIR	VA:/isystem/data/ log

#System log file
MAKE_FILE	VA:/isystem/data/log/ system.log

#Config directory
MAKE_DIR	VA:/isystem/data/ config

#System config file
MAKE_FILE	VA:/isystem/data/config/ system.json
WRITE_FILE	VA:/isystem/data/config/system.json isystem/data/config/system.json

#Lock directory
MAKE_DIR	VA:/isystem/ lock

MAKE_FILE	VA:/isystem/lock/ system.lock
WRITE_FILE	VA:/isystem/lock/system.lock isystem/lock/system.lock

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

#System init file
MAKE_FILE	VA:/isystem/ init.bat
WRITE_FILE	VA:/isystem/init.bat isystem/init.bat

#System test file
MAKE_FILE	VA:/isystem/ test.bat
WRITE_FILE	VA:/isystem/test.bat isystem/test.bat

#System help file
MAKE_FILE	VA:/isystem/ help.txt
WRITE_FILE	VA:/isystem/help.txt isystem/help.txt

#Test applications directory
MAKE_DIR	VA:/isystem/ test

#Testing jsonlib application
MAKE_FILE	VA:/isystem/test/ test_jsonlib
WRITE_FILE	VA:/isystem/test/test_jsonlib test/test_jsonlib/bin/test_jsonlib.bin

MAKE_FILE	VA:/isystem/ girl.bmp
WRITE_FILE	VA:/isystem/girl.bmp isystem/girl.bmp
