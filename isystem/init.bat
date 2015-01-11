rem Define system version variable "@__ver__"
def @__ver__ ""
rpn @__ver__ "ISystem II " @__master_ver__ strcat "." strcat @__slave_ver__ strcat

rem Set char and cursor color
rpn :mode_text @__video_mode__ "text" seq
set cursor_color "7"
rem set char_color "31"
set char_color "7"
goto :mode_vesa
:mode_text
set cursor_color "7"
set char_color "7"
:mode_vesa

rem Define system path variable "@__system__"
def @__system__ "DA:/isystem/"

rem Define application path variable "@__path__"
def @__path__ ";"
rpn @__path__ @__path__ @__system__ strcat "bins/" strcat ";" strcat

rem Go to window mode
rpn :is_window_mode @__video_mode__ "text" seq not
rem vmode vesa800_600
rem reboot
:is_window_mode

clear
