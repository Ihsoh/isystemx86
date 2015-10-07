#System directory
MAKE_DIR 	VA:/ isystem

#Kernel loader directory
MAKE_DIR	VA:/isystem/ kernelldrs

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrtm.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrtm.bin kernelldr/bin/kernelldrtm.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_320_200.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_320_200.bin kernelldr/bin/kernelldrgm_320_200.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_640_400.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_640_400.bin kernelldr/bin/kernelldrgm_640_400.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_640_480.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_640_480.bin kernelldr/bin/kernelldrgm_640_480.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_800_500.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_800_500.bin kernelldr/bin/kernelldrgm_800_600.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_800_600.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_800_600.bin kernelldr/bin/kernelldrgm_800_600.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_896_672.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_896_672.bin kernelldr/bin/kernelldrgm_896_672.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1024_640.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1024_640.bin kernelldr/bin/kernelldrgm_1024_640.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1024_768.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1024_768.bin kernelldr/bin/kernelldrgm_1024_768.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1152_720.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1152_720.bin kernelldr/bin/kernelldrgm_1152_720.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1280_1024.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1280_1024.bin kernelldr/bin/kernelldrgm_1280_1024.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1440_900.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1440_900.bin kernelldr/bin/kernelldrgm_1440_900.bin

MAKE_FILE	VA:/isystem/kernelldrs/ kernelldrgm_1600_1200.bin
WRITE_FILE	VA:/isystem/kernelldrs/kernelldrgm_1600_1200.bin kernelldr/bin/kernelldrgm_1600_1200.bin


#System window application directory
MAKE_DIR	VA:/isystem/ wbins

#Window application 'image'
MAKE_FILE	VA:/isystem/wbins/ image
WRITE_FILE	VA:/isystem/wbins/image wapps/image/bin/image.bin

#Window application 'calc'
MAKE_FILE	VA:/isystem/wbins/ calc
WRITE_FILE	VA:/isystem/wbins/calc wapps/calc/bin/calc.bin


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

MAKE_FILE	VA:/isystem/sys/ timer.sys
WRITE_FILE	VA:/isystem/sys/timer.sys sys/timer/bin/timer.bin

#System dump directory
MAKE_DIR	VA:/isystem/ dump

#Log directory
MAKE_DIR	VA:/isystem/ log

#System log file
MAKE_FILE	VA:/isystem/log/ system.log

#Config directory
MAKE_DIR	VA:/isystem/ config

#System config file
MAKE_FILE	VA:/isystem/config/ system.json
WRITE_FILE	VA:/isystem/config/system.json isystem/config/system.json

#GUI config file
MAKE_FILE	VA:/isystem/config/ gui.json
WRITE_FILE	VA:/isystem/config/gui.json isystem/config/gui.json

#Lock directory
MAKE_DIR	VA:/isystem/ lock

MAKE_FILE	VA:/isystem/lock/ system.lock
WRITE_FILE	VA:/isystem/lock/system.lock isystem/lock/system.lock

#System resource directory
MAKE_DIR	VA:/isystem/ resources
MAKE_DIR	VA:/isystem/resources/ images

MAKE_FILE	VA:/isystem/resources/images/ bg640x480.bmp
WRITE_FILE	VA:/isystem/resources/images/bg640x480.bmp res/bmp/bg640x480.bmp
MAKE_FILE	VA:/isystem/resources/images/ bg800x600.bmp
WRITE_FILE	VA:/isystem/resources/images/bg800x600.bmp res/bmp/bg800x600.bmp
MAKE_FILE	VA:/isystem/resources/images/ bg1024x768.bmp
WRITE_FILE	VA:/isystem/resources/images/bg1024x768.bmp res/bmp/bg1024x768.bmp
MAKE_FILE	VA:/isystem/resources/images/ bg1280x1024.bmp
WRITE_FILE	VA:/isystem/resources/images/bg1280x1024.bmp res/bmp/bg1280x1024.bmp

MAKE_FILE	VA:/isystem/resources/images/ closebutton.bmp
WRITE_FILE	VA:/isystem/resources/images/closebutton.bmp res/bmp/closebutton.bmp
MAKE_FILE	VA:/isystem/resources/images/ closebuttonhover.bmp
WRITE_FILE	VA:/isystem/resources/images/closebuttonhover.bmp res/bmp/closebuttonhover.bmp
MAKE_FILE	VA:/isystem/resources/images/ closebuttonblur.bmp
WRITE_FILE	VA:/isystem/resources/images/closebuttonblur.bmp res/bmp/closebuttonblur.bmp
MAKE_FILE	VA:/isystem/resources/images/ hiddenbutton.bmp
WRITE_FILE	VA:/isystem/resources/images/hiddenbutton.bmp res/bmp/hiddenbutton.bmp
MAKE_FILE	VA:/isystem/resources/images/ hiddenbuttonhover.bmp
WRITE_FILE	VA:/isystem/resources/images/hiddenbuttonhover.bmp res/bmp/hiddenbuttonhover.bmp
MAKE_FILE	VA:/isystem/resources/images/ hiddenbuttonblur.bmp
WRITE_FILE	VA:/isystem/resources/images/hiddenbuttonblur.bmp res/bmp/hiddenbuttonblur.bmp

MAKE_FILE	VA:/isystem/resources/images/ pointer.bmp
WRITE_FILE	VA:/isystem/resources/images/pointer.bmp res/bmp/pointer.bmp

#System font directory
MAKE_DIR	VA:/isystem/ fonts
MAKE_FILE	VA:/isystem/fonts/ default.ef
WRITE_FILE	VA:/isystem/fonts/default.ef isystem/fonts/default.ef
MAKE_FILE	VA:/isystem/fonts/ consola.efx
WRITE_FILE	VA:/isystem/fonts/consola.efx isystem/fonts/consola.efx
MAKE_FILE	VA:/isystem/fonts/ consolab.efx
WRITE_FILE	VA:/isystem/fonts/consolab.efx isystem/fonts/consolab.efx
MAKE_FILE	VA:/isystem/fonts/ consolai.efx
WRITE_FILE	VA:/isystem/fonts/consolai.efx isystem/fonts/consolai.efx
MAKE_FILE	VA:/isystem/fonts/ consolaz.efx
WRITE_FILE	VA:/isystem/fonts/consolaz.efx isystem/fonts/consolaz.efx

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
